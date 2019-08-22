/******************************************************************************
  File: texture.c
  Created: 2019-08-18
  Updated: 2019-08-18
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/

//! \file texture.h

#include <stdlib.h> // malloc, sizeof
#include <string.h> // memset

#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "external/stb_image.h"

struct texture *TextureInitFromFile(char *file) {
        struct texture *t = (struct texture *)malloc(sizeof(struct texture));
        memset(t, 0, sizeof(struct texture));

        t->data = stbi_load(file, &t->width, &t->height, &t->numBytesPerPixel, 4);
        if (NULL == t->data) {
                return NULL;
        }

        return t;
}

void TextureDeinit(struct texture *t) {
        if (NULL == t)
                return;

        if (NULL != t->data) {
                stbi_image_free(t->data);
        }

        free(t);
}

unsigned int TextureSample(struct texture *t, float u, float v) {
        // Drop non-fractional parts.
        //printf("u: %f, v: %f\n", u, v);
        if (u > 1.0f || u < 0.0f)
                u = u - (long)u;
        if (v > 1.0f || v < 0.0f)
                v = v - (long)v;
        //printf("u: %f, v: %f\n", u, v);

        int y = (int)(v * (float)t->height - 1);
        int x = (int)(u * (float)t->width);

        if (x < 0 || x >= t->width || y < 0 || y >= t->height) {
                return 0x000000FF;
        }

        unsigned char *pixelBase = &t->data[y * t->width * 4 + (x * 4)];

        unsigned char r = pixelBase[0];
        unsigned char g = pixelBase[1];
        unsigned char b = pixelBase[2];

        unsigned int result =
                r << (8 * 3) |
                g << (8 * 2) |
                b << (8 * 1) |
                0xFF;

        return result;
}