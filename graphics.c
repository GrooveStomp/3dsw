/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: graphics.c
  Created: 2019-06-25
  Updated: 2019-08-26
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file graphics.c

#include <string.h> // memset
#include <stdio.h> // fprintf

#include "SDL2/SDL.h"

#include "math.h"
#include "graphics.h"
#include "texture.h"
#include "color.h"

//! \brief A mostly generic implementation of swap
//!
//! Both v1 and v2 must point to data that is the same size, as specified in the
//! size parameter.
//!
//! \param[in,out] v1 first value
//! \param[in,out] v2 second value
//! \param[in] size v1 and v2 must each be this size
//!
void swap_generic(void *v1, void *v2, size_t size) {
        char temp[size];
        memmove(temp, v1, size);
        memmove(v1, v2, size);
        memmove(v2, temp, size);
}

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
        SDL_LockTexture(graphics->texture, NULL, (void **)&graphics->pixels, &graphics->bytesPerRow);
}

void GraphicsEnd(struct graphics *graphics) {
        SDL_UnlockTexture(graphics->texture);
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

//! \brief Scale the pixel being drawn
//!
//! This renders the given pixel, scaled as per graphics->scale.
//!
//! When the graphics state is initialized, a width and height are specified.
//!
//! Internally the graphics state multiplies both of these values by the scale
//! and stores a buffer of the resulting size.  This function allows us to
//! treat the resulting scaled buffer as if it were the original size
//! requested.
//!
//! \param[in,out] graphics Graphics state to be manipulated
//! \param[in] x horizontal position in display buffer (assuming no scaling)
//! \param[in] y vertical position in display buffer (assuming no scaling)
//! \param[in] color Color to put into display buffer
void PutPixelScaled(struct graphics *graphics, int x, int y, unsigned int color) {
        for (int sy = y; sy < y + graphics->scale; sy++) {
                int yFlipped = graphics->height - sy - 1;
                for (int sx = x; sx < x + graphics->scale; sx++) {
                        unsigned int *pixel = (unsigned int *)&graphics->pixels[yFlipped * graphics->bytesPerRow + x * 4];
                        *pixel = color;
                }
        }
}

//! \brief Put a pixel into the graphics buffer
//!
//! \param[in,out] graphics Graphics state to be manipulated
//! \param[in] x horizontal position in display buffer (assuming no scaling)
//! \param[in] y vertical position in display buffer (assuming no scaling)
//! \param[in] color Color to put into display buffer
void PutPixel(struct graphics *graphics, int x, int y, unsigned int color) {
        if (x >= 0 && x < graphics->width && y >= 0 && y < graphics->height) {
                /* int y2 = graphics->height - y - 1; */
                /* unsigned int *pixel = (unsigned int *)&graphics->pixels[y2 * graphics->bytesPerRow + x * 4]; */
                /* *pixel = color; */
                PutPixelScaled(graphics, x, y, color);
        }
}

//! \brief Draws a line from (x1,y1) to (x2,y2)
//!
//! Used by GraphicsTriangleWireframe() and GraphicsTriangleSolid()
//!
//! \param[in,out] graphics Graphics state to be manipulated
//! \param[in] x1 horizontal position of the line start.
//! \param[in] y1 vertical position of the line start.
//! \param[in] x2 horizontal position of the line end.
//! \param[in] y2 vertical position of the line end.
//! \param[in] color color to render the line with.
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

void GraphicsTriangleTextured(struct graphics *graphics, struct triangle tri, struct texture *texture) {
        int x1 = tri.x1; int y1 = tri.y1; float u1 = tri.u1; float v1 = tri.v1; float w1 = tri.tw1;
        int x2 = tri.x2; int y2 = tri.y2; float u2 = tri.u2; float v2 = tri.v2; float w2 = tri.tw2;
        int x3 = tri.x3; int y3 = tri.y3; float u3 = tri.u3; float v3 = tri.v3; float w3 = tri.tw3;

        // Sort all vertices by y-value.
        if (y2 < y1) {
                swap_generic(&x1, &x2, sizeof(int));
                swap_generic(&y1, &y2, sizeof(int));
                swap_generic(&u1, &u2, sizeof(float));
                swap_generic(&v1, &v2, sizeof(float));
                swap_generic(&w1, &w2, sizeof(float));
        }

        if (y3 < y1) {
                swap_generic(&x1, &x3, sizeof(int));
                swap_generic(&y1, &y3, sizeof(int));
                swap_generic(&u1, &u3, sizeof(float));
                swap_generic(&v1, &v3, sizeof(float));
                swap_generic(&w1, &w3, sizeof(float));
        }

        if (y3 < y2) {
                swap_generic(&x2, &x3, sizeof(int));
                swap_generic(&y2, &y3, sizeof(int));
                swap_generic(&u2, &u3, sizeof(float));
                swap_generic(&v2, &v3, sizeof(float));
                swap_generic(&w2, &w3, sizeof(float));
        }

        // Calculate the coordinate deltas.
        float dx1 = x2 - x1;
        float dy1 = y2 - y1;
        float du1 = u2 - u1;
        float dv1 = v2 - v1;
        float dw1 = w2 - w1;

        float dx2 = x3 - x1;
        float dy2 = y3 - y1;
        float du2 = u3 - u1;
        float dv2 = v3 - v1;
        float dw2 = w3 - w1;

        float dx1Step = 0, dx2Step = 0,
              du1Step = 0, du2Step = 0,
              dv1Step = 0, dv2Step = 0,
              dw1Step = 0, dw2Step = 0;

        // Calculate the vertex and texture line slopes.
        if (dy1) dx1Step = dx1 / fabs(dy1);
        if (dy1) du1Step = du1 / fabs(dy1);
        if (dy1) dv1Step = dv1 / fabs(dy1);
        if (dy1) dw1Step = dw1 / fabs(dy1);

        if (dy2) dx2Step = dx2 / fabs(dy2);
        if (dy2) du2Step = du2 / fabs(dy2);
        if (dy2) dv2Step = dv2 / fabs(dy2);
        if (dy2) dw2Step = dw2 / fabs(dy2);

        // Rasterize the top half of the triangle.
        if (dy1) {
                for (int i = y1; i <= y2; i++) {
                        float delta = (float)(i - y1);
                        int ax = x1 + delta * dx1Step;
                        int bx = x1 + delta * dx2Step;

                        float su = u1 + delta * du1Step;
                        float sv = v1 + delta * dv1Step;
                        float sw = w1 + delta * dw1Step;

                        float eu = u1 + delta * du2Step;
                        float ev = v1 + delta * dv2Step;
                        float ew = w1 + delta * dw2Step;

                        // Swap coordinates so the lower x value comes first.
                        if (ax > bx) {
                                swap_generic(&ax, &bx, sizeof(int));
                                swap_generic(&su, &eu, sizeof(float));
                                swap_generic(&sv, &ev, sizeof(float));
                                swap_generic(&sw, &ew, sizeof(float));
                        }

                        float tStep = 1.0f / ((float)(bx - ax));
                        float t = 0.0f;

                        for (int j = ax; j < bx; j++) {
                                float u = (1.0f - t) * su + t * eu;
                                float v = (1.0f - t) * sv + t * ev;
                                float w = (1.0f - t) * sw + t * ew;
                                PutPixel(graphics, j, i, TextureSample(texture, u / w, v / w));
                                t += tStep;
                        }
                }
        }

        // We're now at the halfway line of the triangle.
        // Recalculate the new slope.

        dx1 = x3 - x2;
        dy1 = y3 - y2;
        du1 = u3 - u2;
        dv1 = v3 - v2;
        dw1 = w3 - w2;

        du1Step = 0;
        dv1Step = 0;
        dw1Step = 0;

        // Calculate the vertex and texture line slopes.
        if (dy1) dx1Step = dx1 / fabs(dy1);
        if (dy2) dx2Step = dx2 / fabs(dy2);
        if (dy1) du1Step = du1 / fabs(dy1);
        if (dy1) dv1Step = dv1 / fabs(dy1);
        if (dy1) dw1Step = dw1 / fabs(dy1);

        // Rasterize the lower half of the triangle.
        if (dy1) {
                for (int i = y2; i <= y3; i++) {
                        float y1Delta = (float)(i - y1);
                        float y2Delta = (float)(i - y2);

                        int ax = x2 + y2Delta * dx1Step;
                        int bx = x1 + y1Delta * dx2Step;

                        float su = u2 + y2Delta * du1Step;
                        float sv = v2 + y2Delta * dv1Step;
                        float sw = w2 + y2Delta * dw1Step;

                        float eu = u1 + y1Delta * du2Step;
                        float ev = v1 + y1Delta * dv2Step;
                        float ew = w1 + y1Delta * dw2Step;

                        // Swap coordinates so the lower x value comes first.
                        if (ax > bx) {
                                swap_generic(&ax, &bx, sizeof(int));
                                swap_generic(&su, &eu, sizeof(float));
                                swap_generic(&sv, &ev, sizeof(float));
                                swap_generic(&sw, &ew, sizeof(float));
                        }

                        float tStep = 1.0f / ((float)(bx - ax));
                        float t = 0.0f;

                        for (int j = ax; j < bx; j++) {
                                float u = (1.0f - t) * su + t * eu;
                                float v = (1.0f - t) * sv + t * ev;
                                float w = (1.0f - t) * sw + t * ew;
                                PutPixel(graphics, j, i, TextureSample(texture, u / w, v / w));
                                t += tStep;
                        }
                }
        }
}

void GraphicsTriangleWireframe(struct graphics *graphics, struct triangle triangle, unsigned int color) {
        GraphicsDrawLine(graphics, triangle.x1, triangle.y1, triangle.x2, triangle.y2, color);
        GraphicsDrawLine(graphics, triangle.x2, triangle.y2, triangle.x3, triangle.y3, color);
        GraphicsDrawLine(graphics, triangle.x3, triangle.y3, triangle.x1, triangle.y1, color);
}

//! Used internally by GraphicsTriangleSolid()
void TriangleSolidDrawLine(struct graphics *graphics, int xmin, int xmax, int y, unsigned int color) {
        for (int i = xmin; i <= xmax; i++) {
                PutPixel(graphics, i, y, color);
        }
}

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
	if (y1>y2) { swap_generic(&y1,&y2,sizeof(int)); swap_generic(&x1,&x2,sizeof(int)); }
	if (y1>y3) { swap_generic(&y1,&y3,sizeof(int)); swap_generic(&x1,&x3,sizeof(int)); }
	if (y2>y3) { swap_generic(&y2,&y3,sizeof(int)); swap_generic(&x2,&x3,sizeof(int)); }

	t1x=t2x=x1; y=y1;   // Starting points

	dx1 = (int)(x2 - x1); if (dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1); if (dx2<0) { dx2=-dx2; signx2=-1; } else signx2=1;
	dy2 = (int)(y3 - y1);

	if (dy1 > dx1) {   // swap values
                swap_generic(&dx1,&dy1,sizeof(int));
		changed1 = 1;
	}
	if (dy2 > dx2) {   // swap values
                swap_generic(&dy2,&dx2,sizeof(int));
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
                swap_generic(&dy1, &dx1, sizeof(dy1));
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
