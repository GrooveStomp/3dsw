/******************************************************************************
  File: math.c
  Created: 2019-08-07
  Updated: 2019-08-16
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/
#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <stdio.h> // printf
#include <math.h> // sqrtf

#include "math.h"

void Vec3Debug(struct vec3 v) {
        printf("(struct vec3){ .x1 = %.1f, .y1 = %.1f, .z1 = %.1f }\n",
               v.x, v.y, v.z);

        printf("(struct vec3){ .p = { %.1f, %.1f, %.1f } }\n",
               v.p[0], v.p[1], v.p[2]);
}

void TriangleDebug(struct triangle t) {
        printf("(struct triangle){\n  .x1 = %.1f, .y1 = %.1f, .z1 = %.1f,\n  .x2 = %.1f, .y2 = %.1f, .z2 = %.1f,\n  .x3 = %.1f, .y3 = %.1f, .z3 = %.1f\n}\n",
               t.x1, t.y1, t.z1,
               t.x2, t.y2, t.z2,
               t.x3, t.y3, t.z3);

        printf("(struct triangle){\n  .v0 = { %.1f, %.1f, %.1f }\n  .v1 = { %.1f, %.1f, %.1f }\n  .v2 = { %.1f, %.1f, %.1f }\n}\n",
               t.v[0].x, t.v[0].y, t.v[0].z,
               t.v[1].x, t.v[1].y, t.v[1].z,
               t.v[2].x, t.v[2].y, t.v[2].z);

        printf("(struct triangle){\n  .p = { %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f }\n}\n",
               t.p[0], t.p[1], t.p[2], t.p[3], t.p[4], t.p[5], t.p[6], t.p[7], t.p[8]);
}

float Vec3DotProduct(struct vec3 left, struct vec3 right) {
        return left.x * right.x + left.y * right.y + left.z * right.z;
}

struct vec3 Vec3CrossProduct(struct vec3 left, struct vec3 right) {
        struct vec3 cross = { 0 };

        cross.x = left.y * right.z - left.z * right.y;
        cross.y = left.z * right.x - left.x * right.z;
        cross.z = left.x * right.y - left.y * right.x;

        return cross;
}

void Vec3Normalize(struct vec3 *vec3) {
        float l = sqrtf(vec3->x * vec3->x + vec3->y * vec3->y + vec3->z * vec3->z);
        vec3->x /= l;
        vec3->y /= l;
        vec3->z /= l;
}

struct vec3 Vec3Subtract(struct vec3 minuend, struct vec3 subtrahend) {
        struct vec3 vec3 = { 0 };

        vec3.x = minuend.x - subtrahend.x;
        vec3.y = minuend.y - subtrahend.y;
        vec3.z = minuend.z - subtrahend.z;

        return vec3;
}

struct mesh *MeshInit(int numTris) {
        struct mesh *mesh = (struct mesh *)malloc(sizeof(struct mesh));
        memset(mesh, 0, sizeof(struct mesh));

        mesh->tris = (struct triangle *)malloc(sizeof(struct triangle) * numTris);
        memset(mesh->tris, 0, sizeof(struct triangle) * numTris);

        mesh->count = numTris;

        return mesh;
}

struct mesh *MeshInitFromObj(char *objFile) {
        FILE *file = fopen(objFile, "r");
        if (NULL == file) {
                fprintf(stderr, "Couldn't open %s", objFile);
                return NULL;
        }

        // First scan through the file to see how much data we need to read.
        int numFaces = 0;
        int numVertices = 0;
        char line[256];
        while (NULL != fgets(line, sizeof(line), file)) {
                switch (line[0]) {
                        case 'v':
                                numVertices++;
                                break;
                        case 'f':
                                numFaces++;
                                break;
                }
        }
        if (ferror(file)) {
                fprintf(stderr, "I/O error while reading %s", objFile);
                return NULL;
        }
        rewind(file);

        // Now size our data containers appropriately.
        struct mesh *mesh = (struct mesh *)malloc(sizeof(struct mesh));
        memset(mesh, 0, sizeof(struct mesh));

        mesh->tris = (struct triangle *)malloc(sizeof(struct triangle) * numFaces);
        memset(mesh->tris, 0, sizeof(struct triangle) * numFaces);

        mesh->count = numFaces;

        struct vec3 *vertex = (struct vec3 *)malloc(sizeof(struct vec3) * numVertices);
        memset(vertex, 0, sizeof(struct vec3) * numVertices);

        int vertIdx = 0;
        int faceIdx = 0;
        // Finally, read through the file, filling out our data containers.
        while (NULL != fgets(line, sizeof(line), file)) {
                float x, y, z;
                int i1, i2, i3;

                switch (line[0]) {
                        case 'v': {
                                sscanf(line, "v %f %f %f", &x, &y, &z);
                                vertex[vertIdx].x = x;
                                vertex[vertIdx].y = y;
                                vertex[vertIdx].z = z;
                                vertIdx++;
                                break;
                        }
                        case 'f': {
                                sscanf(line, "f %d %d %d", &i1, &i2, &i3);
                                mesh->tris[faceIdx].v[0] = vertex[i1 - 1];
                                mesh->tris[faceIdx].v[1] = vertex[i2 - 1];
                                mesh->tris[faceIdx].v[2] = vertex[i3 - 1];
                                faceIdx++;
                                break;
                        }
                }
        }
        if (ferror(file)) {
                fprintf(stderr, "I/O error while reading %s", objFile);
                free(vertex);
                free(mesh->tris);
                free(mesh);
                return NULL;
        }

        fclose(file);
        free(vertex);

        return mesh;
}

void MeshDeinit(struct mesh *mesh) {
        if (NULL == mesh) {
                return;
        }

        if (NULL != mesh->tris) {
                free(mesh->tris);
        }

        free(mesh);
}

struct vec3 Mat4x4MultiplyVec3d(struct mat4x4 mat, struct vec3 vec) {
        struct vec3 res;
        res.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
        res.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
        res.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];

        float w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + mat.m[3][3];

        if (0.0f != w) {
                res.x /= w;
                res.y /= w;
                res.z /= w;
        }

        return res;
}

void Mat4x4Debug(struct mat4x4 mat) {
        printf("(struct mat4x4){\n");
        for (int y = 0; y < 4; y++ ) {
                printf("\t");
                for (int x = 0; x < 4; x++) {
                        printf("m[%d][%d] = %.1f ", x, y, mat.m[x][y]);
                }
                printf("\n");
        }
        printf("}\n");
}
