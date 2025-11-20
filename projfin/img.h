#ifndef IMG_H
#define IMG_H

#include <stdint.h>

/* Fixed maximum supported image size: 64x64 (one byte per pixel) */
#define MAX_W 64u
#define MAX_H 64u

typedef struct {
    uint16_t w;
    uint16_t h;
    uint8_t  data[MAX_W * MAX_H];   /* grayscale: 1 byte per pixel */
} Image;

#endif
