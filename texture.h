/******************************************************************************
  File: texture.h
  Created: 2019-08-18
  Updated: 2019-08-18
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/

//! \file texture.h

//! data is always RGB with 8-bits per component and no padding in rows.
struct texture {
        unsigned char *data;
        int width;
        int height;
        int numBytesPerPixel;
};

struct texture *
TextureInitFromFile(char *file);

void
TextureDeinit(struct texture *texture);

unsigned int
TextureSample(struct texture *texture, float u, float v);
