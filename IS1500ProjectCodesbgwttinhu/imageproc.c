/*
   Upload/download configuration (UART via dtekv-upload / dtekv-download)

   We assume a 320x240 8-bit grayscale RAW file (76800 bytes) for upload/download.
   Pixels are in row-major order, one byte per pixel.

   Chosen RAM addresses (must fit your memory map):

     INPUT_BASE_ADDR  = 0x01000000  (for dtekv-upload)
     OUTPUT_BASE_ADDR = 0x01030000  (for dtekv-download; non-overlapping)

   Example host commands:

     # upload input image
     dtekv-upload input.raw 0x01000000

     # after pressing KEY1 and LED9 is lit, download result
     dtekv-download output.raw 0x01030000 76800

   Interpretation of switches (SWx = physical switch x):

   SW1:0 = filter 1 selection (2 bits)
   SW3:2 = filter 2 selection (2 bits)
   SW4   = chain enable (0 = only F1, 1 = F1 -> F2)
   SW8   = if set, force filter1 = EMBOSS (override SW1:0)

   Mapping of 2-bit codes to FilterType:
     00 -> FILTER_IDENTITY
     01 -> FILTER_SHARPEN
     10 -> FILTER_GAUSS   (5x5 Gaussian)
     11 -> FILTER_EDGE
*/

#include <stdint.h>
#include "img.h"
#include "kernels.h"
#include "conv.h"
#include "dtekv-lib.h"
#include "imageproc.h"
#include "vga.h"
#include "perf.h"

// Reuse Lab 3 I/O
extern void set_leds(int led_mask);
extern int get_sw(void);
extern int get_bt(void);
extern void delay(int);

// Upload/download image parameters:

#define UPLOAD_W 320u // unsigned int to prevent possible overflows
#define UPLOAD_H 240u
#define BYTES_PER_PIXEL 1u

#define UPLOAD_PIXELS (UPLOAD_W * UPLOAD_H) // 76800 //
#define UPLOAD_BYTES (UPLOAD_PIXELS * BYTES_PER_PIXEL)

#define INPUT_BASE_ADDR 0x01000000u  // address for dtekv-upload
#define OUTPUT_BASE_ADDR 0x01030000u // address for dtekv-download (aligned, no overlap)

// Image Buffers from image struct:
static Image img_in;
static Image img_tmp;
static Image img_out;

#define INPUT_MEM ((volatile uint8_t *)INPUT_BASE_ADDR)   // pointer to input image data
#define OUTPUT_MEM ((volatile uint8_t *)OUTPUT_BASE_ADDR) // pointer to output image data

// Copy image data from src to dst
static void img_copy(const Image *src, Image *dst)
{
    (*dst).w = (*src).w; // dereference struct members
    (*dst).h = (*src).h;
    uint32_t n = (uint32_t)(*src).w * (uint32_t)(*src).h; // total pixels to copy
    for (uint32_t i = 0; i < n; ++i)                      // loop over all pixels
    {
        (*dst).data[i] = (*src).data[i]; // copy pixel data
    }
}

// Simple checksum/debug: sum of all pixels
static uint32_t img_checksum(const Image *img)
{
    uint32_t n = (uint32_t)(*img).w * (uint32_t)(*img).h; // total pixels to sum
    uint32_t sum = 0;                                     // initialize sum
    for (uint32_t i = 0; i < n; ++i)                      // loop over all pixels
    {
        sum += (*img).data[i]; // accumulate pixel values
    }
    return sum;
}

// Load 320x240 RAW grayscale from INPUT_MEM into img_in
static void load_input_image_from_upload(Image *img)
{
    (*img).w = (uint16_t)UPLOAD_W; // set image width
    (*img).h = (uint16_t)UPLOAD_H; // set image height

    uint32_t n = (uint32_t)UPLOAD_PIXELS; // always 76800 bytes.

    for (uint32_t i = 0; i < n; ++i) // loop over all pixels
    {
        (*img).data[i] = INPUT_MEM[i]; // load pixel data from INPUT_MEM
    }
}

// Save processed image to OUTPUT_MEM so we can dtekv-download it
static void save_output_image_to_download(const Image *img)
{
    uint32_t n = (uint32_t)(*img).w * (uint32_t)(*img).h; // total pixels in output image

    // Clamp to 320x240
    if (n > (uint32_t)UPLOAD_PIXELS)
    {
        n = (uint32_t)UPLOAD_PIXELS;
    }

    for (uint32_t i = 0; i < n; ++i) // loop over all pixels
    {
        OUTPUT_MEM[i] = (*img).data[i]; // save pixel data to OUTPUT_MEM
    }
}

