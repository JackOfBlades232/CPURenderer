/* CPURenderer/src/file_reader.h */
#ifndef FILE_READER_SENTRY
#define FILE_READER_SENTRY

#include "scene.h"
#include "render_options.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tag_file_read_result {
    material *mat_buf;
    scene_obj *obj_buf;
    light_src *light_buf;
    int mat_cnt, obj_cnt, light_cnt;
    int mat_cap, obj_cap, light_cap;
} file_read_result;

file_read_result *read_scene_from_files(const char *path);
scene *create_scene_for_read_res(file_read_result *read_res,
                                 render_options ropts);

#endif
