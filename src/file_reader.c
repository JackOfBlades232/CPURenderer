/* CPURenderer/src/file_reader.c */
#include "file_reader.h"
#include "geom.h"
#include "scene.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokenizer.h>

enum {
    base_mtl_bufsize = 8,
    base_obj_bufsize = 16,
    base_light_bufsize = 8,

    bufsize_mod = 2
};

typedef struct word_list *word_listp;

typedef struct tag_material_name_pair {
    char *name;
    material *mat;
} material_name_pair;

typedef struct tag_file_read_state {
    vec3d *vertices, *v_normals, *v_texcoords;
    int vert_cnt, v_norm_cnt, v_texc_cnt;
    int vert_cap, v_norm_cap, v_texc_cap;
    material *cur_mat;
    material_name_pair *material_mapping;
} file_read_state;

static char *get_dir_path(const char *filepath)
{
    const char *p;
    int len;
    char *dirpath;

    for (p = filepath; *p; p++)
        {}
    while (*p != '/' && p >= filepath)
        p--;

    len = p - filepath + 2;
    dirpath = (char *) malloc(len*sizeof(char));
    strncpy(dirpath, filepath, len-1);
    dirpath[len-1] = '\0';

    return dirpath;
}

static file_read_result *create_read_result()
{
    file_read_result *res = malloc(sizeof(file_read_result));
    res->mat_cap = base_mtl_bufsize;
    res->obj_cap = base_obj_bufsize;
    res->light_cap = base_light_bufsize;
    res->mat_buf = malloc(sizeof(material)*res->mat_cap);
    res->obj_buf = malloc(sizeof(scene_obj)*res->obj_cap);
    res->light_buf = malloc(sizeof(light_src)*res->light_cap);
    res->mat_cnt = res->obj_cnt = res->light_cnt = 0;
    return res;
}

static void free_read_result(file_read_result *res)
{
    free(res->mat_buf);
    free(res->obj_buf);
    free(res->light_buf);
    free(res);
}

static void add_object(file_read_result *res, scene_obj obj)
{
    if (res->obj_cnt >= res->obj_cap) {
        res->obj_cap *= bufsize_mod;
        res->obj_buf = realloc(res->obj_buf, sizeof(scene_obj)*res->obj_cap);
    }

    res->obj_buf[res->obj_cnt] = obj;
    res->obj_cnt++;
}

static void add_light(file_read_result *res, light_src light)
{
    if (res->light_cnt >= res->light_cap) {
        res->light_cap *= bufsize_mod;
        res->light_buf = realloc(
                res->light_buf, 
                sizeof(light_src)*res->light_cap
                );
    }

    res->light_buf[res->light_cnt] = light;
    res->light_cnt++;
}

static int try_read_double(word_listp w_list, double *out)
{
    int result;
    struct word *w;

    w = word_list_pop_first(w_list);
    if (!w)
        return 0;

    result = sscanf(word_content(w), "%lf", out);
    word_free(w);
    return result == 1;
}

static int parse_mtllib(word_listp w_list, file_read_result *res,
        const char *dirpath)
{
    return 0;
}

static int parse_usemtl(word_listp w_list, file_read_result *res)
{
    return 0;
}

static int parse_v(word_listp w_list, file_read_result *res)
{
    return 0;
}

static int parse_vt(word_listp w_list, file_read_result *res)
{
    return 0;
}

static int parse_vn(word_listp w_list, file_read_result *res)
{
    return 0;
}

static int parse_f(word_listp w_list, file_read_result *res)
{
    return 0;
}

static int parse_S(word_listp w_list, file_read_result *res)
{
    scene_obj obj;

    obj.type = sphere;
    if (!try_read_double(w_list, &obj.data.s.center.x) ||
            !try_read_double(w_list, &obj.data.s.center.y) ||
            !try_read_double(w_list, &obj.data.s.center.z) ||
            !try_read_double(w_list, &obj.data.s.rad)) {
        return 0;
    }
    /* set material */

    add_object(res, obj);
    return 1;
}

static int parse_P(word_listp w_list, file_read_result *res)
{
    light_src light;

    if (!try_read_double(w_list, &light.pos.x) ||
            !try_read_double(w_list, &light.pos.y) ||
            !try_read_double(w_list, &light.pos.z) ||
            !try_read_double(w_list, &light.illum.x) ||
            !try_read_double(w_list, &light.illum.y) ||
            !try_read_double(w_list, &light.illum.z)) {
        return 0;
    }

    add_light(res, light);
    return 1;
}

static int parse_obj_line(word_listp w_list, file_read_result *res,
        const char *dirpath)
{
    int status = 0;
    struct word *w;
    const char *tok;

    w = word_list_pop_first(w_list);
    if (!w) /* empty line */
        return 1;
    tok = word_content(w);

    if (*tok == '#') /* comment */
        status = 1;
    else if (strcmp(tok, "mtllib") == 0)
        status = parse_mtllib(w_list, res, dirpath);
    else if (strcmp(tok, "usemtl") == 0)
        status = parse_usemtl(w_list, res);
    else if (strcmp(tok, "v") == 0)
        status = parse_v(w_list, res);
    else if (strcmp(tok, "vt") == 0)
        status = parse_vt(w_list, res);
    else if (strcmp(tok, "vn") == 0)
        status = parse_vn(w_list, res);
    else if (strcmp(tok, "f") == 0)
        status = parse_f(w_list, res);
    else if (strcmp(tok, "S") == 0)
        status = parse_S(w_list, res);
    else if (strcmp(tok, "P") == 0)
        status = parse_P(w_list, res);

    word_free(w);

    return status;
}

file_read_result *read_scene_from_files(const char *path)
{
    const char *dirpath = get_dir_path(path);

    FILE *obj_f;
    word_listp w_list;
    int eol_char = '\n';

    file_read_result *res;

    obj_f = fopen(path, "r");
    if (!obj_f)
        return NULL;

    res = create_read_result();

    while (tokenize_input_line_to_word_list(obj_f, &w_list, &eol_char) == 0) {
        int success = parse_obj_line(w_list, res, dirpath);
        word_list_free(w_list);

        if (!success)
            goto read_error;

        if (eol_char == EOF)
            break;
    }

    fclose(obj_f);
    return res;

read_error:
    free_read_result(res);
    fclose(obj_f);
    return NULL;
}

scene *create_scene_for_read_res(file_read_result *read_res)
{
    scene *s = create_scene();
    s->objects = read_res->obj_buf;
    s->objects_cnt = read_res->obj_cnt;
    s->lights = read_res->light_buf;
    s->lights_cnt = read_res->light_cnt;
    return s;
}
