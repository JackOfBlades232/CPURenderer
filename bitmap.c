/* CPURenderer/bitmap.c */
#include "bitmap.h"

pixel_t *pixel_at(bitmap_t *bitmap, int x, int y)
{
    return bitmap->pixels + y * bitmap->width + x;
}

void set_pixel(bitmap_t *bitmap, int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    pixel_t *pix = pixel_at(bitmap, x, y);
    pix->r = r;
    pix->g = g;
    pix->b = b;
}
