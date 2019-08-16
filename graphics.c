/******************************************************************************
  File: graphics.c
  Created: 2019-06-25
  Updated: 2019-08-16
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
        unsigned int scale;

        unsigned char *pixels;
        int bytesPerRow;
};

struct graphics *GraphicsInit(char *title, int width, int height, int scale) {
        struct graphics *g = (struct graphics *)malloc(sizeof(struct graphics));
        memset(g, 0, sizeof(struct graphics));

        g->width = width;
        g->height = height;
        g->scale = scale;

        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);

        g->window = SDL_CreateWindow(
                title,
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                g->width * g->scale, g->height * g->scale,
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

void PutPixelScaled(struct graphics *graphics, int x, int y, unsigned int color) {
        for (int sy = y; sy < y + graphics->scale; sy++) {
                for (int sx = x; sx < x + graphics->scale; sx++) {
                        unsigned int *pixel = (unsigned int *)&graphics->pixels[y * graphics->bytesPerRow + x * 4];
                        *pixel = color;
                }
        }
}

void PutPixel(struct graphics *graphics, int x, int y, unsigned int color) {
        if (x >= 0 && x < graphics->width && y >= 0 && y < graphics->height) {
                PutPixelScaled(graphics, x, y, color);
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

                PutPixel(graphics, x, y, color);

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
                        PutPixel(graphics, x, y, color);
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

                PutPixel(graphics, x, y, color);

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
                        PutPixel(graphics, x, y, color);
                }
        }
}

void GraphicsTriangleWireframe(struct graphics *graphics, struct triangle triangle, unsigned int color) {
        GraphicsDrawLine(graphics, triangle.x1, triangle.y1, triangle.x2, triangle.y2, color);
        GraphicsDrawLine(graphics, triangle.x2, triangle.y2, triangle.x3, triangle.y3, color);
        GraphicsDrawLine(graphics, triangle.x3, triangle.y3, triangle.x1, triangle.y1, color);
}

void TriangleSolidDrawLine(struct graphics *graphics, int xmin, int xmax, int y, unsigned int color) {
        for (int i = xmin; i <= xmax; i++) {
                PutPixel(graphics, i, y, color);
        }
}

// Source: https://stackoverflow.com/a/3982430
/* #define SWAP(x,y) do \ */
/*         { \ */
/*                 unsigned char swap_temp[sizeof(x) == sizeof(y) ? (signed)sizeof(x) : -1]; \ */
/*                 memcpy(swap_temp,&y,sizeof(x));                         \ */
/*                 memcpy(&y,&x,       sizeof(x));                         \ */
/*                 memcpy(&x,swap_temp,sizeof(x));                         \ */
/*         } while(0) */

#define SWAP(x,y) { int t = (x); (x) = (y); (y) = t; }

void GraphicsTriangleSolid(struct graphics *graphics, struct triangle triangle, unsigned int color) {
        GraphicsDrawLine(graphics, triangle.x1, triangle.y1, triangle.x2, triangle.y2, color);
        GraphicsDrawLine(graphics, triangle.x2, triangle.y2, triangle.x3, triangle.y3, color);
        GraphicsDrawLine(graphics, triangle.x3, triangle.y3, triangle.x1, triangle.y1, color);

        int x1 = triangle.v[0].x;
        int y1 = triangle.v[0].y;
        int x2 = triangle.v[1].x;
        int y2 = triangle.v[1].y;
        int x3 = triangle.v[2].x;
        int y3 = triangle.v[2].y;

	unsigned int t1x,t2x,y,minx,maxx,t1xp,t2xp;
	int changed1 = 0;
	int changed2 = 0;
	int signx1,signx2,dx1,dy1,dx2,dy2;
	unsigned int e1,e2;
        // Sort vertices
	if (y1>y2) { SWAP(y1,y2); SWAP(x1,x2); }
	if (y1>y3) { SWAP(y1,y3); SWAP(x1,x3); }
	if (y2>y3) { SWAP(y2,y3); SWAP(x2,x3); }

	t1x=t2x=x1; y=y1;   // Starting points

	dx1 = (int)(x2 - x1); if (dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2<0) { dx2=-dx2; signx2=-1; } else signx2=1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) {   // swap values
                SWAP(dx1,dy1);
		changed1 = 1;
	}
	if (dy2 > dx2) {   // swap values
                SWAP(dy2,dx2);
		changed2 = 1;
	}

	e2 = (unsigned int)(dx2>>1);
        // Flat top, just process the second half
        if (y1==y2) goto next;
        e1 = (unsigned int)(dx1>>1);

	for (unsigned int i = 0; i < dx1;) {
		t1xp=0; t2xp=0;
		if (t1x<t2x) { minx=t1x; maxx=t2x; }
		else		{ minx=t2x; maxx=t1x; }
                // process first line until y value is about to change
		while(i<dx1) {
			i++;
			e1 += dy1;
	   	   	while (e1 >= dx1) {
				e1 -= dx1;
                                if (changed1) t1xp=signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
                // Move line
	next1:
                // process second line until y value is about to change
		while (1) {
			e2 += dy2;
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp=signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if (minx>t1x) minx=t1x;
                if (minx>t2x) minx=t2x;
		if (maxx<t1x) maxx=t1x;
                if (maxx<t2x) maxx=t2x;
                // Draw line from min to max points found on the y.
                TriangleSolidDrawLine(graphics, minx, maxx, y, color);

		// Now increase y
		if (!changed1)
                        t1x += signx1;
		t1x+=t1xp;
		if (!changed2)
                        t2x += signx2;
		t2x+=t2xp;
                y += 1;
		if (y==y2) break;

        }
next:
	// Second half
	dx1 = (int)(x3 - x2);
        if (dx1<0) {
                dx1=-dx1;
                signx1=-1;
        } else {
                signx1=1;
        }
	dy1 = (int)(y3 - y2);
	t1x=x2;

	if (dy1 > dx1) {   // swap values
                SWAP(dy1,dx1);
		changed1 = 1;
	} else {
                changed1=0;
        }

	e1 = (unsigned int)(dx1 >> 1);

	for (unsigned int i = 0; i<=dx1; i++) {
		t1xp=0; t2xp=0;
		if (t1x<t2x) {
                        minx=t1x;
                        maxx=t2x;
                } else {
                        minx=t2x;
                        maxx=t1x;
                }

                // process first line until y value is about to change
		while(i<dx1) {
                        e1 += dy1;
	   	   	while (e1 >= dx1) {
				e1 -= dx1;
   	   	   	   	if (changed1) {
                                        t1xp=signx1;
                                        break;
                                }//t1x += signx1;
				else
                                        goto next3;
			}
			if (changed1)
                                break;
			else
                                t1x += signx1;
			if (i<dx1) i++;
		}
	next3:
                // process second line until y value is about to change
		while (t2x!=x3) {
			e2 += dy2;
	   	   	while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2)
                                        t2xp=signx2;
				else
                                        goto next4;
			}
			if (changed2)
                                break;
			else
                                t2x += signx2;
		}
	next4:

		if (minx>t1x) minx=t1x;
                if (minx>t2x) minx=t2x;
		if (maxx<t1x) maxx=t1x;
                if (maxx<t2x) maxx=t2x;
	   	// Draw line from min to max points found on the y
                TriangleSolidDrawLine(graphics, minx, maxx, y, color);

		// Now increase y
		if (!changed1) t1x += signx1;
		t1x+=t1xp;
		if (!changed2) t2x += signx2;
		t2x+=t2xp;
                y += 1;

		if (y>y3)
                        return;
	}
}
