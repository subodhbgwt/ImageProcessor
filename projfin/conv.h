typedef struct Image Image;
typedef struct Kernel Kernel;

void convolve(const Image *src, Image *dst, const Kernel *k);
