/******************************************************************************
  File: input.c
  Created: 2019-06-21
  Updated: 2019-08-07
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/
#include "SDL2/SDL.h"
//! \file input.c

//! \brief Keypress state. Unexported.
struct input {
        int dummyValue;
};

struct input *InputInit() {
        struct input *i = (struct input *)malloc(sizeof(struct input));
        memset(i, 0, sizeof(struct input));

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
                        // HandleKeyUp(i, s, event->key.keysym.sym);
                        break;

                case SDL_KEYDOWN:
                        if (event->key.keysym.sym == SDLK_ESCAPE) {
                                return 1;
                                break;
                        }
                        // HandleKeyDown(i, s, event->key.keysym.sym);
                        break;
        }

        return 0;
}
