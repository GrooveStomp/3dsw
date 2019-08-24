/******************************************************************************
  File: color.h
  Created: 2019-08-15
  Updated: 2019-08-17
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/

//! \file color.h

#ifndef COLOR_VERSION
//! include guard
#define COLOR_VERSION "0.1.0"

struct color {
        unsigned int rgba;
};

struct color
ColorInitFloat(float r, float g, float b, float a);

unsigned int
ColorGetInt(struct color color, char component);

float
ColorGetFloat(struct color color, char component);

void
ColorSetInt(struct color *color, char component, unsigned int value);

void
ColorSetFloat(struct color *color, char component, float value);

extern struct color ColorWhite;
extern struct color ColorBlack;
extern struct color ColorRed;
extern struct color ColorGreen;
extern struct color ColorBlue;
extern struct color ColorPurple;
extern struct color ColorYellow;
extern struct color ColorCyan;
extern struct color ColorPink;

#endif // COLOR_VERSION
