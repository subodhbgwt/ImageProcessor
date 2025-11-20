#include "images.h"

/* Example 8x8 dummy image (just to show the structure) */
static const uint8_t img_64x64_gradient[64*64] = {
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 45, 49, 53, 57, 61,
    65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125,
    130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190,
    194, 198, 202, 206, 210, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255
    /* remaining 64*64 - 64 = 4032 elements are implicitly 0 */
};

const ImageDef builtin_images[] = {
    { 64, 64, img_64x64_gradient},
    /* later you can add more:
       { width, height, some_other_image_array },
    */
};

const int num_builtin_images = sizeof(builtin_images) / sizeof(builtin_images[0]);
