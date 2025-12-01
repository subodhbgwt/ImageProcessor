#include "conv.h"

/* Clamp integer to 0..255 range */
static inline uint8_t clamp_int(int v)
{
    if (v < 0)
        return 0; // Clamp to minimum
    if (v > 255)
        return 255;    // Clamp to maximum
    return (uint8_t)v; // Within range
}

/* Initialize kernel structure, copies input image dimensions to destination image */
void convolve(const Image *src, Image *dst, const Kernel *k)
{
    int size = (*k).size; // kernel size: 3 or 5
    int R = size / 2;     // radius: 1 for 3x3, 2 for 5x5

    (*dst).w = (*src).w; // copy width
    (*dst).h = (*src).h; // copy height

    for (int y = 0; y < (int)(*src).h; ++y)
    { // for each row
        for (int x = 0; x < (int)(*src).w; ++x)
        {                // for each column
            int acc = 0; // accumulator
            int idx = 0; // kernel index

            for (int dy = -R; dy <= R; ++dy)
            {                    // for each kernel row
                int yy = y + dy; // source y coordinate
                if (yy < 0)
                    yy = 0; // clamp to top edge
                if (yy >= (int)(*src).h)
                    yy = (int)(*src).h - 1; // clamp to bottom edge

                for (int dx = -R; dx <= R; ++dx)
                {                    // for each kernel column
                    int xx = x + dx; // source x coordinate
                    if (xx < 0)
                        xx = 0; // clamp to left edge
                    if (xx >= (int)(*src).w)
                        xx = (int)(*src).w - 1;                             // clamp to right edge
                    acc += (*src).data[yy * (*src).w + xx] * (*k).w[idx++]; // accumulate, increment kernel index
                }
            }

            if ((*k).norm > 1)
            {                                                // normalize if needed, scales to kernels intended magnitude
                if (acc >= 0)                                // positive values
                    acc = (acc + (*k).norm / 2) / (*k).norm; // rounding division
                else                                         // negative values
                    acc = (acc - (*k).norm / 2) / (*k).norm; // rounding division
            }

            acc += (*k).bias;                               // add bias, offset after normalization.Adds brightness or darkness
            (*dst).data[y * (*dst).w + x] = clamp_int(acc); // store clamped result
        }
    }
}
