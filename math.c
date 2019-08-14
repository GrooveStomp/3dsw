/******************************************************************************
  File: math.c
  Created: 2019-08-07
  Updated: 2019-08-13
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/
#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <stdio.h> // printf

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

struct mesh *MeshInit(int numTris) {
        struct mesh *mesh = (struct mesh *)malloc(sizeof(struct mesh));
        memset(mesh, 0, sizeof(struct mesh));

        mesh->tris = (struct triangle *)malloc(sizeof(struct triangle) * numTris);
        memset(mesh->tris, 0, sizeof(struct triangle) * numTris);

        mesh->count = numTris;

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
