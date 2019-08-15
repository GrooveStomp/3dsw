/******************************************************************************
  File: color.c
  Created: 2019-08-15
  Updated: 2019-08-15
  Author: Aaron Oman
  Notice: Creative Commons Attribution 4.0 International License (CC-BY 4.0)
 ******************************************************************************/

#include "color.h"

struct color ColorWhite = { 0xFFFFFFFF };
struct color ColorBlack = { 0x000000FF };

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
