/******************************************************************************
  File: graphics.h
  Created: 2019-07-16
  Updated: 2019-08-07
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/

//! \file graphics.h

#ifndef GRAPHICS_VERSION
#define GRAPHICS_VERSION "0.1.0"

#include "SDL2/SDL.h"

//! \brief Creates and initializes a new graphics object isntance
//! \param[in] debug Whether to enabled the debugging UI
//! \return The initialized graphics object
struct graphics *
GraphicsInit(char *title, int width, int height);

//! \brief De-initializes and frees memory for the given graphics object
//! \param[in,out] graphics The initialized opcode object to be cleaned and reclaimed
void
GraphicsDeinit(struct graphics *graphics);

//! \brief Sets all pixels in the screen to the given color
//! \param[in, out] graphics Graphics state to be manipulated
//! \param[in] color 32-bit color with 8-bits per component: (R,G,B,A)
void
GraphicsClearScreen(struct graphics *graphics, unsigned int color);


//! \brief Draw a triangle with the given set of x and y coordinates
//! \param[in, out] graphics Graphics state to be changed
void
GraphicsDrawTriangle(struct graphics *graphics, float x1, float y1, float x2, float y2, float x3, float y3, unsigned int color);

#endif // GRAPHICS_VERSION
