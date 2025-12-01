#include <stdint.h>
#include "img.h"
#include "kernels.h"
#include "conv.h"
#include "dtekv-lib.h"
#include "imageproc.h"
#include "vga.h"
#include "perf.h"

/* Reuse Lab 3 I/O + timing functions (defined in labmain.c / timetemplate.S) */
extern void set_leds(int led_mask);
extern int  get_sw(void);
extern int  get_bt(void);
extern void delay(int);

/* --------------------------------------------------------------------------
   Upload/download configuration (UART via dtekv-upload / dtekv-download)

   We assume a 320x240 8-bit grayscale RAW file (76800 bytes) for upload/download.
   Pixels are in row-major order, one byte per pixel.

   Chosen RAM addresses (must fit your memory map):

     INPUT_BASE_ADDR  = 0x01000000  (for dtekv-upload)
     OUTPUT_BASE_ADDR = 0x01010000  (for dtekv-download)

   Example host commands:

     # upload input image
     dtekv-upload input.raw 0x01000000

     # after pressing KEY1 and LED9 is lit, download result
     dtekv-download output.raw 0x01010000 76800
 -------------------------------------------------------------------------- */

#define UPLOAD_W         320u
#define UPLOAD_H         240u
#define BYTES_PER_PIXEL  1u

#define UPLOAD_PIXELS    (UPLOAD_W * UPLOAD_H)           /* 76800 */
#define UPLOAD_BYTES     (UPLOAD_PIXELS * BYTES_PER_PIXEL)

#define INPUT_BASE_ADDR   0x01000000u
#define OUTPUT_BASE_ADDR  0x01010000u

#define INPUT_MEM   ((volatile uint8_t *)INPUT_BASE_ADDR)
#define OUTPUT_MEM  ((volatile uint8_t *)OUTPUT_BASE_ADDR)

/* --------------------------------------------------------------------------
   Image buffers
   -------------------------------------------------------------------------- */

static Image img_in;
static Image img_tmp;
static Image img_out;

/* Copy image data */
static void img_copy(const Image *src, Image *dst) {
    dst->w = src->w;
    dst->h = src->h;
    uint32_t n = (uint32_t)src->w * (uint32_t)src->h;
    for (uint32_t i = 0; i < n; ++i) {
        dst->data[i] = src->data[i];
    }
}

/* Simple checksum/debug: sum of all pixels */
static uint32_t img_checksum(const Image *img) {
    uint32_t n = (uint32_t)img->w * (uint32_t)img->h;
    uint32_t sum = 0;
    for (uint32_t i = 0; i < n; ++i) {
        sum += img->data[i];
    }
    return sum;
}

/* --------------------------------------------------------------------------
   Upload/download backend (dtekv-upload / dtekv-download)
   -------------------------------------------------------------------------- */

/* Load 320x240 RAW grayscale from INPUT_MEM into img_in */
static void load_input_image_from_upload(Image *img) {
    img->w = (uint16_t)UPLOAD_W;
    img->h = (uint16_t)UPLOAD_H;

    uint32_t n = (uint32_t)UPLOAD_PIXELS;  /* always 76800 for now */

    for (uint32_t i = 0; i < n; ++i) {
        img->data[i] = INPUT_MEM[i];
    }
}

/* Save processed image to OUTPUT_MEM so host can dtekv-download it */
static void save_output_image_to_download(const Image *img) {
    uint32_t n = (uint32_t)img->w * (uint32_t)img->h;

    /* Safety: clamp to 320x240 if something goes weird */
    if (n > (uint32_t)UPLOAD_PIXELS) {
        n = (uint32_t)UPLOAD_PIXELS;
    }

    for (uint32_t i = 0; i < n; ++i) {
        OUTPUT_MEM[i] = img->data[i];
    }
}

/* --------------------------------------------------------------------------
   Switch → filter/size/chaining mapping
   -------------------------------------------------------------------------- */

/*
   Interpretation of switches (SWx = physical switch x):

   SW1:0 = filter 1 selection (2 bits)
   SW3:2 = filter 2 selection (2 bits)
   SW4   = chain enable (0 = only F1, 1 = F1 -> F2)
   SW5   = size bit: 0 = 3x3, 1 = 5x5 (Gauss uses 5x5; others ignore size)
   SW8   = if set, force filter1 = EMBOSS (override SW1:0)

   (SW7:6, SW9, etc. are currently unused.)

   Mapping of 2-bit codes to FilterType:
     00 -> FILTER_IDENTITY
     01 -> FILTER_SHARPEN
     10 -> FILTER_GAUSS   (5x5 Gaussian)
     11 -> FILTER_EDGE
*/

typedef struct {
    FilterType f1;
    FilterType f2;
    int size1;       /* 3 or 5 */
    int size2;       /* 3 or 5 */
    int chain_on;    /* 0 or 1 */
} Selection;

