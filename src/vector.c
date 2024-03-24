#include <math.h>
#include "vector.h"

// VECTOR 2D
// vec2d functions
float vec2_length(vec2_t v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b) {
    vec2_t v = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };

    return v;
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
    vec2_t v = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };

    return v;
}

vec2_t vec2_mul(vec2_t v, float factor) {
    vec2_t r = {
        .x = v.x * factor,
        .y = v.y * factor
    };

    return r;
}

vec2_t vec2_div(vec2_t v, float factor) {
    vec2_t r = {
        .x = v.x / factor,
        .y = v.y / factor
    };

    return r;
}

float vec2_dot(vec2_t a, vec2_t b) {
    float r = a.x * b.x + a.y * b.y;
    return r;
}

// VECTOR 3D
// vec3d functions
float vec3_length(vec3_t v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t v = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };

    return v;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
    vec3_t v = {
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };

    return v;
}

vec3_t vec3_mul(vec3_t v, float factor) {
    vec3_t r = {
        .x = v.x * factor,
        .y = v.y * factor,
        .z = v.z * factor
    };

    return r;
}

vec3_t vec3_div(vec3_t v, float factor) {
    vec3_t r = {
        .x = v.x / factor,
        .y = v.y / factor,
        .z = v.z / factor
    };

    return r;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t r = {
        .x = a.y*b.z - a.z*b.y,
        .y = a.z*b.x - a.x*b.z,
        .z = a.x*b.y - a.y*b.x
    };

    return r;
}

float vec3_dot(vec3_t a, vec3_t b) {
    float r = a.x * b.x + a.y * b.y + a.z * b.z;
    return r;
}

vec3_t vec3_rotate_x(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };

    return rotated_vector;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };

    return rotated_vector;
}

vec3_t vec3_rotate_z(vec3_t v, float angle) {
    vec3_t rotated_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };

    return rotated_vector;
}