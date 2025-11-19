#include "conv.h"

/* Clamp integer to 0..255 */
static inline uint8_t clamp_int(int v) {
    if (v < 0)   return 0;
    if (v > 255) return 255;
    return (uint8_t)v;
}

void convolve(const Image *src, Image *dst, const Kernel *k) {
    int size = k->size;
    int R = size / 2;   /* radius: 1 for 3x3, 2 for 5x5 */

    dst->w = src->w;
    dst->h = src->h;

    for (int y = 0; y < (int)src->h; ++y) {
        for (int x = 0; x < (int)src->w; ++x) {
            int acc = 0;
            int idx = 0;

            for (int dy = -R; dy <= R; ++dy) {
                int yy = y + dy;
                if (yy < 0) yy = 0;
                if (yy >= (int)src->h) yy = (int)src->h - 1;

                for (int dx = -R; dx <= R; ++dx) {
                    int xx = x + dx;
                    if (xx < 0) xx = 0;
                    if (xx >= (int)src->w) xx = (int)src->w - 1;

                    acc += src->data[yy * src->w + xx] * k->w[idx++];
                }
            }

            if (k->norm > 1) {
                /* rounded division */
                if (acc >= 0)
                    acc = (acc + k->norm / 2) / k->norm;
                else
                    acc = (acc - k->norm / 2) / k->norm;
            }

            acc += k->bias;
            dst->data[y * dst->w + x] = clamp_int(acc);
        }
    }
}
