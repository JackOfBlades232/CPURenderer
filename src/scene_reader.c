/* CPURenderer/src/scene_reader.c */
#include "scene_reader.h"
#include "reader_utils.h"

#include <stdio.h>
#include <string.h>

static int path_has_suffix(const char *path, const char *suf)
{
    int len = strlen(path);
    int suf_len = strlen(suf);
    int i;

    if (len <= suf_len)
        return 0;

    for (i = len-suf_len; i < len; i++)
        if (path[i] != suf[i-len+suf_len])
            return 0;

    return 1;
}

static FILE *open_obj(const char *path)
{
    if (!path_has_suffix(path, ".obj"))
        return NULL;

    return fopen_r(path);
}

scene *read_scene(const char *path)
{
    scene *s;
    FILE *obj_f, *mtl_f;

    obj_f = open_obj(path);
    if (!obj_f) {
        return NULL;
    }

    return s;
}
