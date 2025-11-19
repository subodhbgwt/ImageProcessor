#ifndef CONV_H
#define CONV_H

#include "img.h"
#include "kernels.h"

void convolve(const Image *src, Image *dst, const Kernel *k);

#endif
