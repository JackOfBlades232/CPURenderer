/* CPURenderer/src/render_options.h */
#ifndef RENDER_OPTIONS_SENTRY
#define RENDER_OPTIONS_SENTRY

typedef enum tag_render_mode {
    rmode_full, rmode_depth, rmode_normal
} render_mode;

typedef enum tag_bvh_split_mode {
    bvhs_middle, bvhs_sah
} bvh_split_mode;

typedef struct tag_bvh_options {
    int use_bvh;
    bvh_split_mode split_mode;
    int num_buckets;
    int max_objs_in_leaf;
    double part_cost, inter_cost;
    int max_objs_for_mid;
} bvh_options;

typedef struct tag_render_options {
    render_mode mode;
    int max_rec_depth;
    bvh_options bvh_opts;
} render_options;

#endif
