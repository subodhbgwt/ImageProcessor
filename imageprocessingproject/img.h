#ifndef IMG_H
#define IMG_H

#include <stdint.h>

/* Maximum image size (you can change later if needed) */
#define MAX_W 160
#define MAX_H 120

typedef struct {
    uint16_t w;
    uint16_t h;
    uint8_t  data[MAX_W * MAX_H];
} Image;

#endif
