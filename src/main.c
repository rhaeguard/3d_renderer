#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "array.h"
#include "matrix.h"

enum cull_method {
    CULL_NONE,
    CULL_BACKFACE
} cull_method;

enum render_method {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE
} render_method;

triangle_t* triangles_to_render = NULL;

vec3_t camera_position = {
    .x = 0, .y = 0, .z = 0
};

float fov_factor = 640;

bool is_running = false;
// milliseconds
int previous_frame_time = 0;

void setup(void) {
    // initialize the render mode and triangle culling method
    render_method = RENDER_WIRE;
    cull_method = CULL_BACKFACE;

    color_buffer = (uint32_t*) malloc(
        sizeof(uint32_t) * window_width * window_height
    );

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    load_cube_mesh_data();
    // load_obj_file_data("./assets/cube.obj");
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
            }

            if (event.key.keysym.sym == SDLK_1) {
                render_method = RENDER_WIRE_VERTEX;
            }

            if (event.key.keysym.sym == SDLK_2) {
                render_method = RENDER_WIRE;
            }

            if (event.key.keysym.sym == SDLK_3) {
                render_method = RENDER_FILL_TRIANGLE;
            }

            if (event.key.keysym.sym == SDLK_4) {
                render_method = RENDER_FILL_TRIANGLE_WIRE;
            }

            if (event.key.keysym.sym == SDLK_c) {
                cull_method = CULL_BACKFACE;
            }

            if (event.key.keysym.sym == SDLK_d) {
                cull_method = CULL_NONE;
            }

            break;
    }
}

// this function projects a 3D vector into a 2D vector
vec2_t project(vec3_t point) {
    // perspective projection
    vec2_t projected_point = {
        .x = (fov_factor * point.x) / point.z,
        .y = (fov_factor * point.y) / point.z
    };

    return projected_point;
}

// Reference: https://stackoverflow.com/a/27284318/9985287
// compare function
int triangle_compare_function (const void * a, const void * b) {
    triangle_t* t1 = (triangle_t*) a;
    triangle_t* t2 = (triangle_t*) b;
    return t2->avg_depth - t1->avg_depth;
}

void update(void) {
    // wait until the next update time
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks(); // milliseconds

    triangles_to_render = NULL;

    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    // translate the vertex away from the camera
    mesh.translation.z = 5;

    // create a scale matrix
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    // create translation matrix
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    // create rotation matrix
    mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

    int num_faces = array_length(mesh.faces);

    for (int i=0;i<num_faces;i++) {
        face_t mesh_face = mesh.faces[i];
        
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec4_t transformed_vertices[3];

        // transformation
        for (int j=0; j < 3; j++) {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
            // matrix for scaling the original vertex
            // multiply scale matrix by the vertex
            transformed_vertex = mat4_mul_vec4(scale_matrix, transformed_vertex);
            
            // rotate 
            transformed_vertex = mat4_mul_vec4(rotation_matrix_x, transformed_vertex);
            transformed_vertex = mat4_mul_vec4(rotation_matrix_y, transformed_vertex);
            transformed_vertex = mat4_mul_vec4(rotation_matrix_z, transformed_vertex);
            
            // translate
            transformed_vertex = mat4_mul_vec4(translation_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        if (cull_method == CULL_BACKFACE) {
            // backface culling
            // https://en.wikipedia.org/wiki/Back-face_culling#Implementation
            vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
            vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

            // culling: find the vectors for the sides of the triangle
            vec3_t vector_ab = vec3_sub(vector_b, vector_a);
            vec3_t vector_ac = vec3_sub(vector_c, vector_a);
            vec3_normalize(&vector_ab);
            vec3_normalize(&vector_ac);

            // culling: take the cross product of those two vectors to find the normal vector
            // cross product is not commutative!
            // we're using a left handed coordinate system
            // it's clockwise, thus the following order
            vec3_t normal = vec3_cross(vector_ab, vector_ac);
            // normalize the face normal vector
            vec3_normalize(&normal);

            // culling: find the vector between a point in the triangle and the camera origin
            vec3_t camera_ray = vec3_sub(camera_position, vector_a);

            // culling: find the dot product to find if the triangle is looking towards the camera
            // dot product is commutative
            float dot_normal_camera = vec3_dot(camera_ray, normal);

            // bypass the triangles that are not looking at the camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        vec2_t projected_points[3];

        // perform projection
        for (int j=0; j < 3; j++) {

            // project the current vertex
            projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));

            // scale and translate the projected point to the middle of the screen
            projected_points[j].x += (window_width / 2);
            projected_points[j].y += (window_height / 2);
        }

        // calculate the average depth for each face based on the vertices after transformation
        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z)/3.0; 

        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y },
                { projected_points[1].x, projected_points[1].y },
                { projected_points[2].x, projected_points[2].y }
            },
            .color = mesh_face.color,
            .avg_depth = avg_depth
        };

        // save for rendering
        array_push(triangles_to_render, projected_triangle);
    }

    // sort the triangles to render by their average depth
    qsort(
        triangles_to_render, 
        array_length(triangles_to_render), 
        sizeof(triangle_t), 
        triangle_compare_function
    );
}

void render(void) {
    draw_grid();

    int num_triangles = array_length(triangles_to_render);
    
    for (int i=0; i<num_triangles; i++) {
        triangle_t triangle = triangles_to_render[i];

        if (
            render_method == RENDER_FILL_TRIANGLE || 
            render_method == RENDER_FILL_TRIANGLE_WIRE
        ) {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.color
            );
        }

        if (
            render_method == RENDER_WIRE || 
            render_method == RENDER_WIRE_VERTEX || 
            render_method == RENDER_FILL_TRIANGLE_WIRE
        ) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                0xFFFFFFFF
            );
        }

        if (render_method == RENDER_WIRE_VERTEX) {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000);
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000);
        }
    }

    array_free(triangles_to_render);

    render_color_buffer();

    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

void free_resources(void) {
    // free the buffer in the memory
    free(color_buffer);
    array_free(mesh.vertices);
    array_free(mesh.faces);
}

int main(void) {

    // Create an SDL window
    is_running = initialize_window();

    setup();

    while(is_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    free_resources();

    return 0;
}