/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: color.c
  Created: 2019-08-15
  Updated: 2019-08-25
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file color.c

#include "color.h"

struct color ColorWhite = { 0xFFFFFFFF };
struct color ColorBlack = { 0x000000FF };
struct color ColorRed = { 0xFF0000FF };
struct color ColorGreen = { 0x00FF00FF };
struct color ColorBlue = { 0x0000FFFF };
struct color ColorPurple = { 0x7F00FFFF };
struct color ColorYellow = { 0xFFFF00FF };
struct color ColorCyan = { 0x00FFFFFF };
struct color ColorPink = { 0xFF00FFFF };

void ColorSetInt(struct color *color, char component, unsigned int value) {
        unsigned int pos = 0;
        switch (component) {
                case 'r':
                        pos = 3;
                        break;
                case 'g':
                        pos = 2;
                        break;
                case 'b':
                        pos = 1;
                        break;
                case 'a':
                        pos = 0;
                        break;
                default:
                        pos = 0;
        }

        unsigned int shift = pos * 8;

        unsigned int rgba = color->rgba & ~(0xFF << shift);
        color->rgba = rgba | (value << shift);
}

void ColorSetFloat(struct color *color, char component, float value) {
        unsigned int intVal = (unsigned int)(value * 255.0f);
        ColorSetInt(color, component, intVal);
}

struct color ColorInitFloat(float r, float g, float b, float a) {
        struct color color = { 0 };

        ColorSetFloat(&color, 'r', r);
        ColorSetFloat(&color, 'g', g);
        ColorSetFloat(&color, 'b', b);
        ColorSetFloat(&color, 'a', a);

        return color;
}

unsigned int ColorGetInt(struct color color, char component) {
        unsigned int pos = 0;
        switch (component) {
                case 'r':
                        pos = 3;
                        break;
                case 'g':
                        pos = 2;
                        break;
                case 'b':
                        pos = 1;
                        break;
                case 'a':
                        pos = 0;
                        break;
                default:
                        pos = 0;
        }

        unsigned int shift = pos * 8;
        return (color.rgba >> shift) & 0xFF;
}

float ColorGetFloat(struct color color, char component) {
        int value = ColorGetInt(color, component);
        return (float)value / 255.0f;
}
