/* CPURenderer/bitmap.h */
#ifndef BITMAP_SENTRY
#define BITMAP_SENTRY

#include <stddef.h>
#include <stdint.h>

typedef struct tag_pixel {
    uint8_t r, g, b;
} pixel_t;

typedef struct tag_bitmap {
    pixel_t *pixels;
    size_t width, height;
} bitmap_t;

pixel_t create_pixel(uint8_t r, uint8_t g, uint8_t b);
pixel_t *pixel_at(bitmap_t *bitmap, int x, int y);
void set_pixel(bitmap_t *bitmap, int x, int y, pixel_t pixel);

#endif
