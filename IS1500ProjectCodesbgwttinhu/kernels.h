#ifndef KERNELS_H
#define KERNELS_H

#include <stdint.h>

typedef struct
{
    int size;
    int16_t w[25];
    int norm;
    int bias;
} Kernel;

typedef enum
{
    FILTER_IDENTITY = 0,
    FILTER_SHARPEN,
    FILTER_GAUSS,
    FILTER_EDGE,
    FILTER_EMBOSS,
    FILTER_COUNT
} FilterType;

const Kernel *get_kernel(FilterType f, int size);

#endif
