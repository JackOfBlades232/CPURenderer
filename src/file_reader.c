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

    base_v_bufsize = 32,

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

typedef struct tag_vertex_info {
    vec3d *v, *vt, *vn;
} vertex_info;

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

static void init_read_state(file_read_state *state)
{
    state->vert_cap = state->v_norm_cap = state->v_texc_cap = base_v_bufsize;
    state->vert_cnt = state->v_norm_cnt = state->v_texc_cnt = 0;
    state->vertices = malloc(sizeof(vec3d) * state->vert_cap);
    state->v_normals = malloc(sizeof(vec3d) * state->v_norm_cap);
    state->v_texcoords = malloc(sizeof(vec3d) * state->v_texc_cap);

    state->cur_mat = NULL;
    state->material_mapping = 
        malloc(sizeof(material_name_pair) * base_mtl_bufsize);
}

static void deinit_read_state(file_read_state *state)
{
    free(state->vertices);
    free(state->v_normals);
    free(state->v_texcoords);
    free(state->material_mapping);
}

static void *resize_buf_if_necessary(void *buf, int *cap,
        int len, int elem_size)  
{
    if (len >= *cap) {
        *cap *= bufsize_mod;
        return realloc(buf, (*cap) * elem_size);
    } else
        return buf;
}

static void add_object(file_read_result *res, scene_obj obj)
{
    res->obj_buf = resize_buf_if_necessary(res->obj_buf, 
            &res->obj_cap, res->obj_cnt, sizeof(scene_obj));

    res->obj_buf[res->obj_cnt] = obj;
    res->obj_cnt++;
}

static void add_light(file_read_result *res, light_src light)
{
    res->light_buf = resize_buf_if_necessary(res->light_buf, 
            &res->light_cap, res->light_cnt, sizeof(light_src));

    res->light_buf[res->light_cnt] = light;
    res->light_cnt++;
}

static vec3d *add_vec_item(vec3d *buf, int *cap, int *len, vec3d item)
{
    buf = resize_buf_if_necessary(buf, cap, *len, sizeof(vec3d));
    buf[*len] = item;
    (*len)++;

    return buf;
}

static void add_vertex(file_read_state *state, vec3d v)
{
    state->vertices = add_vec_item(state->vertices,
            &state->vert_cap, &state->vert_cnt, v);
}

static void add_v_normal(file_read_state *state, vec3d vn)
{
    state->v_normals = add_vec_item(state->v_normals,
            &state->v_norm_cap, &state->v_norm_cnt, vn);
}

/*
static void add_v_texcoord(file_read_state *state, vec3d vt)
{
    state->v_texcoords = add_vec_item(state->v_texcoords,
            &state->v_texc_cap, &state->v_texc_cnt, vt);
}
*/

static void add_material(file_read_state *state, file_read_result *res,
        material mat, const char *mat_name)
{
    int old_cap = res->mat_cap;
    material_name_pair *pairp;

    res->mat_buf = resize_buf_if_necessary(res->mat_buf,
            &res->mat_cap, res->mat_cnt, sizeof(material));
    if (res->mat_cap != old_cap) {
        state->material_mapping = realloc(state->material_mapping,
                sizeof(material_name_pair) * res->mat_cap);

        // @HACK: restore pointers after reallocating the material buffer
        for (int i = 0; i < res->mat_cnt; i++)
            state->material_mapping[i].mat = res->mat_buf + i;
    }

    res->mat_buf[res->mat_cnt] = mat;

    pairp = state->material_mapping + res->mat_cnt;
    pairp->mat = res->mat_buf + res->mat_cnt;

    size_t len = strlen(mat_name);
    pairp->name = malloc(sizeof(char) * (len+1));
    strncpy(pairp->name, mat_name, len);
    pairp->name[len] = '\0';

    res->mat_cnt++;
    state->cur_mat = pairp->mat;
}

#include "debug.h"

