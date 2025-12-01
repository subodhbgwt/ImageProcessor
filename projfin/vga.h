#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include "img.h"

/* Clear the VGA screen to a grayscale value */
void vga_clear(uint8_t gray);

/* Draw an Image centered in the 320x240 VGA area */
void vga_draw_image_centered(const Image *img);

#endif
