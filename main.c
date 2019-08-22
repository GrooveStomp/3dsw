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
#include "texture.h"

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

struct graphics *graphics;
struct input *input;
struct texture *texture;
struct mesh *mesh;

void Shutdown(int code) {
        if (NULL != texture)
                TextureDeinit(texture);

        if (NULL != mesh)
                MeshDeinit(mesh);

        if (NULL != input)
                InputDeinit(input);

        if (NULL != graphics)
                GraphicsDeinit(graphics);

        exit(code);
}

int main(int argc, char **argv) {
        struct timespec progStart;
        clock_gettime(CLOCK_REALTIME, &progStart);

        graphics = GraphicsInit("3D Software Demo", SCREEN_WIDTH, SCREEN_HEIGHT, 1);
        if (NULL == graphics) {
                fprintf(stderr, "Couldn't initialize graphics");
                Shutdown(1);
        }

        input = InputInit();
        if (NULL == input) {
                fprintf(stderr, "Couldn't initialize input");
                Shutdown(1);
        }

        texture = TextureInitFromFile("debug_texture.png");
        if (NULL == texture) {
                fprintf(stderr, "Couldn't initialize texture");
                Shutdown(1);
        }

        mesh = MeshInit(12);
        if (NULL == mesh) {
                fprintf(stderr, "Couldn't initialize mesh");
                Shutdown(1);
        }

        // South
        mesh->tris[0] = TriangleInit(0,0,0, 0,1, 0,1,0, 0,0, 1,1,0, 1,0);
        mesh->tris[1] = TriangleInit(0,0,0, 0,1, 1,1,0, 1,0, 1,0,0, 1,1);

        // East
        mesh->tris[2] = TriangleInit(1,0,0, 0,1, 1,1,0, 0,0, 1,1,1, 1,0);
        mesh->tris[3] = TriangleInit(1,0,0, 0,1, 1,1,1, 1,0, 1,0,1, 1,1);

        // North
        mesh->tris[4] = TriangleInit(1,0,1, 0,1, 1,1,1, 0,0, 0,1,1, 1,0);
        mesh->tris[5] = TriangleInit(1,0,1, 0,1, 0,1,1, 1,0, 0,0,1, 1,1);

        // West
        mesh->tris[6] = TriangleInit(0,0,1, 0,1, 0,1,1, 0,0, 0,1,0, 1,0);
        mesh->tris[7] = TriangleInit(0,0,1, 0,1, 0,1,0, 1,0, 0,0,0, 1,1);

        // Top
        mesh->tris[8] = TriangleInit(0,1,0, 0,1, 0,1,1, 0,0, 1,1,1, 1,0);
        mesh->tris[9] = TriangleInit(0,1,0, 0,1, 1,1,1, 1,0, 1,1,0, 1,1);

        // Bottom
        mesh->tris[10] = TriangleInit(1,0,1, 0,1, 0,0,1, 0,0, 0,0,0, 1,0);
        mesh->tris[11] = TriangleInit(1,0,1, 0,1, 0,0,0, 1,0, 1,0,0, 1,1);

        /* struct mesh *mesh = MeshInitFromObj("axis-flipped-x.obj"); */
        /* if (NULL == mesh) { */
        /*         fprintf(stderr, "There was a problem initializing the mesh"); */
        /* } */

        camera = (struct vec3){ 0 };
        yaw = 0;
        elapsedTime = 0;

        struct mat4x4 matProj = Mat4x4Project(90.0f, (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH, 0.1f, 1000.0f);

        struct triangle *renderTris = malloc(sizeof(struct triangle) * mesh->count * 2);
        int renderTrisCount = 0;

        double count = 0.0;
        SDL_Event event;
        int running = 1;

        while (running) {
                struct timespec start;
                clock_gettime(CLOCK_REALTIME, &start);

                double theta = 1.0f; //1.0f + count;
                count += 0.01;

                struct mat4x4 matRotZ = Mat4x4RotateZ(0); //theta * 0.5f);
                struct mat4x4 matRotX = Mat4x4RotateX(0); //theta);
                struct mat4x4 matRotY = Mat4x4RotateY(theta);
                struct mat4x4 matTrans = Mat4x4Translate(0.0f, -0.3f, 1.0f);

                struct mat4x4 matWorld = Mat4x4Identity();
                matWorld = Mat4x4Multiply(matRotZ, matRotX);
                matWorld = Mat4x4Multiply(matWorld, matRotY);
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
                for (int i = 0; i < mesh->count; i++) {
                        // World matrix transform.
                        struct triangle transformed = mesh->tris[i];
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
                                struct triangle viewed = transformed;
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

                                        // Project texture coords
                                        projected.u1 = projected.u1 / projected.w1;
                                        projected.u2 = projected.u2 / projected.w2;
                                        projected.u3 = projected.u3 / projected.w3;

                                        projected.v1 = projected.v1 / projected.w1;
                                        projected.v2 = projected.v2 / projected.w2;
                                        projected.v3 = projected.v3 / projected.w3;

                                        projected.tw1 = 1.0f / projected.w1;
                                        projected.tw2 = 1.0f / projected.w2;
                                        projected.tw3 = 1.0f / projected.w3;

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

                                GraphicsTriangleTextured(graphics, t, texture);
                                // Draw solid faces.
                                // GraphicsTriangleSolid(graphics, t, t.color);
                                // Draw wireframe faces.
                                // GraphicsTriangleWireframe(graphics, t, ColorWhite.rgba);
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
        Shutdown(0);

        return 0;
}
