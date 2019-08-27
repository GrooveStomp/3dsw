/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: input.h
  Created: 2019-07-21
  Updated: 2019-08-27
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file input.h
//! A small interface to manage handling input separately from main().

#ifndef INPUT_VERSION
#define INPUT_VERSION "0.1.0" //!< include guard

#include "SDL.h"

struct input;
struct system;

//! \brief Creates and initializes a new input object
//! \return The initialized input object
struct input *
InputInit();

//! \brief De-initializes and frees memory for the given input object
//! \param[in,out] input The initialized input object to be cleaned and reclaimed
void
InputDeinit(struct input *input);

//! \brief Check if the user has tried to quit the program
//!
//! \param[in] event the event object to query
//! \return 1 if quit has been pressed, otherwise zero
int
InputIsQuitPressed(SDL_Event *event);

//! \brief Handle input
//!
//! This function reaches into global state and changes it based on user input,
//! doing things like changing the view direction.
//!
//! This interface is a prime candidate to be reworked into something far less
//! intrusive, like a callback mechanism.
//!
//! \param[in,out] input Input state to be updated
void
InputProcess(struct input *input);

#endif // INPUT_VERSION
