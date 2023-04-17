/* CPURenderer/src/render_options.h */
#ifndef RENDER_OPTIONS_SENTRY
#define RENDER_OPTIONS_SENTRY

typedef enum tag_render_mode {
    rmode_full, rmode_depth, rmode_normal
} render_mode;

typedef struct tag_render_options {
    render_mode mode;
    int max_rec_depth;
} render_options;

#endif
