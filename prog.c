/* testing */
#include <stdint.h>
#include <stdlib.h>
#include "bitmap.h"
#include "png_save.h"

uint8_t random_val()
{
    return (uint8_t)(256.0*rand()/(RAND_MAX+1.0));
}

pixel_t random_pixel()
{
    pixel_t pixel;

    pixel.r = random_val();
    pixel.g = random_val();
    pixel.b = random_val();

    return pixel;
}

int main()
{
    bitmap_t bm;
    int i, j;

    bm.width = 1920;
    bm.height = 1080;
    bm.pixels = calloc(bm.width * bm.height, sizeof(pixel_t));

    for (i = 0; i < bm.height; i++)
        for (j = 0; j < bm.width; j++)
            *pixel_at(&bm, j, i) = random_pixel();
  
    return save_bitmap_to_png(&bm, "./test.png");
}
