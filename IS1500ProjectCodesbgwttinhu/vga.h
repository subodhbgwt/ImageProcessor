#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include "img.h"

void vga_clear(uint8_t gray);
void vga_draw_image_centered(const Image *img);

#endif