typedef struct // filter selection based on the selection struct
{
    FilterType f1; // first filter
    FilterType f2; // second filter
    int chain_on;  // 0 or 1
} Selection;

// Decode switches into Selection struct
static Selection decode_switches(uint32_t sw)
{
    Selection s; // output struct

    uint32_t code1 = sw & 0x3u;        // bits 1:0
    uint32_t code2 = (sw >> 2) & 0x3u; // bits 3:2
    uint32_t chain = (sw >> 4) & 0x1u; // bit 4

    // 2-bit code -> filter
    static const FilterType table[4] = {
        FILTER_IDENTITY, // 0b00
        FILTER_SHARPEN,  // 0b01
        FILTER_GAUSS,    // 0b10
        FILTER_EDGE      // 0b11
    };

    s.f1 = table[code1];
    s.f2 = table[code2];

    // Emboss override (SW8)
    if (sw & (1u << 8))
    {
        s.f1 = FILTER_EMBOSS;
    }

    s.chain_on = (int)chain;

    return s;
}

//   Main image-processing loop (called from labmain.c: main)

void imageproc_main(void)
{
    uint32_t last_sw = 0xFFFFFFFFu; // force initial update
    int last_bt = 0;                // last button state

    print("[Image processing]\n");
    print("Controls:\n");
    print("  SW1:0  = Filter 1\n");
    print("  SW3:2  = Filter 2\n");
    print("  SW4    = Chain (0=F1 only, 1=F1->F2)\n");
    print("  SW8    = Emboss override for Filter 1\n");
    print("\n");
    print("Upload/download usage:\n");
    print("  dtekv-upload input.raw 0x01000000 // 320x240 RAW grayscale (76800 bytes)\n");
    print("  [set filters with switches]\n");
    print("  [press KEY1 once]\n");
    print("  dtekv-download output.raw 0x01030000 76800 OR observe output in VGA\n");
    print("\n");

    while (1) // inf loop
    {
        uint32_t sw = (uint32_t)get_sw(); // read switches
        int bt = get_bt() ? 1 : 0;        // read button (1=pressed)

        // Mirror switches on LEDs (without done-flag) as live status
        if (sw != last_sw) // only update if changed
        {
            set_leds((int)sw); // mirror switches to LEDs
            last_sw = sw;      // update last_sw
        }

        // Rising edge on button -> run filters once
        if (bt && !last_bt) // button pressed now, but not last time
        {
            Selection sel = decode_switches(sw); // decode switches

            // Load input image from uploaded buffer
            load_input_image_from_upload(&img_in);

            // --- Get kernels FIRST so we can use their size fields ---
            const Kernel *k1 = get_kernel(sel.f1, 0);
            const Kernel *k2 = 0;

            if (sel.chain_on)
            {
                k2 = get_kernel(sel.f2, 0);
            }

            print("\n---- New instance of processing ----\n");
            print("SW = 0x");
            print_hex32(sw);
            print("\n");

            // Filter 1 info
            print("Filter 1: ");
            print_dec((unsigned)sel.f1);
            print("  size=");
            print_dec((unsigned)(*k1).size);
            print("\n");

            // Filter 2 info
            if (sel.chain_on && (k2 != 0))
            {
                print("Filter 2: ");
                print_dec((unsigned)sel.f2);
                print("  size=");
                print_dec((unsigned)(*k2).size);
                print("\n");
            }
            else
            {
                print("Filter 2: [disabled]\n");
            }

            // --- Core image-processing work ---

            // First filter: img_in -> img_tmp
            convolve(&img_in, &img_tmp, k1);

            // Optional second filter: img_tmp -> img_out
            if (sel.chain_on && (k2 != 0))
            {
                convolve(&img_tmp, &img_out, k2);
            }
            else
            {
                img_copy(&img_tmp, &img_out);
            }

            // Compute checksum of final image and print it to verify correctness
            uint32_t sum = img_checksum(&img_out);
            print("Output checksum = 0x");
            print_hex32(sum);
            print("\n");

            // Save processed image to RAM for dtekv-download
            save_output_image_to_download(&img_out);

            // Draw processed image on VGA screen
            vga_clear(0);
            vga_draw_image_centered(&img_out);

            // Turn on LED9 as a flag that processing is done
            set_leds((int)(sw | (1u << 9)));

            // Simple delay so holding button doesn't retrigger immediately
            delay(150);
        }

        last_bt = bt; // update last button state
    }
}
