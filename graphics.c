/******************************************************************************
  File: graphics.c
  Created: 2019-06-25
  Updated: 2019-08-13
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/

//! \file graphics.c

#include <string.h> // memset
#include <stdio.h> // fprintf

#include "SDL2/SDL.h"

#include "math.h"
#include "graphics.h"

//! \brief Graphics state
struct graphics {
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
        unsigned int width;
        unsigned int height;

        unsigned char *pixels;
        int bytesPerRow;
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

void GraphicsBegin(struct graphics *graphics) {
        // int isLocked
        SDL_LockTexture(graphics->texture, NULL, (void **)&graphics->pixels, &graphics->bytesPerRow);
        // if (0 != isLocked) {
        //        fprintf(stderr, "Couldn't lock texture: %s\n", SDL_GetError());
        //        return;
        // }
}

void GraphicsEnd(struct graphics *graphics) {
        // int isUnlocked
        SDL_UnlockTexture(graphics->texture);
        // if (0 != isUnlocked) {
        //        fprintf(stderr, "Couldn't unlock texture: %s\n", SDL_GetError());
        //        return;
        // }
        SDL_RenderClear(graphics->renderer);
        SDL_RenderCopy(graphics->renderer, graphics->texture, 0, 0);
        SDL_RenderPresent(graphics->renderer);
}

void GraphicsClearScreen(struct graphics *graphics, unsigned int color) {
        for (int i = 0; i < graphics->bytesPerRow * graphics->height; i+=4) {
                unsigned int *pixel = (unsigned int *)&graphics->pixels[i];
                *pixel = color;
        }
}

int DrawRol(unsigned int pattern) {
        pattern = (pattern << 1) | (pattern >> 31);
        return pattern & 1;
}

void DrawPixel(struct graphics *graphics, int x, int y, unsigned int color) {
        if (x >= 0 && x < graphics->width && y >= 0 && y < graphics->height) {
                unsigned int *pixel = (unsigned int *)&graphics->pixels[y * graphics->bytesPerRow + x * 4];
                *pixel = color;
        }
}

void GraphicsDrawLine(struct graphics *graphics, int x1, int y1, int x2, int y2, unsigned int color) {
        int dx = x2 - x1;
        int dy = y2 - y1;

        int dx1 = abs(dx);
        int dy1 = abs(dy);

        int px = 2 * dy1 - dx1;
        int py = 2 * dx1 - dy1;

        int x, y, xe, ye;

        if (dy1 <= dx1) {
                // Line is horizontal.
                if (dx >= 0) {
                        x = x1;
                        y = y1;
                        xe = x2;
                } else {
                        x = x2;
                        y = y2;
                        xe = x1;
                }

                DrawPixel(graphics, x, y, color);

                for (int i = 0; x < xe; i++) {
                        x = x + 1;
                        if (px < 0) {
                                px = px + 2 * dy1;
                        } else {
                                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                                        y = y + 1;
                                } else {
                                        y = y - 1;
                                }
                                px = px + 2 * (dy1 - dx1);
                        }
                        DrawPixel(graphics, x, y, color);
                }
        } else {
                // Line is vertical.
                if (dy >= 0) {
                        x = x1;
                        y = y1;
                        ye = y2;
                } else {
                        x = x2;
                        y = y2;
                        ye = y1;
                }

                DrawPixel(graphics, x, y, color);

                for (int i = 0; y < ye; i++) {
                        y = y + 1;
                        if (py <= 0) {
                                py = py + 2 * dx1;
                        } else {
                                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                                        x = x + 1;
                                } else {
                                        x = x - 1;
                                }
                                py = py + 2 * (dx1 - dy1);
                        }
                        DrawPixel(graphics, x, y, color);
                }
        }
}

void GraphicsDrawTriangle(struct graphics *graphics, struct triangle triangle, unsigned int color) {
        GraphicsDrawLine(graphics, triangle.x1, triangle.y1, triangle.x2, triangle.y2, color);
        GraphicsDrawLine(graphics, triangle.x2, triangle.y2, triangle.x3, triangle.y3, color);
        GraphicsDrawLine(graphics, triangle.x3, triangle.y3, triangle.x1, triangle.y1, color);
}
