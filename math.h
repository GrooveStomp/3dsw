/******************************************************************************
  File: math.h
  Created: 2019-08-13
  Updated: 2019-08-16
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/

//! \file math.h

//! A collection of vector and matrix types and associated functions.

#ifndef MATH_VERSION
//! include guard
#define MATH_VERSION "0.1.0"

struct vec3 {
        union {
                struct {
                        float x;
                        float y;
                        float z;
                };
                float p[3];
        };
};

struct triangle {
        union {
                struct {
                        float x1;
                        float y1;
                        float z1;
                        float x2;
                        float y2;
                        float z2;
                        float x3;
                        float y3;
                        float z3;
                };
                float p[9];
                struct vec3 v[3];
        };
        unsigned int color;
};

struct mesh {
        struct triangle *tris;
        int count;
};

struct mat4x4 {
        float m[4][4];
};

void
Vec3Debug(struct vec3 vec3);

float
Vec3DotProduct(struct vec3 left, struct vec3 right);

struct vec3
Vec3CrossProduct(struct vec3 left, struct vec3 right);

void
Vec3Normalize(struct vec3 *vec3);

struct vec3
Vec3Subtract(struct vec3 minuend, struct vec3 subtrahend);

void
TriangleDebug(struct triangle triangle);

struct mesh *
MeshInit(int numTris);

struct mesh *
MeshInitFromObj(char *objFile);

void
MeshDeinit(struct mesh *mesh);

struct vec3
Mat4x4MultiplyVec3d(struct mat4x4 mat, struct vec3 vec);

void
Mat4x4Debug(struct mat4x4 mat);

#endif // MATH_VERSION
