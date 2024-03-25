#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "triangle.h"
#include "array.h"

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

    // load_cube_mesh_data();
    load_obj_file_data("./assets/cube.obj");
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            SDL_KeyCode key_sym = event.key.keysym.sym;
            if (key_sym == SDLK_ESCAPE) {
                is_running = false;
            }

            if (key_sym == SDLK_1) {
                render_method = RENDER_WIRE_VERTEX;
            }

            if (key_sym == SDLK_2) {
                render_method = RENDER_WIRE;
            }

            if (key_sym == SDLK_3) {
                render_method = RENDER_FILL_TRIANGLE;
            }

            if (key_sym == SDLK_4) {
                render_method = RENDER_FILL_TRIANGLE_WIRE;
            }

            if (key_sym == SDLK_c) {
                cull_method = CULL_BACKFACE;
            }

            if (key_sym == SDLK_d) {
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
    // mesh.rotation.z += 0.01;

    int num_faces = array_length(mesh.faces);

    for (int i=0;i<num_faces;i++) {
        face_t mesh_face = mesh.faces[i];
        
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];

        vec3_t transformed_vertices[3];

        // transformation
        for (int j=0; j < 3; j++) {
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

            // translate the vertex away from the camera
            transformed_vertex.z += 5;

            transformed_vertices[j] = transformed_vertex;
        }

        if (cull_method == CULL_BACKFACE) {
            // backface culling
            // https://en.wikipedia.org/wiki/Back-face_culling#Implementation
            vec3_t vector_a = transformed_vertices[0];
            vec3_t vector_b = transformed_vertices[1];
            vec3_t vector_c = transformed_vertices[2];

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

        triangle_t projected_triangle;

        // perform projection
        for (int j=0; j < 3; j++) {

            // project the current vertex
            vec2_t projected_point = project(transformed_vertices[j]);

            // scale and translate the projected point to the middle of the screen
            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;
        }

        // save for rendering
        array_push(triangles_to_render, projected_triangle);
    }

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
                0xFFC0C0C0
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