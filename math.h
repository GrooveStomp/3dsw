/******************************************************************************
  File: math.h
  Created: 2019-08-13
  Updated: 2019-08-22
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/

//! \file math.h

//! A collection of vector and matrix types and associated functions.

#ifndef MATH_VERSION
//! include guard
#define MATH_VERSION "0.1.0"

struct vec2 {
        union {
                struct {
                        float u;
                        float v;
                        float w;
                };
                float p[3];
        };
};

struct vec3 {
        union {
                struct {
                        float x;
                        float y;
                        float z;
                        float w;
                };
                float p[4];
        };
};

struct triangle {
        union {
                struct {
                        float x1;
                        float y1;
                        float z1;
                        float w1;
                        float x2;
                        float y2;
                        float z2;
                        float w2;
                        float x3;
                        float y3;
                        float z3;
                        float w3;
                };
                struct vec3 v[3];
        };
        union {
                struct {
                        float u1;
                        float v1;
                        float tw1;
                        float u2;
                        float v2;
                        float tw2;
                        float u3;
                        float v3;
                        float tw3;
                };
                struct vec2 t[3];
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
Vec3Debug(struct vec3 vec3, char *name);

struct vec3
Vec3Init(float x, float y, float z);

float
Vec3DotProduct(struct vec3 left, struct vec3 right);

struct vec3
Vec3CrossProduct(struct vec3 left, struct vec3 right);

struct vec3
Vec3Normalize(struct vec3 vec3);

struct vec3
Vec3Add(struct vec3 left, struct vec3 right);

struct vec3
Vec3Subtract(struct vec3 minuend, struct vec3 subtrahend);

struct vec3
Vec3Multiply(struct vec3 vec3, float f);

struct vec3
Vec3Divide(struct vec3 vec3, float f);

//! \brief Return the 3d point where a line intersects a plane.
//!
//! \param[in] plane 3d point on the plane to intersect against
//! \param[in] normal normal of the plane to intersect against
//! \param[in] lineStart
//! \param[in] lineEnd
//! \param[out] t where on the line the intersection occurs.
struct vec3
Vec3IntersectPlane(struct vec3 plane, struct vec3 normal, struct vec3 lineStart, struct vec3 lineEnd, float *t);

//! \brief Create a new triangle with the given params.
struct triangle TriangleInit(
        float x1, float y1, float z1,
        float u1, float v1,
        float x2, float y2, float z2,
        float u2, float v2,
        float x3, float y3, float z3,
        float u3, float v3);

//! \brief Clip a triangle against a plane
//!
//! Given a triangle and a plane, if some of the points lie on one side of the
//! plane and some on the other, then split the triangle into 1 or 2 smaller
//! triangles that do not pass through the plane.
//!
//! \param plane point on the plane to clip against
//! \param normal normal of the plane to clip against
//! \param in the triangle to clip
//! \param out1 one of the smaller triangles resulting from the clipping, if necessary
//! \param out2 one of the smaller triangles resulting from the clipping, if necessary
//! \return 0 if all points are outside the plane, 1 if the triangle is not
//! clipped, otherwise the number of smaller triangles resulting from clipping.
int
TriangleClipAgainstPlane(struct vec3 plane, struct vec3 normal, struct triangle in, struct triangle *out1, struct triangle *out2);

void
TriangleDebug(struct triangle triangle, char *name);

struct mesh *
MeshInit(int numTris);

struct mesh *
MeshInitFromObj(char *objFile);

void
MeshDeinit(struct mesh *mesh);

struct vec3
Mat4x4MultiplyVec3(struct mat4x4 mat, struct vec3 vec);

struct mat4x4
Mat4x4Identity();

struct mat4x4
Mat4x4RotateX(float rad);

struct mat4x4
Mat4x4RotateY(float rad);

struct mat4x4
Mat4x4RotateZ(float rad);

struct mat4x4
Mat4x4Translate(float x, float y, float z);

struct mat4x4
Mat4x4Project(float fovDegrees, float aspect, float near, float far);

struct mat4x4
Mat4x4Multiply(struct mat4x4 left, struct mat4x4 right);

//! \brief Fast matrix inverse. Doesn't work if matrix does scaling.
struct mat4x4
Mat4x4InvertFast(struct mat4x4 matrix);

struct mat4x4
Mat4x4PointAt(struct vec3 pos, struct vec3 target, struct vec3 up);

void
Mat4x4Debug(struct mat4x4 mat, char *name);

#endif // MATH_VERSION
