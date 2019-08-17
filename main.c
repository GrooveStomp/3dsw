/******************************************************************************
  File: main.c
  Created: 2019-08-07
  Updated: 2019-08-17
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

        struct vec3 camera = { 0 };
        struct mat4x4 matProj = Mat4x4Project(90.0f, (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH, 0.1f, 1000.0f);

        struct triangle *renderTris = malloc(sizeof(struct triangle) * ship->count);
        int renderTrisCount = 0;

        double count = 0.0;
        int running = 1;
        SDL_Event event;

        while (running) {
                double theta = 1.0f + count;
                count += 0.01;
                struct timespec start;
                clock_gettime(CLOCK_REALTIME, &start);

                struct mat4x4 matRotZ = Mat4x4RotateZ(theta * 0.5f);
                struct mat4x4 matRotX = Mat4x4RotateX(theta);
                struct mat4x4 matTrans = Mat4x4Translate(0.0f, 0.0f, 8.0f);

                struct mat4x4 matWorld = Mat4x4Identity();
                matWorld = Mat4x4Multiply(matRotZ, matRotX);
                matWorld = Mat4x4Multiply(matWorld, matTrans);

                GraphicsBegin(graphics);
                GraphicsClearScreen(graphics, 0x000000FF);

                renderTrisCount = 0;
                for (int i = 0; i < ship->count; i++) {
                        struct triangle transformed = ship->tris[i];
                        transformed.v[0] = Mat4x4MultiplyVec3(matWorld, transformed.v[0]);
                        transformed.v[1] = Mat4x4MultiplyVec3(matWorld, transformed.v[1]);
                        transformed.v[2] = Mat4x4MultiplyVec3(matWorld, transformed.v[2]);

                        // Calculate the normal.
                        struct vec3 line1 = Vec3Subtract(transformed.v[1], transformed.v[0]);
                        struct vec3 line2 = Vec3Subtract(transformed.v[2], transformed.v[0]);
                        struct vec3 normal = Vec3CrossProduct(line1, line2);
                        normal = Vec3Normalize(normal);

                        if (Vec3DotProduct(normal, Vec3Subtract(transformed.v[0], camera)) < 0) {
                                // Illumination
                                struct vec3 lightDirection = { 0.0f, 0.0f, -1.0f };
                                lightDirection = Vec3Normalize(lightDirection);

                                // How similar is normal to light direction?
                                float dp = Vec3DotProduct(normal, lightDirection);
                                struct color color = ColorInitFloat(dp, dp, dp, 1.0);

                                struct triangle projected;
                                projected.color = color.rgba;

                                projected.v[0] = Mat4x4MultiplyVec3(matProj, transformed.v[0]);
                                projected.v[1] = Mat4x4MultiplyVec3(matProj, transformed.v[1]);
                                projected.v[2] = Mat4x4MultiplyVec3(matProj, transformed.v[2]);

                                projected.v[0] = Vec3Divide(projected.v[0], projected.v[0].w);
                                projected.v[1] = Vec3Divide(projected.v[1], projected.v[1].w);
                                projected.v[2] = Vec3Divide(projected.v[2], projected.v[2].w);

                                struct vec3 offset = { 1, 1, 0, 0 };
                                projected.v[0] = Vec3Add(projected.v[0], offset);
                                projected.v[1] = Vec3Add(projected.v[1], offset);
                                projected.v[2] = Vec3Add(projected.v[2], offset);

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
