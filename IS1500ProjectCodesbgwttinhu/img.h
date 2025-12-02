#ifndef IMG_H
#define IMG_H

#include <stdint.h>

#define MAX_W 320u
#define MAX_H 240u

typedef struct
{
    uint16_t w;
    uint16_t h;
    uint8_t data[MAX_W * MAX_H];
} Image;

#endif