static int set_current_material(file_read_state *state, file_read_result *res,
        const char *mat_name)
{
    material_name_pair *pairp;
    for (pairp = state->material_mapping; 
            pairp - state->material_mapping < res->mat_cnt;
            pairp++) {
        if (strcmp(pairp->name, mat_name) == 0) {
            state->cur_mat = pairp->mat;
            return 1;
        }
    }

    return 0;
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

static int try_read_vec3d(word_listp w_list, vec3d *out)
{
    if (!try_read_double(w_list, &out->x) ||
            !try_read_double(w_list, &out->y) ||
            !try_read_double(w_list, &out->z)) {
        return 0;
    }

    return 1;
}

static int parse_newmtl(word_listp w_list, file_read_result *res,
        file_read_state *state)
{
    struct word *w;

    w = word_list_pop_first(w_list);
    if (!w)
        return 0;

    add_material(state, res, material_zero(), word_content(w));
    word_free(w);

    return 1;
}

static int parse_mtl_line(word_listp w_list, file_read_result *res,
        file_read_state *state)
{
    int status = 1;
    struct word *w;
    const char *tok;

    w = word_list_pop_first(w_list);
    if (!w) /* empty line */
        return 1;
    tok = word_content(w);

    if (*tok == '#') /* comment */
        status = 1;
    else if (strcmp(tok, "newmtl") == 0)
        status = parse_newmtl(w_list, res, state);
    else if (strcmp(tok, "Ka") == 0)
        status = try_read_vec3d(w_list, &state->cur_mat->ka);
    else if (strcmp(tok, "Ke") == 0)
        status = try_read_vec3d(w_list, &state->cur_mat->ke);
    else if (strcmp(tok, "Kd") == 0)
        status = try_read_vec3d(w_list, &state->cur_mat->kd);
    else if (strcmp(tok, "Ks") == 0)
        status = try_read_vec3d(w_list, &state->cur_mat->ks);
    else if (strcmp(tok, "Ns") == 0)
        status = try_read_double(w_list, &state->cur_mat->ns);
    else if (strcmp(tok, "Ni") == 0)
        status = try_read_double(w_list, &state->cur_mat->ni);
    else if (strcmp(tok, "al") == 0)
        status = try_read_vec3d(w_list, &state->cur_mat->al);
    else if (strcmp(tok, "Tr") == 0)
        status = try_read_double(w_list, &state->cur_mat->tr);
    else if (strcmp(tok, "d") == 0) { /* density is 1 - transparency */
        double d;
        status = try_read_double(w_list, &d);
        if (status)
            state->cur_mat->tr = 1. - d;
    }

    word_free(w);

    return status;
}

static int read_mtl_file(const char *path, file_read_result *res, 
        file_read_state *state)
{
    int success = 1;
    FILE *mtl_f;
    word_listp w_list;
    int eol_char = '\n';

    mtl_f = fopen(path, "r");
    if (!mtl_f)
        return 0;

    while (tokenize_input_line_to_word_list(mtl_f, &w_list, &eol_char) == 0) {
        success = parse_mtl_line(w_list, res, state);
        word_list_free(w_list);

        if (!success || eol_char == EOF)
            break;
    }

    state->cur_mat = NULL;
    fclose(mtl_f);

    return success;
}

static char *concat_strings(const char *s1, const char *s2)
{
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    char *s = malloc(sizeof(char) * (len1+len2+1));
    strncpy(s, s1, len1);
    strncpy(s+len1, s2, len2);
    s[len1+len2] = '\0';
    return s;
}

static int parse_mtllib(word_listp w_list, file_read_result *res, 
        file_read_state *state, const char *dirpath)
{
    struct word *w;
    char *full_mtl_path;

    w = word_list_pop_first(w_list);
    if (!w)
        return 0;

    full_mtl_path = concat_strings(dirpath, word_content(w));
    return read_mtl_file(full_mtl_path, res, state);
}

static int parse_usemtl(word_listp w_list, file_read_result *res, 
        file_read_state *state)
{
    int result;
    struct word *w;

    w = word_list_pop_first(w_list);
    if (!w)
        return 0;

    result = set_current_material(state, res, word_content(w));

    word_free(w);
    return result;
}

static int parse_v(word_listp w_list, file_read_result *res, 
        file_read_state *state)
{
    vec3d v;

    if (!try_read_vec3d(w_list, &v))
        return 0;

    add_vertex(state, v);
    return 1;
}

static int parse_vn(word_listp w_list, file_read_result *res, 
        file_read_state *state)
{
    vec3d vn;

    if (!try_read_vec3d(w_list, &vn))
        return 0;

    add_v_normal(state, vn);
    return 1;
}

/*
static int parse_vt(word_listp w_list, file_read_result *res, 
        file_read_state *state)
{
    vec3d vt;

    if (!try_read_vec3d(w_list, &vt))
        return 0;

    add_v_texcoord(state, vt);
    return 1;
}
*/

static vec3d *get_vec3d_at_index(vec3d *buf, int idx, int buf_len)
{
    if (idx < 0)
        idx += buf_len;
    else
        idx--;

    if (idx < 0 || idx >= buf_len)
        return NULL;

    return buf+idx;
}

static vec3d *parse_vec3d_index(const char *idx_str, vec3d *buf, int buf_len)
{
    int scan_res;
    int idx;
    scan_res = sscanf(idx_str, "%d", &idx);
    if (scan_res == 1)
        return get_vec3d_at_index(buf, idx, buf_len);
    else
        return NULL;
}

static int parse_face_item(word_listp w_list, file_read_state *state,
        vertex_info *out)
{
    int slash_cnt;
    struct word *w;
    const char *vp, *vtp = NULL, *vnp = NULL;
    const char *cp;

    w = word_list_pop_first(w_list);
    if (!w)
        return 0;

    vp = word_content(w);
    slash_cnt = 0;
    for (cp = vp; *cp; cp++) {
        if (*cp == '/') {
            slash_cnt++;
            if (!vtp)
                vtp = cp+1;
            else if (!vnp) { 
                vnp = cp+1;
                break;
            }
        }
    }

    out->vt = out->vn = NULL;

    out->v = parse_vec3d_index(vp, state->vertices, state->vert_cnt);
    /*
    if (slash_cnt >= 1)
        out->vt = parse_vec3d_index(vtp, state->v_texcoords, state->v_texc_cnt);
    */
    if (slash_cnt >= 2)
        out->vn = parse_vec3d_index(vnp, state->v_normals, state->v_norm_cnt);

    word_free(w);

    return out->v != NULL;
}

static scene_obj triangle_from_vertex_infos(vertex_info *v1,
        vertex_info *v2, vertex_info *v3, file_read_state *state)

{
    scene_obj tr;
    tr.type = triangle;
    tr.mat = state->cur_mat;
    tr.data.tr = trianlge_literal(
            v1->v->x, v1->v->y, v1->v->z,
            v2->v->x, v2->v->y, v2->v->z,
            v3->v->x, v3->v->y, v3->v->z
            );

    if (v1->vn && v2->vn && v3->vn) {
        tr.data.tr.has_vn = 1;
        tr.data.tr.vn1 = vec3d_normalized(*(v1->vn));
        tr.data.tr.vn2 = vec3d_normalized(*(v2->vn));
        tr.data.tr.vn3 = vec3d_normalized(*(v3->vn));
    } 
    
    return tr;
}

static int parse_f(word_listp w_list, file_read_result *res, 
        file_read_state *state)
{
    vertex_info vi1, vi2, vi3, tmp;

    if (!parse_face_item(w_list, state, &vi1) || 
            !parse_face_item(w_list, state, &vi2) || 
            !parse_face_item(w_list, state, &vi3)) {
        return 0;
    }

    add_object(res, triangle_from_vertex_infos(&vi1, &vi2, &vi3, state));

    while (parse_face_item(w_list, state, &tmp)) {
        vi2 = vi3;
        vi3 = tmp;
        add_object(res, triangle_from_vertex_infos(&vi1, &vi2, &vi3, state));
    }

    return 1;
}

static int parse_S(word_listp w_list, file_read_result *res,
        file_read_state *state)
{
    scene_obj obj;

    obj.type = sphere;
    if (!try_read_vec3d(w_list, &obj.data.s.center) ||
            !try_read_double(w_list, &obj.data.s.rad)) {
        return 0;
    }
    obj.mat = state->cur_mat;

    add_object(res, obj);
    return 1;
}

static int parse_P(word_listp w_list, file_read_result *res)
{
    light_src light;

    if (!try_read_vec3d(w_list, &light.pos) || 
            !try_read_vec3d(w_list, &light.illum)) {
        return 0;
    }

    add_light(res, light);
    return 1;
}

static int parse_obj_line(word_listp w_list, file_read_result *res,
        file_read_state *state, const char *dirpath)
{
    int status = 1;
    struct word *w;
    const char *tok;

    w = word_list_pop_first(w_list);
    if (!w) /* empty line */
        return 1;
    tok = word_content(w);

    if (*tok == '#') /* comment */
        status = 1;
    else if (strcmp(tok, "mtllib") == 0)
        status = parse_mtllib(w_list, res, state, dirpath);
    else if (strcmp(tok, "usemtl") == 0)
        status = parse_usemtl(w_list, res, state);
    else if (strcmp(tok, "v") == 0)
        status = parse_v(w_list, res, state);
    else if (strcmp(tok, "vn") == 0)
        status = parse_vn(w_list, res, state);
    else if (strcmp(tok, "vt") == 0) // @FEATURE don't need texcoord yet
        status = 1; // parse_vt(w_list, res, state);
    else if (strcmp(tok, "f") == 0)
        status = parse_f(w_list, res, state);
    else if (strcmp(tok, "S") == 0)
        status = parse_S(w_list, res, state);
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
    file_read_state state;

    obj_f = fopen(path, "r");
    if (!obj_f)
        return NULL;

    res = create_read_result();
    init_read_state(&state);

    while (tokenize_input_line_to_word_list(obj_f, &w_list, &eol_char) == 0) {
        int success = parse_obj_line(w_list, res, &state, dirpath);
        word_list_free(w_list);

        if (!success)
            goto read_error;

        if (eol_char == EOF)
            break;
    }

    deinit_read_state(&state);
    fclose(obj_f);
    return res;

read_error:
    deinit_read_state(&state);
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
