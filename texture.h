/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: texture.h
  Created: 2019-08-18
  Updated: 2019-08-27
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file texture.h

#ifndef TEXTURE_VERSION
#define TEXTURE_VERSION "0.1.0" //!< include guard

//! Structure representing a texture image
struct texture {
        unsigned char *data; //!< data is always RGB with 8-bits per component and no padding in rows.
        int width;
        int height;
        int numBytesPerPixel;
};

//! \brief Initialize a new texture object
//!
//! This uses [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h), and thus all the image formats supported by that library.
//!
//! \param[in] file Path to the image file to load
//! \return an initialized texture object
struct texture *
TextureInitFromFile(char *file);

//! \brief De-initialize a texture object
//!
//! \param[in,out] texture The texture object to de-initialize
void
TextureDeinit(struct texture *texture);

//! \brief Texel lookup
//!
//! Textures are organized such that (0,0) is the top-left corner,
//! and (0,1) is the lower-left corner.
//!
//! \param[in] texture the Texture object to sample
//! \param[in] u the horizontal texture coordinate
//! \param[in] v the vertical texture coordinate
unsigned int
TextureSample(struct texture *texture, float u, float v);

#endif // TEXTURE_VERSION
