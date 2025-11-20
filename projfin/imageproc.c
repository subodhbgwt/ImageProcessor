#include <stdint.h>
#include "img.h"
#include "kernels.h"
#include "conv.h"
#include "dtekv-lib.h"
#include "imageproc.h"
#include "images.h"
#include "vga.h"

/* Reuse Lab 3 I/O + timing functions (defined in labmain.c / timetemplate.S) */
extern void set_leds(int led_mask);
extern int  get_sw(void);
extern int  get_bt(void);
extern void delay(int);

/* --------------------------------------------------------------------------
   Send image as ASCII PGM over JTAG UART
   -------------------------------------------------------------------------- */

static void send_image_pgm(const Image *img) {
    print("\n--- BEGIN_PGM ---\n");
    print("P2\n");                        // PGM magic
    print_dec(img->w);
    print(" ");
    print_dec(img->h);
    print("\n255\n");                      // max gray

    for (uint16_t y = 0; y < img->h; ++y) {
        for (uint16_t x = 0; x < img->w; ++x) {
            uint8_t v = img->data[y * img->w + x];
            print_dec(v);
            if (x + 1 < img->w) {
                print(" ");
            }
        }
        print("\n");
    }

    print("--- END_PGM ---\n");
}

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

static void load_builtin_image(int index, Image *dst) {
    if (index < 0 || index >= num_builtin_images) {
        index = 0;
    }

    const ImageDef *src = &builtin_images[index];

    dst->w = src->w;
    dst->h = src->h;

    uint32_t n = (uint32_t)src->w * (uint32_t)src->h;
    for (uint32_t i = 0; i < n; ++i) {
        dst->data[i] = src->data[i];
    }
}

static void init_input_image(Image *img, uint32_t sw) {
    /* Example: use SW7:6 to select which built-in image to use */
    int idx = (int)((sw >> 6) & 0x3u);  // 0..3
    load_builtin_image(idx, img);
}

/* --------------------------------------------------------------------------
   Switch → filter/size/chaining mapping
   -------------------------------------------------------------------------- */

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
    s.size1 = sizeb ? 5 : 3;
    s.size2 = sizeb ? 5 : 3;

    return s;
}

/* --------------------------------------------------------------------------
   Main image-processing loop (called from labmain.c: main)
   -------------------------------------------------------------------------- */

void imageproc_main(void) {
    uint32_t last_sw = 0xFFFFFFFFu;
    int last_bt = 0;

    print("Image processing demo starting (Lab3 base)...\n");

    /* Initialize the input image once, based on switches at startup */
    uint32_t sw0 = (uint32_t)get_sw();
    init_input_image(&img_in, sw0);

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

            /* Compute checksum of final image and print it */
            uint32_t sum = img_checksum(&img_out);
            print("Output checksum = 0x");
            print_hex32(sum);
            print("\n");

            /* Send processed image back to PC as PGM */
            send_image_pgm(&img_out);

            /* NEW: draw processed image to VGA */
            vga_clear(0);                         // clear to black first (optional)
            vga_draw_image_centered(&img_out);

            /* Turn on LED9 as "done" flag, plus preserve switch bits */
            set_leds((int)(sw | (1u << 9)));

            /* Simple debounce so holding button doesn't retrigger immediately */
            delay(150);
        }

        last_bt = bt;
    }
}
