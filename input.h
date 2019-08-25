/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: input.h
  Created: 2019-07-21
  Updated: 2019-08-25
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file input.h
//!
//! This package provides a small input abstraction layer to separate
//! input processing from system emulation.
//! Input operates in the same thread as graphics, at the same frequency (30hz)
//! and calls back into the system emulation thread.

#ifndef INPUT_VERSION
#define INPUT_VERSION "0.1.0"

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

int
InputIsQuitPressed(struct input *input, SDL_Event *event);

//! \brief Handle input
//!
//! \param[in,out] input Input state to be updated
//! \param[in] event SDL event to process
void
InputProcess(struct input *input);

#endif // INPUT_VERSION
