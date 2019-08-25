/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: graphics.h
  Created: 2019-07-16
  Updated: 2019-08-25
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file graphics.h
//! Drawing interface to the operating system.

#ifndef GRAPHICS_VERSION
#define GRAPHICS_VERSION "0.1.0" //!< include guard

#include "SDL2/SDL.h"

struct triangle;
struct texture;

//! \brief Creates and initializes a new graphics object isntance
//!
//! Scale can be specified as a non-negative number. This value is used to
//! multiply both the width and the height and the pixel size of any drawing
//! operations.
//!
//! For example, specifying a scale of 2 would multiply the width by 2, the
//! height by 2, and every pixel would be 2x2; so the total scale factor ends up
//! being scale^2
//!
//! \param[in] title The title displayed in the window titlebar
//! \param[in] width Width of the display area of the window, in pixels
//! \param[in] height Height of the display are of the window, in pixels
//! \param[in] scale Size and rendering scale, natural number multiple
//! \return The initialized graphics object
struct graphics *
GraphicsInit(char *title, int width, int height, int scale);

//! \brief De-initializes and frees memory for the given graphics object
//! \param[in,out] graphics The initialized opcode object to be cleaned and reclaimed
void
GraphicsDeinit(struct graphics *graphics);

//! \brief Initializes the graphics subsystem for drawing routines
//!
//! Internally locks streaming texture for direct manipulation.
//!
//! \param[in, out] graphics Graphics state to be manipulated
void
GraphicsBegin(struct graphics *graphics);

//! \brief Prepares the graphics subsystem for presentation, then presents
//!
//! Internally unlocks streaming texture then calls presentation routines.
//!
//! \param[in, out] graphics Graphics state to be manipulated.
void
GraphicsEnd(struct graphics *graphics);

//! \brief Sets all pixels in the screen to the given color
//!
//! \param[in, out] graphics Graphics state to be manipulated
//! \param[in] color 32-bit color with 8-bits per component: (R,G,B,A)
void
GraphicsClearScreen(struct graphics *graphics, unsigned int color);


//! \brief Draw a triangle with the given set of x and y coordinates
//!
//! Only draws the lines, doesn't fill the polygon.
//!
//! \param[in, out] graphics Graphics state to be changed
//! \param[in] triangle The triangle to draw
//! \param[in] color What color the wireframe should be rendered with
void
GraphicsTriangleWireframe(struct graphics *graphics, struct triangle triangle, unsigned int color);

//! \brief Draw a triangle with the given set of x and y coordinates
//!
//! Fills the specified polygon with the given color.
//!
//! \param[in, out] graphics Graphics state to be changed
//! \param[in] triangle The triangle to draw
//! \param[in] color What color the solid triangle should be rendered with
//!
//! \see Source: http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
void
GraphicsTriangleSolid(struct graphics *graphics, struct triangle triangle, unsigned int color);

//! \brief Draw a textured triangle with the given set of x and y coordinates
//!
//! Fills the specified polygon with the given texture.
//!
//! \param[in, out] graphics Graphics state to be changed
//! \param[in] tri The triangle to draw
//! \param[in] texture What texture to sample while drawing the solid triangle
//!
//! \see Source: https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part4.cpp
void
GraphicsTriangleTextured(struct graphics *graphics, struct triangle tri, struct texture *texture);

#endif // GRAPHICS_VERSION
