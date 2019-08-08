#include <stdlib.h> // malloc, free
#include <string.h> // memset

struct vec3d {
        union {
                float x, y, z;
                float v[3];
        };
};

struct triangle {
        union {
                struct vec3d p[3];
                float x1, y1, z1, x2, y2, z2, x3, y3, z3;
                float v[9];
        };
};

struct mesh {
        struct triangle *tris;
        int count;
};

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

struct mat4x4 {
        union {
                struct vec3d v[4];
                float m[4][4];
        };
};

struct vec3d Mat4x4MultiplyVec3d(struct mat4x4 *mat, struct vec3d *vec) {
        struct vec3d res;
        res.x = vec->x * mat->m[0][0] + vec->y * mat->m[1][0] + vec->z * mat->m[2][0] * mat->m[3][0];
        res.y = vec->x * mat->m[0][1] + vec->y * mat->m[1][1] + vec->z * mat->m[2][1] * mat->m[3][1];
        res.z = vec->x * mat->m[0][2] + vec->y * mat->m[1][2] + vec->z * mat->m[2][2] * mat->m[3][2];

        float w = vec->x * mat->m[0][3] + vec->y * mat->m[1][3] + vec->z * mat->m[2][3] * mat->m[3][3];

        if (0.0f != w) {
                res.x /= w;
                res.y /= w;
                res.z /= w;
        }

        return res;
}
