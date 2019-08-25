/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: texture.h
  Created: 2019-08-18
  Updated: 2019-08-25
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
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
