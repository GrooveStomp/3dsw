/******************************************************************************
  File: graphics.c
  Created: 2019-06-25
  Updated: 2019-07-31
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/

//! \file graphics.c

#include <string.h> // memset
#include <stdio.h> // fprintf

#include "SDL2/SDL.h"

#include "graphics.h"

//! \brief Graphics state
struct graphics {
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
        unsigned int width;
        unsigned int height;
};

struct graphics *GraphicsInit(char *title, int width, int height) {
        struct graphics *g = (struct graphics *)malloc(sizeof(struct graphics));
        memset(g, 0, sizeof(struct graphics));

        g->width = width;
        g->height = height;

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);

        g->window = SDL_CreateWindow(
                title,
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                g->width, g->height,
                SDL_WINDOW_SHOWN
        );
        if (NULL == g->window) {
                fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
                GraphicsDeinit(g);
                return NULL;
        }

        g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_SOFTWARE);
        if (NULL == g->renderer) {
                fprintf(stderr, "Couldn't create renderer: %s\n", SDL_GetError());
                GraphicsDeinit(g);
                return NULL;
        }

        g->texture = SDL_CreateTexture(g->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (NULL == g->texture) {
                fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
                GraphicsDeinit(g);
                return NULL;
        }

        return g;
}

void GraphicsDeinit(struct graphics *g) {
        if (NULL == g) {
                return;
        }

        if (NULL != g->texture) {
                SDL_DestroyTexture(g->texture);
        }

        if (NULL != g->renderer) {
                SDL_DestroyRenderer(g->renderer);
        }

        if (NULL != g->window) {
                SDL_DestroyWindow(g->window);
        }

        SDL_Quit();
        free(g);
}

/*
  TODO: AARONO
  To render:
  SDL_LockTexture(graphics->texture, ...)
  {
    .. do stuff ..
  }
  SDL_UnlockTexture(graphics->texture);

  Not sure if I should have an API that takes a function pointer, or thinly wrap
  lock/unlock, or implicitly call lock and return the buffer, requiring explict
  unlock afterward...
 */

void GraphicsClearScreen(struct graphics *graphics, unsigned int color) {
        unsigned char *pixels;
        int bytesPerRow;
        int isLocked = SDL_LockTexture(graphics->texture, NULL, (void **)&pixels, &bytesPerRow);
        if (0 != isLocked) {
                fprintf(stderr, "Couldn't lock texture: %s\n", SDL_GetError());
                return;
        }

        for (int i = 0; i < bytesPerRow; i+=4) {
                unsigned int *pixel = (unsigned int *)&pixels[i];
                *pixel = color;
        }

        SDL_UnlockTexture(graphics->texture);
}

void GraphicsDrawTriangle(struct graphics *graphics, float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color) {
        unsigned char *pixels;
        int bytesPerRow;
        int isLocked = SDL_LockTexture(graphics->texture, NULL, (void **)&pixels, &bytesPerRow);
        if (0 != isLocked) {
                fprintf(stderr, "Couldn't lock texture: %s\n", SDL_GetError());
                return;
        }

        DrawLine(graphics, x1, y1, x2, y2, color);
        DrawLine(graphics, x2, y2, x3, y3, color);
        DrawLine(graphics, x3, y3, x1, y1, color);

        SDL_UnlockTexture(graphics->texture);
}

void DrawLine(struct graphics *graphics, float x1, float y1, float x2, float y2, unsigned int color) {
        // TODO: Implement me!
        // See: https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/olcPixelGameEngine.h#L1240
}
