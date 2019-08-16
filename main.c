/******************************************************************************
  File: main.c
  Created: 2019-08-07
  Updated: 2019-08-16
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "graphics.h"
#include "input.h"
#include "math.h"
#include "color.h"

#pragma GCC diagnostic ignored "-Wmissing-braces"

#define S_TO_MS(x) (x) * 1000.0 //!< Convert seconds to milliseconds
#define NS_TO_MS(x) (x) / 1000000.0 //!< Convert nanoseconds to milliseconds
#define MS_TO_NS(x) (x) * 1000000.0 //!< Convert milliseconds to nanoseconds
#define HZ_TO_MS(x) (1.0 / (x)) * 1000.0 //!< Convert hertz to milliseconds per frame

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 256

const double msPerFrame = HZ_TO_MS(60);

//! \brief Compare the Z-Sorting order of two triangles
//!
//! \param left pointer to a triangle
//! \param right pointer to a triangle
//! \return 0 if their sort values are the same, -1 if left comes first, otherwise 1
int TriangleCompareFn(const void *left, const void *right) {
        const struct triangle *l = (const struct triangle *)left;
        const struct triangle *r = (const struct triangle *)right;

        float zl = (l->v[0].z + l->v[1].z + l->v[2].z / 3.0f);
        float zr = (r->v[0].z + r->v[1].z + r->v[2].z / 3.0f);

        if (zl == zr) {
                return 0;
        } else if (zl > zr) {
                return -1;
        } else {
                return 1;
        }
}

int main(int argc, char **argv) {
        struct graphics *graphics = GraphicsInit("3D Software Demo", SCREEN_WIDTH, SCREEN_HEIGHT, 4);
        struct input *input = InputInit();

        struct mesh *ship = MeshInitFromObj("videoship.obj");
        if (NULL == ship) {
                fprintf(stderr, "There was a problem initializing the mesh");
        }

        float near = 0.1f;
        float far = 1000.0f;
        float fov = 90.0f;
        float aspect = (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH;
        float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);

        struct vec3 camera = { 0 };

        struct mat4x4 matProj = { 0 };
        matProj.m[0][0] = aspect * fovRad;
        matProj.m[1][1] = fovRad;
        matProj.m[2][2] = far / (far - near);
        matProj.m[3][2] = (-far * near) / (far - near);
        matProj.m[2][3] = 1.0f;
        matProj.m[3][3] = 0.0f;

        struct timespec progStart;
        clock_gettime(CLOCK_REALTIME, &progStart);
        double totalElapsedTime = 0.0;
        double count = 0.0;

        struct triangle *renderTris = malloc(sizeof(struct triangle) * ship->count);
        int renderTrisCount = 0;

        int running = 1;
        SDL_Event event;
        while (running) {
                count += 0.01;
                struct timespec start;
                clock_gettime(CLOCK_REALTIME, &start);

                totalElapsedTime += S_TO_MS(start.tv_sec - progStart.tv_sec);
                totalElapsedTime += NS_TO_MS(start.tv_nsec - progStart.tv_sec);
                double theta = 1.0f + count; //(totalElapsedTime / 10000.0f);

                struct mat4x4 matRotZ = { 0 };
                matRotZ.m[0][0] = cosf(theta);
                matRotZ.m[0][1] = sinf(theta);
                matRotZ.m[1][0] = -sinf(theta);
                matRotZ.m[1][1] = cosf(theta);
                matRotZ.m[2][2] = 1;
                matRotZ.m[3][3] = 1;

                struct mat4x4 matRotX = { 0 };
                matRotX.m[0][0] = 1;
                matRotX.m[1][1] = cosf(theta * 0.5f);
                matRotX.m[1][2] = sinf(theta * 0.5f);
                matRotX.m[2][1] = -sinf(theta * 0.5f);
                matRotX.m[2][2] = cosf(theta * 0.5f);
                matRotX.m[3][3] = 1;

                GraphicsBegin(graphics);
                GraphicsClearScreen(graphics, 0x000000FF);

                renderTrisCount = 0;
                for (int i = 0; i < ship->count; i++) {
                        struct triangle rotatedz = ship->tris[i];
                        rotatedz.v[0] = Mat4x4MultiplyVec3d(matRotZ, ship->tris[i].v[0]);
                        rotatedz.v[1] = Mat4x4MultiplyVec3d(matRotZ, ship->tris[i].v[1]);
                        rotatedz.v[2] = Mat4x4MultiplyVec3d(matRotZ, ship->tris[i].v[2]);

                        struct triangle rotatedzx = rotatedz;
                        rotatedzx.v[0] = Mat4x4MultiplyVec3d(matRotX, rotatedz.v[0]);
                        rotatedzx.v[1] = Mat4x4MultiplyVec3d(matRotX, rotatedz.v[1]);
                        rotatedzx.v[2] = Mat4x4MultiplyVec3d(matRotX, rotatedz.v[2]);

                        struct triangle translated = rotatedzx;
                        translated.v[0].z = rotatedzx.v[0].z + 8.0f;
                        translated.v[1].z = rotatedzx.v[1].z + 8.0f;
                        translated.v[2].z = rotatedzx.v[2].z + 8.0f;

                        // Calculate the normal.
                        struct vec3 line1;
                        line1.x = translated.v[1].x - translated.v[0].x;
                        line1.y = translated.v[1].y - translated.v[0].y;
                        line1.z = translated.v[1].z - translated.v[0].z;

                        struct vec3 line2;
                        line2.x = translated.v[2].x - translated.v[0].x;
                        line2.y = translated.v[2].y - translated.v[0].y;
                        line2.z = translated.v[2].z - translated.v[0].z;

                        struct vec3 normal = Vec3CrossProduct(line1, line2);
                        Vec3Normalize(&normal);

                        if (Vec3DotProduct(normal, Vec3Subtract(translated.v[0], camera)) < 0) {
                                // Illumination
                                struct vec3 lightDirection = { 0.0f, 0.0f, -1.0f };
                                Vec3Normalize(&lightDirection);

                                // How similar is normal to light direction?
                                float dp = Vec3DotProduct(normal, lightDirection);
                                struct color color = ColorInitFloat(dp, dp, dp, 1.0);

                                struct triangle projected;
                                projected.color = color.rgba;

                                projected.v[0] = Mat4x4MultiplyVec3d(matProj, translated.v[0]);
                                projected.v[1] = Mat4x4MultiplyVec3d(matProj, translated.v[1]);
                                projected.v[2] = Mat4x4MultiplyVec3d(matProj, translated.v[2]);

                                projected.v[0].x += 1.0f; projected.v[0].y += 1.0f;
                                projected.v[1].x += 1.0f; projected.v[1].y += 1.0f;
                                projected.v[2].x += 1.0f; projected.v[2].y += 1.0f;

                                projected.v[0].x *= 0.5f * (float)SCREEN_WIDTH;
                                projected.v[0].y *= 0.5f * (float)SCREEN_HEIGHT;
                                projected.v[1].x *= 0.5f * (float)SCREEN_WIDTH;
                                projected.v[1].y *= 0.5f * (float)SCREEN_HEIGHT;
                                projected.v[2].x *= 0.5f * (float)SCREEN_WIDTH;
                                projected.v[2].y *= 0.5f * (float)SCREEN_HEIGHT;

                                // Store triangles for sorting.
                                renderTris[renderTrisCount] = projected;
                                renderTrisCount++;
                        }
                }

                // Sort the triangles from back to front.
                qsort(renderTris, renderTrisCount, sizeof(struct triangle), TriangleCompareFn);

                for (int i = 0; i < renderTrisCount; i++) {
                        // Draw solid faces.
                        GraphicsTriangleSolid(graphics, renderTris[i], renderTris[i].color);
                        // Draw wireframe faces.
                        GraphicsTriangleWireframe(graphics, renderTris[i], ColorBlack.rgba);
                }

                GraphicsEnd(graphics);

                while (SDL_PollEvent(&event)) {
                        running = !InputIsQuitPressed(input, &event);
                }

                struct timespec end;
                clock_gettime(CLOCK_REALTIME, &end);

                double elapsedTime = S_TO_MS(end.tv_sec - start.tv_sec);
                elapsedTime += NS_TO_MS(end.tv_nsec - start.tv_nsec);

                struct timespec sleep = { .tv_sec = 0, .tv_nsec = MS_TO_NS(msPerFrame - elapsedTime) };
                nanosleep(&sleep, NULL);
        }

        MeshDeinit(ship);
        GraphicsDeinit(graphics);

        return 0;
}
