#ifndef IMAGES_H
#define IMAGES_H

#include <stdint.h>

typedef struct {
    uint16_t w;
    uint16_t h;
    const uint8_t *data;
} ImageDef;

/* List of built-in images */
extern const ImageDef builtin_images[];
extern const int num_builtin_images;

#endif
