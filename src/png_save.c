/* CPURenderer/png_save.h */
#include "png_save.h"
#include "image.h"
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <png.h>

#define FILEMODE "wb"

enum { bit_depth = 8 };

static void set_row_data(image *img, png_structp png_ptr,
        png_bytep *row_data_p, png_bytepp *row_pointers_p)
{
    int i;

    *row_data_p = png_malloc(png_ptr, 
        3 * img->width * img->height * sizeof(png_byte));
    *row_pointers_p = png_malloc(png_ptr, img->height * sizeof(png_bytep));

    for (i = 0; i < img->height; i++) {
        int base, j;

        base = 3 * i * img->width;
        (*row_pointers_p)[i] = (*row_data_p) + base;

        for (j = 0; j < img->width; j++) {
            vec3d *img_pixel;

            img_pixel = img_pixel_at(img, j, i);

            (*row_data_p)[base + 3 * j] = (png_byte)(255.0*img_pixel->x);
            (*row_data_p)[base + 3 * j + 1] = (png_byte)(255.0*img_pixel->y);
            (*row_data_p)[base + 3 * j + 2] = (png_byte)(255.0*img_pixel->z);
        }
    }
}

int save_img_to_png(image *img, const char *path)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytepp row_pointers;
    png_bytep row_data;
    int status = -1;

    fp = fopen(path, FILEMODE);

    if (!fp) {
        perror(path);
        goto return_status;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "Failed to create png write struct\n");
        goto close_file;
    }
    
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "Failed to create png info struct\n");
        goto destroy_write_struct;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Failed to write png\n");
        goto destroy_write_struct;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(
        png_ptr,
        info_ptr,
        img->width,
        img->height,
        bit_depth,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png_ptr, info_ptr);

    set_row_data(img, png_ptr, &row_data, &row_pointers);

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
