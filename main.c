/******************************************************************************
  File: main.c
  Created: 2019-08-07
  Updated: 2019-08-07
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
#include "math.c"

#pragma GCC diagnostic ignored "-Wmissing-braces"

#define S_TO_MS(x) (x) * 1000.0 //!< Convert seconds to milliseconds
#define NS_TO_MS(x) (x) / 1000000.0 //!< Convert nanoseconds to milliseconds
#define MS_TO_NS(x) (x) * 1000000.0 //!< Convert milliseconds to nanoseconds
#define HZ_TO_MS(x) (1.0 / (x)) * 1000.0 //!< Convert hertz to milliseconds per frame

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 256

const double msPerFrame = HZ_TO_MS(60);

int main(int argc, char **argv) {
        struct graphics *graphics = GraphicsInit("3D Software Demo", SCREEN_WIDTH, SCREEN_HEIGHT);
        struct input *input = InputInit();
        struct mesh *mesh = MeshInit(12);

        // South
        mesh->tris[0] = (struct triangle){ .x1 = 0.0, .y1 = 0.0, .z1 = 0.0,  .x2 = 0.0, .y2 = 1.0, .z2 = 0.0,  .x3 = 1.0, .y3 = 1.0, .z3 = 0.0};
        mesh->tris[1] = (struct triangle){ .x1 = 0.0, .y1 = 0.0, .z1 = 0.0,  .x2 = 1.0, .y2 = 1.0, .z2 = 0.0,  .x3 = 1.0, .y3 = 0.0, .z3 = 0.0};

        // East
        mesh->tris[2] = (struct triangle){ .x1 = 1.0, .y1 = 0.0, .z1 = 0.0,  .x2 = 1.0, .y2 = 1.0, .z2 = 0.0,  .x3 = 1.0, .y3 = 1.0, .z3 = 1.0};
        mesh->tris[3] = (struct triangle){ .x1 = 1.0, .y1 = 0.0, .z1 = 0.0,  .x2 = 1.0, .y2 = 1.0, .z2 = 1.0,  .x3 = 1.0, .y3 = 0.0, .z3 = 1.0};

        // North
        mesh->tris[4] = (struct triangle){ .x1 = 1.0, .y1 = 0.0, .z1 = 1.0,  .x2 = 1.0, .y2 = 1.0, .z2 = 1.0,  .x3 = 0.0, .y3 = 1.0, .z3 = 1.0};
        mesh->tris[5] = (struct triangle){ .x1 = 1.0, .y1 = 0.0, .z1 = 1.0,  .x2 = 0.0, .y2 = 1.0, .z2 = 1.0,  .x3 = 0.0, .y3 = 0.0, .z3 = 1.0};

        // West
        mesh->tris[6] = (struct triangle){ .x1 = 0.0, .y1 = 0.0, .z1 = 1.0,  .x2 = 0.0, .y2 = 1.0, .z2 = 1.0,  .x3 = 0.0, .y3 = 1.0, .z3 = 0.0};
        mesh->tris[7] = (struct triangle){ .x1 = 0.0, .y1 = 0.0, .z1 = 1.0,  .x2 = 0.0, .y2 = 1.0, .z2 = 0.0,  .x3 = 0.0, .y3 = 0.0, .z3 = 0.0};

        // Top
        mesh->tris[8] = (struct triangle){ .x1 = 0.0, .y1 = 1.0, .z1 = 0.0,  .x2 = 0.0, .y2 = 1.0, .z2 = 1.0,  .x3 = 1.0, .y3 = 1.0, .z3 = 1.0};
        mesh->tris[9] = (struct triangle){ .x1 = 0.0, .y1 = 1.0, .z1 = 0.0,  .x2 = 1.0, .y2 = 1.0, .z2 = 1.0,  .x3 = 1.0, .y3 = 1.0, .z3 = 0.0};

        // Bottom
        mesh->tris[10] = (struct triangle){ .x1 = 0.0, .y1 = 0.0, .z1 = 1.0,  .x2 = 0.0, .y2 = 0.0, .z2 = 0.0,  .x3 = 1.0, .y3 = 0.0, .z3 = 0.0};
        mesh->tris[11] = (struct triangle){ .x1 = 0.0, .y1 = 0.0, .z1 = 1.0,  .x2 = 1.0, .y2 = 0.0, .z2 = 0.0,  .x3 = 1.0, .y3 = 0.0, .z3 = 1.0};

        float near = 0.1f;
        float far = 1000.0f;
        float fov = 90.0f;
        float aspect = 512.0 / 256.0;
        float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);

        struct mat4x4 matProj;
        matProj.m[0][0] = aspect * fovRad;
        matProj.m[1][1] = fovRad;
        matProj.m[2][2] = far / (far - near);
        matProj.m[3][2] = (-far * near) / (far - near);
        matProj.m[2][3] = 1.0f;
        matProj.m[3][3] = 0.0f;

        struct mat4x4 matRotZ;

        struct mat4x4 matRotX;

        struct timespec progStart;
        clock_gettime(CLOCK_REALTIME, &progStart);
        double totalElapsedTime = 0.0;

        int running = 1;
        SDL_Event event;
        while (running) {
                struct timespec start;
                clock_gettime(CLOCK_REALTIME, &start);

                totalElapsedTime += S_TO_MS(start.tv_sec - progStart.tv_sec);
                totalElapsedTime += NS_TO_MS(start.tv_nsec - progStart.tv_sec);
                float theta = 1.0f + totalElapsedTime;

                GraphicsClearScreen(graphics, 0x00000000);

                matRotZ.m[0][0] = cosf(theta);
                matRotZ.m[0][1] = sinf(theta);
                matRotZ.m[1][0] = -sinf(theta);
                matRotZ.m[1][1] = cosf(theta);
                matRotZ.m[2][2] = 1;
                matRotZ.m[3][3] = 1;

                matRotX.m[0][0] = 1;
                matRotX.m[1][1] = cosf(theta * 0.5f);
                matRotX.m[1][2] = sinf(theta * 0.5f);
                matRotX.m[2][1] = -sinf(theta * 0.5f);
                matRotX.m[2][2] = cosf(theta * 0.5f);
                matRotX.m[3][3] = 1;

                for (int i = 0; i < mesh->count; i++) {
                        struct triangle rotatedz;
                        rotatedz.p[0] = Mat4x4MultiplyVec3d(&matRotZ, &mesh->tris[i].p[0]);
                        rotatedz.p[1] = Mat4x4MultiplyVec3d(&matRotZ, &mesh->tris[i].p[1]);
                        rotatedz.p[2] = Mat4x4MultiplyVec3d(&matRotZ, &mesh->tris[i].p[2]);

                        struct triangle rotatedzx = rotatedz;
                        rotatedzx.p[0] = Mat4x4MultiplyVec3d(&matRotX, &rotatedz.p[0]);
                        rotatedzx.p[1] = Mat4x4MultiplyVec3d(&matRotX, &rotatedz.p[1]);
                        rotatedzx.p[2] = Mat4x4MultiplyVec3d(&matRotX, &rotatedz.p[2]);

                        struct triangle translated = rotatedzx;
                        translated.p[0].z = mesh->tris[i].p[0].z + 3.0f;
                        translated.p[1].z = mesh->tris[i].p[1].z + 3.0f;
                        translated.p[2].z = mesh->tris[i].p[2].z + 3.0f;

                        struct triangle projected;
                        projected.p[0] = Mat4x4MultiplyVec3d(&matProj, &translated.p[0]);
                        projected.p[1] = Mat4x4MultiplyVec3d(&matProj, &translated.p[1]);
                        projected.p[2] = Mat4x4MultiplyVec3d(&matProj, &translated.p[2]);

                        // Scale into view
                        projected.p[0].x += 1.0f; projected.p[0].y += 1.0f;
                        projected.p[1].x += 1.0f; projected.p[1].y += 1.0f;
                        projected.p[2].x += 1.0f; projected.p[2].y += 1.0f;

                        projected.p[0].x *= 0.5f * (float)SCREEN_WIDTH;
                        projected.p[0].y *= 0.5f * (float)SCREEN_HEIGHT;
                        projected.p[1].x *= 0.5f * (float)SCREEN_WIDTH;
                        projected.p[1].y *= 0.5f * (float)SCREEN_HEIGHT;
                        projected.p[2].x *= 0.5f * (float)SCREEN_WIDTH;
                        projected.p[2].y *= 0.5f * (float)SCREEN_HEIGHT;

                        GraphicsDrawTriangle(
                                graphics,
                                projected.p[0].x, projected.p[0].y,
                                projected.p[1].x, projected.p[1].y,
                                projected.p[2].x, projected.p[2].y,
                                0xFFFFFFFF
                                );
                }

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

        MeshDeinit(mesh);
        GraphicsDeinit(graphics);

        return 0;
}
