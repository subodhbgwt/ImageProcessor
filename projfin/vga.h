#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include "img.h"

/* Clear the whole VGA screen to a constant grayscale value */
void vga_clear(uint8_t gray);

/* Draw an Image centered on the VGA screen */
void vga_draw_image_centered(const Image *img);

#endif
