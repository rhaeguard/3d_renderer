#include <stdio.h>
#include "mesh.h"
#include "array.h"
#include <string.h>

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0, 0, 0},
    .scale = {1.0, 1.0, 1.0},
    .translation = {0, 0, 0}
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1 }, // 1
    { .x = -1, .y =  1, .z = -1 }, // 2
    { .x =  1, .y =  1, .z = -1 }, // 3
    { .x =  1, .y = -1, .z = -1 }, // 4
    { .x =  1, .y =  1, .z =  1 }, // 5
    { .x =  1, .y = -1, .z =  1 }, // 6
    { .x = -1, .y =  1, .z =  1 }, // 7
    { .x = -1, .y = -1, .z =  1 }, // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3, .color = 0xFFFF0000},
    { .a = 1, .b = 3, .c = 4, .color = 0xFFFF0000},
    // right
    { .a = 4, .b = 3, .c = 5, .color = 0xFF00FF00},
    { .a = 4, .b = 5, .c = 6, .color = 0xFF00FF00},
    // back
    { .a = 6, .b = 5, .c = 7, .color = 0xFF0000FF},
    { .a = 6, .b = 7, .c = 8, .color = 0xFF0000FF},
    // left
    { .a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00},
    { .a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00},
    // top
    { .a = 2, .b = 7, .c = 5, .color = 0xFFFF00FF},
    { .a = 2, .b = 5, .c = 3, .color = 0xFFFF00FF},
    // bottom
    { .a = 6, .b = 8, .c = 1, .color = 0xFF00FFFF},
    { .a = 6, .b = 1, .c = 4, .color = 0xFF00FFFF}
};

void load_cube_mesh_data(void) {
    for (int i=0; i < N_CUBE_VERTICES; i++) {
        array_push(mesh.vertices, cube_vertices[i]);
    }

    for (int i=0; i < N_CUBE_FACES; i++) {
        array_push(mesh.faces, cube_faces[i]);
    }
}

void load_obj_file_data(char* filename) {
    // read the contents of the .obj file
    // load the vertices and faces into the mesh object

    FILE* file = fopen(filename, "r");

    char buf[255];

    while (fgets(buf, 255, file)) {
        if (buf[0]=='v') {
            if (buf[1]==' ') {
                // vertices
                float a, b, c;
                sscanf(buf,"v %f %f %f\n", &a, &b, &c);
                vec3_t v = {
                    .x = a,
                    .y = b,
                    .z = c
                };
                array_push(mesh.vertices, v);
            } else if (buf[1] == 't') {
                // textures
            } else if (buf[1] == 'n') {
                // normals
            }
        } else if (buf[0] == 'f') {
            // faces
            int av, at, an;
            int bv, bt, bn;
            int cv, ct, cn;
            sscanf(buf,
                "f %d/%d/%d %d/%d/%d %d/%d/%d\n", 
                &av, &at, &an,
                &bv, &bt, &bn,
                &cv, &ct, &cn
            );

            face_t face = {
                .a = av,
                .b = bv,
                .c = cv
            };

            array_push(mesh.faces, face);
        }
    }

    fclose(file);
}