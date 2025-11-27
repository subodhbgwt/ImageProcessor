#include <stdint.h>
#include "img.h"
#include "vga.h"

/*
 * DE10-Lite course setup (as in the VGA demo):
 *
 * VGA Screen Buffer
 *   Base address: 0x08000000
 *   Layout:       320 x 240 pixels, 8-bit per pixel (grayscale / color index)
 *
 * We treat it as a 320x240 array of 8-bit grayscale values.
 */

#define VGA_BASE_ADDR  0x08000000u
#define VGA_WIDTH      320
#define VGA_HEIGHT     240
#define VGA_STRIDE     320     /* bytes per line */

static volatile uint8_t *const VGA_FB8 =
    (volatile uint8_t *)VGA_BASE_ADDR;

/* -------------------------------------------------------------------------- */
/* Clear the whole VGA screen to a constant grayscale value                   */
/* -------------------------------------------------------------------------- */

void vga_clear(uint8_t gray)
{
    for (int y = 0; y < VGA_HEIGHT; ++y) {
        int row_off = y * VGA_STRIDE;
        for (int x = 0; x < VGA_WIDTH; ++x) {
            VGA_FB8[row_off + x] = gray;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* Draw an Image centered on the VGA screen                                   */
/* -------------------------------------------------------------------------- */

void vga_draw_image_centered(const Image *img)
{
    int draw_w = img->w;
    int draw_h = img->h;

    /* Clamp in case someone passes a larger image */
    if (draw_w > VGA_WIDTH)  draw_w = VGA_WIDTH;
    if (draw_h > VGA_HEIGHT) draw_h = VGA_HEIGHT;

    /* Center inside the 320x240 logical screen */
    int off_x = (VGA_WIDTH  - draw_w) / 2;
    int off_y = (VGA_HEIGHT - draw_h) / 2;

    for (int y = 0; y < draw_h; ++y) {
        int src_row_off = y * img->w;
        int dst_row_off = (off_y + y) * VGA_STRIDE;

        for (int x = 0; x < draw_w; ++x) {
            uint8_t g = img->data[src_row_off + x];   /* grayscale 0..255 */
            int vx = off_x + x;

            VGA_FB8[dst_row_off + vx] = g;
        }
    }
}
