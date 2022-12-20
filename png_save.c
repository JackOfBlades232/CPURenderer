/* CPURenderer/png_save.h */
/* TODO : write comments */
#include "png_save.h"
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <png.h>

#define FILEMODE "wb"

enum { depth = 8 };

int save_bitmap_to_png(bitmap_t *bitmap, const char *path)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    int status = 0;

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

    png_set_IHDR(
        png_ptr,
        info_ptr,
        bitmap->width,
        bitmap->height,
        depth,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    /* TODO : create rows from bitmap */

    png_init_io(png_ptr, fp);

    status = 1;

destroy_write_struct:
    png_destroy_write_struct(&png_ptr, &info_ptr);
close_file:
    fclose(fp);
return_status:
    return status;
}

/* testing */
int main()
{
    bitmap_t bm;

    bm.width = 1920;
    bm.height = 1080;
    
    return save_bitmap_to_png(&bm, "./test.png");
}
