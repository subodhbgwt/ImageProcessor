#include <stdint.h>
#include "img.h"
#include "vga.h"

/*
 * DE10-Lite course setup:
 *
 * VGA Screen Buffer (320x240x2 bytes per pixel)
 *   Base address: 0x08000000
 *   Size:         320 * 240 * 2 = 0x25800 bytes
 *
 * We treat it as a 320x240 array of 16-bit RGB565 values.
 */

#define VGA_BASE_ADDR  0x08000000u
#define VGA_WIDTH      320
#define VGA_HEIGHT     240

/* 16-bit pixels: RGB565 */
#define VGA_FB16 ((volatile uint16_t *)(uintptr_t)VGA_BASE_ADDR)

/* Convert 0..255 grayscale to 16-bit RGB565 */
static inline uint16_t gray_to_rgb565(uint8_t g) {
    uint16_t r = (uint16_t)(g >> 3);          // 5 bits
    uint16_t g6 = (uint16_t)(g >> 2);         // 6 bits
    uint16_t b = (uint16_t)(g >> 3);          // 5 bits
    return (uint16_t)((r << 11) | (g6 << 5) | b);
}

/* --------------------------------------------------------------------------
   Clear full screen
   -------------------------------------------------------------------------- */
void vga_clear(uint8_t gray) {
    uint16_t color = gray_to_rgb565(gray);
    const int N = VGA_WIDTH * VGA_HEIGHT;
    for (int i = 0; i < N; ++i) {
        VGA_FB16[i] = color;
    }
}

/* --------------------------------------------------------------------------
   Draw grayscale Image centered on screen
   -------------------------------------------------------------------------- */
void vga_draw_image_centered(const Image *img) {
    int draw_w = img->w;
    int draw_h = img->h;

    if (draw_w > VGA_WIDTH)  draw_w = VGA_WIDTH;
    if (draw_h > VGA_HEIGHT) draw_h = VGA_HEIGHT;

    int off_x = (VGA_WIDTH  - draw_w) / 2;
    int off_y = (VGA_HEIGHT - draw_h) / 2;

    for (int y = 0; y < draw_h; ++y) {
        for (int x = 0; x < draw_w; ++x) {
            uint8_t g = img->data[y * img->w + x];  // 0..255 grayscale
            uint16_t color = gray_to_rgb565(g);

            int vx = off_x + x;
            int vy = off_y + y;
            VGA_FB16[vy * VGA_WIDTH + vx] = color;
        }
    }
}
