#include "kernels.h"

/* -------- Subodh: Identity, Sharpen, Gaussian (5x5) -------- */

static const Kernel K_IDENTITY_3 = {
    3,
    {
         0, 0, 0,
         0, 1, 0,
         0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    1,
    0
};

static const Kernel K_SHARPEN_3 = {
    3,
    {
         0, -1,  0,
        -1,  5, -1,
         0, -1,  0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    1,
    0
};

/* Gaussian blur 5x5 (sum = 256) */
static const Kernel K_GAUSS_5 = {
    5,
    {
         1,  4,  6,  4,  1,
         4, 16, 24, 16,  4,
         6, 24, 36, 24,  6,
         4, 16, 24, 16,  4,
         1,  4,  6,  4,  1
    },
    256,
    0
};

/* -------- Ting: Edge detection, Emboss -------- */

static const Kernel K_EDGE_3 = {
    3,
    {
         0, -1,  0,
        -1,  4, -1,
         0, -1,  0,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    1,
    0
};

static const Kernel K_EMBOSS_3 = {
    3,
    {
        -2, -1,  0,
        -1,  1,  1,
         0,  1,  2,
         0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    1,
    128   /* bias so mid-grey stays around 128 */
};

/* -------- Kernel selector -------- */

const Kernel *get_kernel(FilterType f, int size)
{
    (void)size; /* for now we always pick the “natural” size for each filter */

    switch (f) {
        case FILTER_IDENTITY:
            return &K_IDENTITY_3;
        case FILTER_SHARPEN:
            return &K_SHARPEN_3;
        case FILTER_GAUSS:
            return &K_GAUSS_5;
        case FILTER_EDGE:
            return &K_EDGE_3;
        case FILTER_EMBOSS:
            return &K_EMBOSS_3;
        default:
            return &K_IDENTITY_3;
    }
}
