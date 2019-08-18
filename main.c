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
#include "triangle_list.h"

#pragma GCC diagnostic ignored "-Wmissing-braces"

#define S_TO_MS(x) (x) * 1000.0 //!< Convert seconds to milliseconds
#define NS_TO_MS(x) (x) / 1000000.0 //!< Convert nanoseconds to milliseconds
#define MS_TO_NS(x) (x) * 1000000.0 //!< Convert milliseconds to nanoseconds
#define HZ_TO_MS(x) (1.0 / (x)) * 1000.0 //!< Convert hertz to milliseconds per frame

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

const double msPerFrame = HZ_TO_MS(60);

// Externally exposed vars
struct vec3 camera;
struct vec3 lookDir;
struct vec3 up;
float yaw;
double elapsedTime;

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
        struct timespec progStart;
        clock_gettime(CLOCK_REALTIME, &progStart);

        struct graphics *graphics = GraphicsInit("3D Software Demo", SCREEN_WIDTH, SCREEN_HEIGHT, 1);
        struct input *input = InputInit();

        struct mesh *ship = MeshInitFromObj("teapot.obj");
        if (NULL == ship) {
                fprintf(stderr, "There was a problem initializing the mesh");
        }

        camera = (struct vec3){ 0 };
        yaw = 0;
        elapsedTime = 0;

        struct mat4x4 matProj = Mat4x4Project(90.0f, (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH, 0.1f, 1000.0f);

        struct triangle *renderTris = malloc(sizeof(struct triangle) * ship->count * 2);
        int renderTrisCount = 0;

        double count = 0.0;
        SDL_Event event;
        int running = 1;

        while (running) {
                struct timespec start;
                clock_gettime(CLOCK_REALTIME, &start);

                double theta = 0.0f; //1.0f + count;
                count += 0.01;

                struct mat4x4 matRotZ = Mat4x4RotateZ(theta * 0.5f);
                struct mat4x4 matRotX = Mat4x4RotateX(theta);
                struct mat4x4 matTrans = Mat4x4Translate(0.0f, 0.0f, 8.0f);

                struct mat4x4 matWorld = Mat4x4Identity();
                matWorld = Mat4x4Multiply(matRotZ, matRotX);
                matWorld = Mat4x4Multiply(matWorld, matTrans);

                up = (struct vec3){ 0, 1, 0, 1 };
                struct vec3 target = { 0, 0, 1, 1 };
                struct mat4x4 matCameraRot = Mat4x4RotateY(yaw);
                lookDir = Mat4x4MultiplyVec3(matCameraRot, target);
                target = Vec3Add(camera, lookDir);

                struct mat4x4 matCamera = Mat4x4PointAt(camera, target, up);
                struct mat4x4 matView = Mat4x4InvertFast(matCamera);

                GraphicsBegin(graphics);
                GraphicsClearScreen(graphics, ColorBlack.rgba);

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

                        if (Vec3DotProduct(normal, Vec3Subtract(transformed.v[0], camera)) < 0.0f) {
                                // Illumination
                                struct vec3 lightDirection = { 0.0f, 1.0f, -1.0f };
                                lightDirection = Vec3Normalize(lightDirection);

                                // How similar is normal to light direction?
                                float dp = fmax(0.1f, Vec3DotProduct(normal, lightDirection));
                                struct color color = ColorInitFloat(dp, dp, dp, 1.0);

                                // Convert from world space to view space.
                                struct triangle viewed;
                                viewed.v[0] = Mat4x4MultiplyVec3(matView, transformed.v[0]);
                                viewed.v[1] = Mat4x4MultiplyVec3(matView, transformed.v[1]);
                                viewed.v[2] = Mat4x4MultiplyVec3(matView, transformed.v[2]);
                                viewed.color = color.rgba;

                                struct triangle clipped[2];
                                int numClippedTriangles = TriangleClipAgainstPlane(
                                        (struct vec3){ 0, 0, 0.1f, 1 },
                                        (struct vec3){ 0, 0, 1, 1 },
                                        viewed,
                                        &clipped[0],
                                        &clipped[1]);

                                for (int n = 0; n < numClippedTriangles; n++) {
                                        // Convert from 3D to 2D.
                                        struct triangle projected = clipped[n];
                                        projected.v[0] = Mat4x4MultiplyVec3(matProj, clipped[n].v[0]);
                                        projected.v[1] = Mat4x4MultiplyVec3(matProj, clipped[n].v[1]);
                                        projected.v[2] = Mat4x4MultiplyVec3(matProj, clipped[n].v[2]);

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
                }

                // Sort the triangles from back to front.
                qsort(renderTris, renderTrisCount, sizeof(struct triangle), TriangleCompareFn);

                for (int i = 0; i < renderTrisCount; i++) {
                        // 16 because we potentially get two triangles per clip,
                        // and each triangle can beget up to two more per side.
                        // Side 1: 1 -> 2
                        // Side 2: 2 -> 4
                        // Side 3: 4 -> 8
                        // Side 4: 8 -> 16
                        struct triangle_list triangleList = TriangleListInit();

                        TriangleListPushBack(&triangleList, renderTris[i]);
                        int numNewTriangles = 1;

                        // Now do screen edge clipping.
                        for (int p = 0; p < 4; p++) {
                                struct triangle clipped[2];
                                int numTrisToAdd = 0;
                                while (numNewTriangles > 0) {
                                        struct triangle test = TriangleListPopFront(&triangleList);
                                        numNewTriangles--;

                                        switch(p) {
                                                case 0:
                                                        numTrisToAdd = TriangleClipAgainstPlane(
                                                                (struct vec3){ 0, 0, 0, 1 },
                                                                (struct vec3){ 0, 1, 0, 1 },
                                                                test,
                                                                &clipped[0],
                                                                &clipped[1]);
                                                        break;
                                                case 1:
                                                        numTrisToAdd = TriangleClipAgainstPlane(
                                                                (struct vec3){ 0, (float)SCREEN_HEIGHT - 1, 0, 1 },
                                                                (struct vec3){ 0, -1, 0, 1 },
                                                                test,
                                                                &clipped[0],
                                                                &clipped[1]);
                                                        break;
                                                case 2:
                                                        numTrisToAdd = TriangleClipAgainstPlane(
                                                                (struct vec3){ 0, 0, 0, 1 },
                                                                (struct vec3){ 1, 0, 0, 1 },
                                                                test,
                                                                &clipped[0],
                                                                &clipped[1]);
                                                        break;
                                                case 3:
                                                        numTrisToAdd = TriangleClipAgainstPlane(
                                                                (struct vec3){ (float)SCREEN_WIDTH - 1, 0, 0, 1 },
                                                                (struct vec3){ -1, 0, 0, 1 },
                                                                test,
                                                                &clipped[0],
                                                                &clipped[1]);
                                                        break;
                                        }

                                        for (int w = 0; w < numTrisToAdd; w++) {
                                                TriangleListPushBack(&triangleList, clipped[w]);
                                        }
                                }
                                numNewTriangles = TriangleListSize(&triangleList);
                        }

                        int listSize = TriangleListSize(&triangleList);
                        for (int b = 0; b < listSize; b++) {
                                struct triangle t = TriangleListPopFront(&triangleList);
                                // Draw solid faces.
                                GraphicsTriangleSolid(graphics, t, t.color);
                                // Draw wireframe faces.
                                // GraphicsTriangleWireframe(graphics, t, ColorBlack.rgba);
                        }
                }

                GraphicsEnd(graphics);

                while (SDL_PollEvent(&event)) {
                        running = !InputIsQuitPressed(input, &event);
                }
                InputProcess(input);

                struct timespec end;
                clock_gettime(CLOCK_REALTIME, &end);

                elapsedTime = S_TO_MS(end.tv_sec - start.tv_sec);
                elapsedTime += NS_TO_MS(end.tv_nsec - start.tv_nsec);

                struct timespec sleep = { .tv_sec = 0, .tv_nsec = MS_TO_NS(msPerFrame - elapsedTime) };
                nanosleep(&sleep, NULL);
        }

        free(renderTris);
        MeshDeinit(ship);
        GraphicsDeinit(graphics);

        return 0;
}
