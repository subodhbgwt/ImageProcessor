#include <stdint.h>

typedef struct Image Image;

void vga_clear(uint8_t gray);
void vga_draw_image_centered(const Image *img);
