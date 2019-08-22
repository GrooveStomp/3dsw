/******************************************************************************
  File: math.c
  Created: 2019-08-07
  Updated: 2019-08-17
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/
#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <stdio.h> // printf
#include <math.h> // sqrtf

#include "math.h"
#include "color.h"

struct vec3 Vec3Init(float x, float y, float z) {
        struct vec3 vec3;
        vec3.x = x;
        vec3.y = y;
        vec3.z = z;
        vec3.w = 1.0f;
        return vec3;
}

void Vec3Debug(struct vec3 v, char *name) {
        if (NULL != name)
                printf("struct vec3 %s = ", name);
        else
                printf("(struct vec3)");

        printf("{ .x = %.1f, .y = %.1f, .z = %.1f, .w = %.1f }\n",
               v.x, v.y, v.z, v.w);

        if (NULL != name)
                printf("struct vec3 %s = ", name);
        else
                printf("(struct vec3)");
        printf("{ .p = { %.1f, %.1f, %.1f, %.1f } }\n",
               v.p[0], v.p[1], v.p[2], v.p[3]);
}

void TriangleDebug(struct triangle t, char *name) {
        if (NULL != name)
                printf("struct triangle %s = ", name);
        else
                printf("(struct triangle)");

        printf(
"{\n"
"  .x1 = %.1f, .y1 = %.1f, .z1 = %.1f, .w1 = %.1f,\n"
"  .x2 = %.1f, .y2 = %.1f, .z2 = %.1f, .w2 = %.1f,\n"
"  .x3 = %.1f, .y3 = %.1f, .z3 = %.1f, .w3 = %.1f,\n"
"  .u1 = %.1f, .v1 = %.1f,\n"
"  .u2 = %.2f, .v2 = %.2f,\n"
"  .u3 = %.3f, .v3 = %.3f\n"
"}\n",
               t.x1, t.y1, t.z1, t.w1,
               t.x2, t.y2, t.z2, t.w2,
               t.x3, t.y3, t.z3, t.w3,
               t.u1, t.v1,
               t.u2, t.v2,
               t.u3, t.v3);

        if (NULL != name)
                printf("struct triangle %s = ", name);
        else
                printf("(struct triangle)");

        printf(
"{\n"
"  .v0 = { %.1f, %.1f, %.1f },\n"
"  .v1 = { %.1f, %.1f, %.1f },\n"
"  .v2 = { %.1f, %.1f, %.1f },\n"
"  .t0 = { %.1f, %.1f },\n"
"  .t1 = { %.1f, %.1f },\n"
"  .t2 = { %.1f, %.1f }\n"
"}\n",
               t.v[0].x, t.v[0].y, t.v[0].z,
               t.v[1].x, t.v[1].y, t.v[1].z,
               t.v[2].x, t.v[2].y, t.v[2].z,
               t.t[0].u, t.t[0].v,
               t.t[1].u, t.t[1].v,
               t.t[2].u, t.t[2].v);
}

float Vec3DotProduct(struct vec3 left, struct vec3 right) {
        return left.x * right.x + left.y * right.y + left.z * right.z;
}

struct vec3 Vec3CrossProduct(struct vec3 left, struct vec3 right) {
        struct vec3 cross = { 0 };

        cross.x = left.y * right.z - left.z * right.y;
        cross.y = left.z * right.x - left.x * right.z;
        cross.z = left.x * right.y - left.y * right.x;
        cross.w = 1.0f;

        return cross;
}

struct vec3 Vec3Normalize(struct vec3 vec3) {
        float len = sqrtf(Vec3DotProduct(vec3, vec3));

        struct vec3 res = { 0 };
        res.x = vec3.x / len;
        res.y = vec3.y / len;
        res.z = vec3.z / len;
        res.w = 1.0f;
        return res;
}

struct vec3 Vec3Add(struct vec3 left, struct vec3 right) {
        struct vec3 res = { 0 };
        res.x = left.x + right.x;
        res.y = left.y + right.y;
        res.z = left.z + right.z;
        res.w = 1.0f;
        return res;
}


struct vec3 Vec3Subtract(struct vec3 minuend, struct vec3 subtrahend) {
        struct vec3 res = { 0 };
        res.x = minuend.x - subtrahend.x;
        res.y = minuend.y - subtrahend.y;
        res.z = minuend.z - subtrahend.z;
        res.w = 1.0f;
        return res;
}

struct vec3 Vec3Multiply(struct vec3 vec3, float f) {
        struct vec3 res = { 0 };
        res.x = vec3.x * f;
        res.y = vec3.y * f;
        res.z = vec3.z * f;
        res.w = 1.0f;
        return res;
}

struct vec3 Vec3Divide(struct vec3 vec3, float f) {
        struct vec3 res = { 0 };
        res.x = vec3.x / f;
        res.y = vec3.y / f;
        res.z = vec3.z / f;
        res.w = 1.0f;
        return res;
}

struct vec3 Vec3IntersectPlane(struct vec3 plane, struct vec3 normal, struct vec3 lineStart, struct vec3 lineEnd, float *t) {
        normal = Vec3Normalize(normal);
        float planeD = -Vec3DotProduct(normal, plane);
        float ad = Vec3DotProduct(lineStart, normal);
        float bd = Vec3DotProduct(lineEnd, normal);
        *t = (-planeD - ad) / (bd - ad);
        struct vec3 lineStartToEnd = Vec3Subtract(lineEnd, lineStart);
        struct vec3 lineToIntersect = Vec3Multiply(lineStartToEnd, *t);
        return Vec3Add(lineStart, lineToIntersect);
}

//! \brief Return shortest distance from point to plan given a normalized normal.
float ShortestDistToPlane(struct vec3 point, struct vec3 planePoint, struct vec3 planeNormal) {
        return Vec3DotProduct(planeNormal, point) - Vec3DotProduct(planeNormal, planePoint);
}

struct triangle TriangleInit(
        float x1, float y1, float z1,
        float u1, float v1,
        float x2, float y2, float z2,
        float u2, float v2,
        float x3, float y3, float z3,
        float u3, float v3) {

        struct triangle t;
        t.x1 = x1; t.y1 = y1; t.z1 = z1; t.w1 = 1.0f; t.u1 = u1; t.v1 = v1;
        t.x2 = x2; t.y2 = y2; t.z2 = z2; t.w2 = 1.0f; t.u2 = u2; t.v2 = v2;
        t.x3 = x3; t.y3 = y3; t.z3 = z3; t.w3 = 1.0f; t.u3 = u3; t.v3 = v3;
        return t;
}

int TriangleClipAgainstPlane(struct vec3 plane, struct vec3 normal, struct triangle in, struct triangle *out1, struct triangle *out2) {
        normal = Vec3Normalize(normal);

        struct vec3* insidePoints[3];  int insidePointsCount = 0;
        struct vec3* outsidePoints[3]; int outsidePointsCount = 0;
        struct vec2* insideTex[3];     int insideTexCount = 0;
        struct vec2* outsideTex[3];    int outsideTexCount = 0;

        float d0 = ShortestDistToPlane(in.v[0], plane, normal);
        float d1 = ShortestDistToPlane(in.v[1], plane, normal);
        float d2 = ShortestDistToPlane(in.v[2], plane, normal);

        if (d0 >= 0 ) {
                insidePoints[insidePointsCount++] = &in.v[0];
                insideTex[insideTexCount++] = &in.t[0];
        } else {
                outsidePoints[outsidePointsCount++] = &in.v[0];
                outsideTex[outsideTexCount++] = &in.t[0];
        }

        if (d1 >= 0 ) {
                insidePoints[insidePointsCount++] = &in.v[1];
                insideTex[insideTexCount++] = &in.t[1];
        } else {
                outsidePoints[outsidePointsCount++] = &in.v[1];
                outsideTex[outsideTexCount++] = &in.t[1];
        }

        if (d2 >= 0 ) {
                insidePoints[insidePointsCount++] = &in.v[2];
                insideTex[insideTexCount++] = &in.t[2];
        } else {
                outsidePoints[outsidePointsCount++] = &in.v[2];
                outsideTex[outsideTexCount++] = &in.t[2];
        }

        // Now classify the triangle points.

        if (insidePointsCount == 0) {
                // All points are outside of the plan, so clip the whole
                // triangle.
                return 0; // No returned triangles are valid.
        }

        if (insidePointsCount == 3) {
                // All points lie on the inside of the plan, so do nothing.
                *out1 = in;

                return 1; // Just one returned triangle is valid.
        }

        if (insidePointsCount == 1 && outsidePointsCount == 2) {
                float t;
                // The triangle should be clipped as two points lie outside the
                // plane.  Create a new, smaller triangle.

                *out1 = in;
                // The inside point is valid, so keep it.
                out1->v[0] = *insidePoints[0];
                // out1->color = ColorRed.rgba; // Debug color.
                out1->t[0] = *insideTex[0];

                // The two new points are at the locations where the original
                // sides of the triangle (lines) intersect with the plane.
                out1->v[1] = Vec3IntersectPlane(plane, normal, *insidePoints[0], *outsidePoints[0], &t);
                out1->u2 = t * (outsideTex[0]->u - insideTex[0]->u) + insideTex[0]->u;
                out1->v2 = t * (outsideTex[0]->v - insideTex[0]->v) + insideTex[0]->v;

                out1->v[2] = Vec3IntersectPlane(plane, normal, *insidePoints[0], *outsidePoints[1], &t);
                out1->u3 = t * (outsideTex[1]->u - insideTex[0]->u) + insideTex[0]->u;
                out1->v3 = t * (outsideTex[1]->v - insideTex[0]->v) + insideTex[0]->v;

                return 1; // Just one returned triangle is valid.
        }

        if (insidePointsCount == 2 && outsidePointsCount == 1) {
                float t;
                // The triangle should be clipped. Two points lie inside the
                // plane. When we clip we create a rectangle, but we subdivide
                // this into two triangles.

                *out1 = in;
                // out1->color = ColorGreen.rgba; // Debug color.
                *out2 = in;
                // out2->color = ColorBlue.rgba; // Debug color.

                // The first triangle consists of the two inside points and a
                // new point determined by the location where one side of the
                // triangle intersects with the plane.
                out1->v[0] = *insidePoints[0];
                out1->v[1] = *insidePoints[1];
                out1->t[0] = *insideTex[0];
                out1->t[1] = *insideTex[1];

                out1->v[2] = Vec3IntersectPlane(plane, normal, *insidePoints[0], *outsidePoints[0], &t);
                out1->u3 = t * (outsideTex[0]->u - insideTex[0]->u) + insideTex[0]->u;
                out1->v3 = t * (outsideTex[0]->v - insideTex[0]->v) + insideTex[0]->v;

                // The second triangle is composed of one of the inside points,
                // a new point determined by the intersection of the other side
                // of the triangle and the plane, and the newly created point
                // above.
                out2->v[0] = *insidePoints[1];
                out2->t[0] = *insideTex[1];
                out2->v[1] = out1->v[2];
                out2->t[1] = out1->t[2];
                out2->v[2] = Vec3IntersectPlane(plane, normal, *insidePoints[1], *outsidePoints[0], &t);
                out2->u3 = t * (outsideTex[0]->u - insideTex[1]->u) + insideTex[1]->u;
                out2->v3 = t * (outsideTex[0]->v - insideTex[1]->v) + insideTex[1]->v;

                return 2; // Return two newly formed triangles wich form a quad.
        }

        return 0;
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
                                vertex[vertIdx].w = 1.0f;
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

struct vec3 Mat4x4MultiplyVec3(struct mat4x4 mat, struct vec3 vec) {
        struct vec3 res;
        res.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + vec.w * mat.m[3][0];
        res.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + vec.w * mat.m[3][1];
        res.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + vec.w * mat.m[3][2];
        res.w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + vec.w * mat.m[3][3];
        return res;
}

struct mat4x4 Mat4x4Identity() {
        struct mat4x4 res = { 0 };
        res.m[0][0] = 1.0f;
        res.m[1][1] = 1.0f;
        res.m[2][2] = 1.0f;
        res.m[3][3] = 1.0f;
        return res;
}

struct mat4x4 Mat4x4RotateX(float rad) {
        struct mat4x4 res = { 0 };
        res.m[0][0] = 1.0f;
        res.m[1][1] = cosf(rad);
        res.m[1][2] = sinf(rad);
        res.m[2][1] = -sinf(rad);
        res.m[2][2] = cosf(rad);
        res.m[3][3] = 1.0f;
        return res;
}

struct mat4x4 Mat4x4RotateY(float rad) {
        struct mat4x4 res = { 0 };
        res.m[0][0] = cosf(rad);
        res.m[0][2] = sinf(rad);
        res.m[2][0] = -sinf(rad);
        res.m[1][1] = 1.0f;
        res.m[2][2] = cosf(rad);
        res.m[3][3] = 1.0f;
        return res;
}

struct mat4x4 Mat4x4RotateZ(float rad) {
        struct mat4x4 res = { 0 };
        res.m[0][0] = cosf(rad);
        res.m[0][1] = sinf(rad);
        res.m[1][0] = -sinf(rad);
        res.m[1][1] = cosf(rad);
        res.m[2][2] = 1.0f;
        res.m[3][3] = 1.0f;
        return res;
}

struct mat4x4 Mat4x4Translate(float x, float y, float z) {
        struct mat4x4 res = Mat4x4Identity();
        res.m[3][0] = x;
        res.m[3][1] = y;
        res.m[3][2] = z;
        return res;
}

struct mat4x4 Mat4x4Project(float fovDegrees, float aspect, float near, float far) {
        float fovRad = 1.0f / tanf(fovDegrees * 0.5f / 180.0f * 3.14159f);

        struct mat4x4 res = { 0 };
        res.m[0][0] = aspect * fovRad;
        res.m[1][1] = fovRad;
        res.m[2][2] = far / (far - near);
        res.m[3][2] = (-far * near) / (far - near);
        res.m[2][3] = 1.0f;
        res.m[3][3] = 0.0f;
        return res;
}

struct mat4x4 Mat4x4Multiply(struct mat4x4 left, struct mat4x4 right) {
        struct mat4x4 res = { 0 };
        for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                        res.m[row][col] =
                                left.m[row][0] * right.m[0][col] +
                                left.m[row][1] * right.m[1][col] +
                                left.m[row][2] * right.m[2][col] +
                                left.m[row][3] * right.m[3][col];
                }
        }
        return res;
}

struct mat4x4 Mat4x4InvertFast(struct mat4x4 matrix) {
        struct mat4x4 res = { 0 };
        res.m[0][0] = matrix.m[0][0];
        res.m[1][0] = matrix.m[0][1];
        res.m[2][0] = matrix.m[0][2];

        res.m[0][1] = matrix.m[1][0];
        res.m[1][1] = matrix.m[1][1];
        res.m[2][1] = matrix.m[1][2];

        res.m[0][2] = matrix.m[2][0];
        res.m[1][2] = matrix.m[2][1];
        res.m[2][2] = matrix.m[2][2];

        res.m[3][0] = -(matrix.m[3][0] * res.m[0][0] + matrix.m[3][1] * res.m[1][0] + matrix.m[3][2] * res.m[2][0]);
        res.m[3][1] = -(matrix.m[3][0] * res.m[0][1] + matrix.m[3][1] * res.m[1][1] + matrix.m[3][2] * res.m[2][1]);
        res.m[3][2] = -(matrix.m[3][0] * res.m[0][2] + matrix.m[3][1] * res.m[1][2] + matrix.m[3][2] * res.m[2][2]);

        res.m[3][3] = 1.0f;
        return res;
}

struct mat4x4 Mat4x4PointAt(struct vec3 pos, struct vec3 target, struct vec3 up) {
        struct vec3 forward = Vec3Subtract(target, pos);
        forward = Vec3Normalize(forward);

        struct vec3 a = Vec3Multiply(forward, Vec3DotProduct(up, forward));
        struct vec3 newUp = Vec3Subtract(up, a);
        newUp = Vec3Normalize(newUp);

        struct vec3 right = Vec3CrossProduct(newUp, forward);

        struct mat4x4 res;
        res.m[0][0] = right.x;   res.m[0][1] = right.y;   res.m[0][2] = right.z;   res.m[0][3] = 0.0f;
        res.m[1][0] = newUp.x;   res.m[1][1] = newUp.y;   res.m[1][2] = newUp.z;   res.m[1][3] = 0.0f;
        res.m[2][0] = forward.x; res.m[2][1] = forward.y; res.m[2][2] = forward.z; res.m[2][3] = 0.0f;
        res.m[3][0] = pos.x;     res.m[3][1] = pos.y;     res.m[3][2] = pos.z;     res.m[3][3] = 1.0f;
        return res;
}

void Mat4x4Debug(struct mat4x4 mat, char *name) {
        if (NULL != name)
                printf("struct mat4x4 %s = ", name);
        else
                printf("(struct mat4x4)");

        printf("{\n");
        for (int y = 0; y < 4; y++ ) {
                printf("\t");
                for (int x = 0; x < 4; x++) {
                        printf("m[%d][%d] = %.1f ", x, y, mat.m[x][y]);
                }
                printf("\n");
        }
        printf("}\n");
}
