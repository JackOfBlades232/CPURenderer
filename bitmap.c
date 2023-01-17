/* CPURenderer/bitmap.c */
#include "bitmap.h"

pixel_t create_pixel(uint8_t r, uint8_t g, uint8_t b)
{
    pixel_t pixel;
    pixel.r = r;
    pixel.g = g;
    pixel.b = b;
    return pixel;
}

pixel_t *pixel_at(bitmap_t *bitmap, int x, int y)
{
    return bitmap->pixels + y * bitmap->width + x;
}

void set_pixel(bitmap_t *bitmap, int x, int y, pixel_t pixel)
{
    *pixel_at(bitmap, x, y) = pixel;
}
