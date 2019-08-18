/******************************************************************************
  File: input.c
  Created: 2019-06-21
  Updated: 2019-08-17
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/
#include "SDL2/SDL.h"
//! \file input.c

#include "math.h"

extern struct vec3 camera;
extern struct vec3 lookDir;
extern struct vec3 up;
extern float yaw;
extern double elapsedTime;

//! \brief Keypress state. Unexported.
struct input {
        int dummyValue;
        const unsigned char *sdlKeyStates;
};

struct input *InputInit() {
        struct input *i = (struct input *)malloc(sizeof(struct input));
        memset(i, 0, sizeof(struct input));

        i->sdlKeyStates = SDL_GetKeyboardState(NULL);

        return i;
}

void InputDeinit(struct input *i) {
        if (NULL == i)
                return;

        free(i);
}

int InputIsQuitPressed(struct input *i, SDL_Event *event) {
        switch (event->type) {
                case SDL_QUIT:
                        return 1;
                        break;

                case SDL_KEYUP:
                        break;

                case SDL_KEYDOWN:
                        if (event->key.keysym.sym == SDLK_ESCAPE) {
                                return 1;
                                break;
                        }
                        break;
        }

        return 0;
}

void InputProcess(struct input *i) {
        double t = elapsedTime / 1000.0f;
        struct vec3 forward = Vec3Multiply(lookDir, 8.0f * t);
        struct vec3 right = Vec3CrossProduct(up, lookDir);
        right = Vec3Multiply(Vec3Normalize(right), 8.0f * t);

        if (i->sdlKeyStates[SDL_SCANCODE_LEFT]) {
                yaw += 2.0f * t;
        }

        if (i->sdlKeyStates[SDL_SCANCODE_RIGHT]) {
                yaw -= 2.0f * t;
        }

        if (i->sdlKeyStates[SDL_SCANCODE_UP]) {
                camera.y += 8.0f * t;
        }

        if (i->sdlKeyStates[SDL_SCANCODE_DOWN]) {
                camera.y -= 8.0f * t;
        }

        if (i->sdlKeyStates[SDL_SCANCODE_W]) {
                camera = Vec3Add(camera, forward);
        }

        if (i->sdlKeyStates[SDL_SCANCODE_S]) {
                camera = Vec3Subtract(camera, forward);
        }

        if (i->sdlKeyStates[SDL_SCANCODE_A]) {
                camera = Vec3Subtract(camera, right);
        }

        if (i->sdlKeyStates[SDL_SCANCODE_D]) {
                camera = Vec3Add(camera, right);
        }
}