static Selection decode_switches(uint32_t sw) {
    Selection s;

    uint32_t code1 =  sw        & 0x3u;    /* bits 1:0 */
    uint32_t code2 = (sw >> 2)  & 0x3u;    /* bits 3:2 */
    uint32_t chain = (sw >> 4)  & 0x1u;    /* bit 4   */
    uint32_t sizeb = (sw >> 5)  & 0x1u;    /* bit 5   */

    /* 2-bit code -> filter */
    static const FilterType table[4] = {
        FILTER_IDENTITY,   /* 0b00 */
        FILTER_SHARPEN,    /* 0b01 */
        FILTER_GAUSS,      /* 0b10 */
        FILTER_EDGE        /* 0b11 */
    };

    s.f1 = table[code1];
    s.f2 = table[code2];

    /* Emboss override (SW8) */
    if (sw & (1u << 8)) {
        s.f1 = FILTER_EMBOSS;
    }

    s.chain_on = (int)chain;

    /* Size logic:
       - SW5 = 0 → everyone is 3x3
       - SW5 = 1 → GAUSS uses 5x5, others still 3x3
    */
    s.size1 = (s.f1 == FILTER_GAUSS && sizeb) ? 5 : 3;
    s.size2 = (s.f2 == FILTER_GAUSS && sizeb) ? 5 : 3;

    return s;
}


/* --------------------------------------------------------------------------
   Main image-processing loop (called from labmain.c: main)
   -------------------------------------------------------------------------- */

void imageproc_main(void) {
    uint32_t last_sw = 0xFFFFFFFFu;
    int last_bt = 0;

    print("Image processing demo (upload + VGA + UART, fixed 320x240)...\n");
    print("Switch map:\n");
    print("  SW1:0  = Filter 1\n");
    print("  SW3:2  = Filter 2\n");
    print("  SW4    = Chain (0=F1 only, 1=F1->F2)\n");
    print("  SW5    = Kernel size (0=3x3, 1=5x5)\n");
    print("  SW8    = Emboss override for Filter 1\n");
    print("  (Other switches unused for now)\n");
    print("\n");
    print("Upload/download usage:\n");
    print("  dtekv-upload input.raw 0x01000000   # 320x240 RAW grayscale (76800 bytes)\n");
    print("  [set filters with switches]\n");
    print("  [press KEY1 once]\n");
    print("  dtekv-download output.raw 0x01010000 76800\n");
    print("\n");

    while (1) {
        uint32_t sw = (uint32_t)get_sw();
        int bt = get_bt() ? 1 : 0;

        /* Mirror switches on LEDs (without done-flag) as live status */
        if (sw != last_sw) {
            set_leds((int)sw);
            last_sw = sw;
        }

        /* Rising edge on button -> run filters once */
        if (bt && !last_bt) {
            Selection sel = decode_switches(sw);

            print("\n=== New processing run ===\n");
            print("SW = 0x");
            print_hex32(sw);
            print("\n");

            print("Filter 1: ");
            print_dec((unsigned)sel.f1);
            print("  size=");
            print_dec((unsigned)sel.size1);
            print("\n");

            if (sel.chain_on) {
                print("Filter 2: ");
                print_dec((unsigned)sel.f2);
                print("  size=");
                print_dec((unsigned)sel.size2);
                print("\n");
            } else {
                print("Filter 2: [disabled]\n");
            }

            /* Load input image from uploaded buffer */
            load_input_image_from_upload(&img_in);

            /* --- Measure just the core image-processing work --- advanced project */
            PerfCounters c;
            clear_counters();

            /* First filter: img_in -> img_tmp */
            const Kernel *k1 = get_kernel(sel.f1, sel.size1);
            convolve(&img_in, &img_tmp, k1);

            /* Optional second filter: img_tmp -> img_out */
            if (sel.chain_on) {
                const Kernel *k2 = get_kernel(sel.f2, sel.size2);
                convolve(&img_tmp, &img_out, k2);
            } else {
                img_copy(&img_tmp, &img_out);
            }

            read_counters(&c); // advanced project

            print("Perf counters:\n");
            print("  mcycle = ");      print_dec(c.mcycle);        print("\n");
            print("  minstret = ");    print_dec(c.minstret);      print("\n");
            print("  mem instr = ");   print_dec(c.mhpm3_mem);     print("\n");
            print("  I-miss = ");      print_dec(c.mhpm4_ic_miss); print("\n");
            print("  D-miss = ");      print_dec(c.mhpm5_dc_miss); print("\n");
            print("  I-stall = ");     print_dec(c.mhpm6_ic_stall); print("\n");
            print("  D-stall = ");     print_dec(c.mhpm7_dc_stall); print("\n");
            print("  Dhaz-stall = ");  print_dec(c.mhpm8_dhaz_stall); print("\n");
            print("  ALU-stall = ");   print_dec(c.mhpm9_alu_stall);  print("\n");

            /* Compute checksum of final image and print it */
            uint32_t sum = img_checksum(&img_out);
            print("Output checksum = 0x");
            print_hex32(sum);
            print("\n");

            /* Save processed image to RAM for dtekv-download */
            save_output_image_to_download(&img_out);

            /* Draw processed image on VGA screen */
            vga_clear(0);
            vga_draw_image_centered(&img_out);

            /* Turn on LED9 as "done" flag, plus preserve switch bits */
            set_leds((int)(sw | (1u << 9)));

            /* Simple debounce so holding button doesn't retrigger immediately */
            delay(150);
        }

        last_bt = bt;
    }
}
