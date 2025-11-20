#ifndef KERNELS_H
#define KERNELS_H

#include <stdint.h>

typedef struct {
    int size;       /* 3 or 5 */
    int16_t w[25];  /* 3x3 uses first 9, 5x5 uses all 25 */
    int norm;       /* divisor for normalization (e.g. 256 for Gaussian) */
    int bias;       /* value to add after division (e.g. 128 for emboss) */
} Kernel;

typedef enum {
    FILTER_IDENTITY = 0,
    FILTER_SHARPEN,
    FILTER_GAUSS,
    FILTER_EDGE,
    FILTER_EMBOSS,
    FILTER_COUNT
} FilterType;

const Kernel *get_kernel(FilterType f, int size);

#endif
