#ifndef _LAP_H
#define _LAP_H

#include <stdint.h>

#define SCALAR(x)       (int32_t)((x) * 65536)
#define VEC2(x,y)       (vec2_t){(x) * 65536, (y) * 65536}
#define MAT22(a,b,c,d)  (mat22_t){(a) * 65536, (b) * 65536, (c) * 65536, (d) * 65536}

typedef struct {
    int32_t x;
    int32_t y;
} vec2_t;

typedef struct {
    int32_t a;
    int32_t b;
    int32_t c;
    int32_t d;
} mat22_t;

static inline vec2_t add_vec2(const vec2_t a, const vec2_t b) { 
    return (vec2_t){a.x + b.x, a.y + b.y};
}

static inline vec2_t sub_vec2(const vec2_t a, const vec2_t b) { 
    return (vec2_t){a.x - b.x, a.y - b.y};
} 

// TODO: move this to asm?
static inline vec2_t scalar_mul_vec2(const vec2_t a, const int32_t scalar) {
    return (vec2_t){a.x * scalar, a.y * scalar};
}

static inline int32_t comp_to_int(int32_t a) {
    return (a / 65536);
}

static inline int is_comp_neg(int32_t a, int fp) {
    return (a >> fp) < 0; 
}

int32_t inv_scalar(const int32_t x);
void mul_vec2(int32_t* a, const int32_t b);
void mul_mat2x2_1xn(mat22_t *d, const mat22_t *a, const mat22_t *b, uint32_t n);
void mul_mat2x2_vec2_1xn(vec2_t *d, const mat22_t *a, const vec2_t *b, uint32_t n);
void inv_mat2x2(mat22_t *d, const mat22_t *s);

#endif