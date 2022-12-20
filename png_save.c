/* CPURenderer/png_save.h */
/* TODO : write comments */
#include "png_save.h"
#include "bitmap.h"
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <png.h>

#define FILEMODE "wb"

enum { bit_depth = 8 };

static void set_row_data(bitmap_t *bitmap, png_structp png_ptr,
        png_bytep *row_data_p, png_bytepp *row_pointers_p)
{
    int i;

    *row_data_p = png_malloc(png_ptr, 
        3 * bitmap->width * bitmap->height * sizeof(png_byte));
    *row_pointers_p = png_malloc(png_ptr, bitmap->height * sizeof(png_bytep));

    for (i = 0; i < bitmap->height; i++) {
        int base, j;

        base = 3 * i * bitmap->width;
        (*row_pointers_p)[i] = (*row_data_p) + base;

        for (j = 0; j < bitmap->width; j++) {
            pixel_t *bm_pixel;

            bm_pixel = pixel_at(bitmap, j, i);

            (*row_data_p)[base + 3 * j] = bm_pixel->r;
            (*row_data_p)[base + 3 * j + 1] = bm_pixel->g;
            (*row_data_p)[base + 3 * j + 2] = bm_pixel->b;
        }
    }
}

int save_bitmap_to_png(bitmap_t *bitmap, const char *path)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytepp row_pointers;
    png_bytep row_data;
    int status = -1;

    fp = fopen(path, FILEMODE);

    if (!fp)
        goto return_status;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        goto close_file;
    
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        goto destroy_write_struct;

    if (setjmp(png_jmpbuf(png_ptr)))
        goto destroy_write_struct;

    png_init_io(png_ptr, fp);

    png_set_IHDR(
        png_ptr,
        info_ptr,
        bitmap->width,
        bitmap->height,
        bit_depth,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png_ptr, info_ptr);

    set_row_data(bitmap, png_ptr, &row_data, &row_pointers);

    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png_ptr, info_ptr);

    png_free(png_ptr, row_data);
    png_free(png_ptr, row_pointers);

    status = 0;

destroy_write_struct:
    png_destroy_write_struct(&png_ptr, &info_ptr);
close_file:
    fclose(fp);
return_status:
    return status;
}
