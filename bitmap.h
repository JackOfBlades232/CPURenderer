/* CPURenderer/bitmap.h */
#ifndef BITMAP_SENTRY
#define BITMAP_SENTRY

#include <stdint.h>
#include <stddef.h>

typedef struct tag_pixel {
    uint8_t r, g, b;
} pixel_t;

typedef struct tag_bitmap {
    pixel_t *pixels;
    size_t width, height;
} bitmap_t;

#endif
