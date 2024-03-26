#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8 // a cube has 8 vertices
#define N_CUBE_FACES (6 * 2) // 6 faces of the cube and 2 triangles per face

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

// defines a mesh
typedef struct {
    vec3_t* vertices;   // dynamic array of vertices
    face_t* faces;      // dynamic array of faces
    vec3_t rotation;    // the rotation info / euler angles
    vec3_t scale;
    vec3_t translation;
} mesh_t;

// actual mesh that will be used
extern mesh_t mesh;

void load_cube_mesh_data(void);
void load_obj_file_data(char* filename);

#endif