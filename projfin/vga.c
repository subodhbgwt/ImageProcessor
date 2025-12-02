#include <stdint.h>
#include "img.h"
#include "vga.h"

/*
VGA Screen Buffer
Treat as a 320x240 8-bit grayscale buffer laid out linearly
from base address 0x08000000.
*/

#define VGA_BASE_ADDR 0x08000000u
#define VGA_WIDTH 320
#define VGA_HEIGHT 240
#define VGA_STRIDE 320

static volatile uint8_t *const VGA_FB8 =
    (volatile uint8_t *)VGA_BASE_ADDR;

// Clear screen to given gray level (0..255)
void vga_clear(uint8_t gray)
{
    for (int y = 0; y < VGA_HEIGHT; ++y)
    {
        int row_off = y * VGA_STRIDE;
        for (int x = 0; x < VGA_WIDTH; ++x)
        {
            VGA_FB8[row_off + x] = gray;
        }
    }
}

// Draw image centered on screen

void vga_draw_image_centered(const Image *img)
{
    int draw_w = (*img).w;
    int draw_h = (*img).h;

    if (draw_w > VGA_WIDTH)
        draw_w = VGA_WIDTH;
    if (draw_h > VGA_HEIGHT)
        draw_h = VGA_HEIGHT;

    int off_x = (VGA_WIDTH - draw_w) / 2;
    int off_y = (VGA_HEIGHT - draw_h) / 2;

    for (int y = 0; y < draw_h; ++y)
    {
        int src_row_off = y * (*img).w;
        int dst_row_off = (off_y + y) * VGA_STRIDE;

        for (int x = 0; x < draw_w; ++x)
        {
            uint8_t g = (*img).data[src_row_off + x];
            VGA_FB8[dst_row_off + (off_x + x)] = g;
        }
    }
}
