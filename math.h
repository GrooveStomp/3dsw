/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: math.h
  Created: 2019-08-13
  Updated: 2019-08-27
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file math.h
//! A collection of vector and matrix types and associated functions.

#ifndef MATH_VERSION
#define MATH_VERSION "0.1.0" //!< include guard

//! \brief Homogenous 2D coordinates
//!
//! A 3-element structure used to represent 2D coordinate space with 2 cartesian
//! coordinates and a homogenous 3rd coordinate for projection.
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

//! \brief Homogenous 3D coordinates
//!
//! A 4-element structure used to represent 3D coordinate space with 3 cartesian
//! coordinates and a homogeouns 4th coordinate for projection.
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

//! \brief Triangular mesh face
//!
//! A mesh face consisting solely of homogenous 3D coordinates and homogenous 2D
//! texture coordinates, and a color attribute.
//!
//! Currently the color attribute is not used, but it is useful for debugging or
//! falling back to in lieu of texture values or coordinate values.
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

//! A collection of triangles representing some kind of 3D model.
struct mesh {
        struct triangle *tris;
        int count;
};

//! A 3D matrix using homogenous coordinates.
struct mat4x4 {
        float m[4][4];
};

//! \brief Prints debug information about the homogenous 3D vector
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
//! \param[in] plane point on the plane to clip against
//! \param[in] normal normal of the plane to clip against
//! \param[in] in the triangle to clip
//! \param[out] out1 one of the smaller triangles resulting from the clipping, if necessary
//! \param[out] out2 one of the smaller triangles resulting from the clipping, if necessary
//! \return 0 if all points are outside the plane, 1 if the triangle is not
//! clipped, otherwise the number of smaller triangles resulting from clipping.
int
TriangleClipAgainstPlane(struct vec3 plane, struct vec3 normal, struct triangle in, struct triangle *out1, struct triangle *out2);

//! \brief Prints debug information about the triangle face.
void
TriangleDebug(struct triangle triangle, char *name);

//! \brief Initialize a new mesh object
//!
//! This function is useful if a mesh needs to be explicitly, manually defined.
//! Initialization here only refers to setting a valid "zero" state.
//!
//! \param[in] numTris the number of triangular faces the mesh will hold.
//! \return a properly "zeroed" mesh object that can support numTris faces.
struct mesh *
MeshInit(int numTris);

//! \brief Initialize a new mesh object from an obj file
//!
//! Loads the specified obj file and constructs a new mesh object representation of it.
//!
//! Note: This doesn't read material definitions properly, instead explicitly loading a predefined texture out-of-band and applying it if there are texture coordinates in the obj file.
//!
//! \param[in] objFile path to the obj file to read
//! \return a mesh object representing the data in the obj file
//!
//! \see \ref features
struct mesh *
MeshInitFromObj(char *objFile);

//! \brief De-initializes the mesh object
//!
//! Frees any memory allocated by the object and frees the pointer to the object itself.
//!
//! \param[in,out] mesh The mesh to be de-initialized
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

//! \brief Prints debug information about the 4x4 matrix.
void
Mat4x4Debug(struct mat4x4 mat, char *name);

#endif // MATH_VERSION
