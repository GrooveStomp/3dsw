/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: triangle_list.h
  Created: 2019-08-17
  Updated: 2019-08-25
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

//! \file triangle_list.h

#include "math.h"

struct triangle_list {
        struct triangle triangles[256];
        unsigned char head;
        unsigned char tail;
};

struct triangle_list TriangleListInit() {
        struct triangle_list triangleList;
        triangleList.head = 0;
        triangleList.tail = 0;
        return triangleList;
}

struct triangle TriangleListPopFront(struct triangle_list *triangleList) {
        if (triangleList->head == triangleList->tail) {
                return (struct triangle){0};
        }
        return triangleList->triangles[triangleList->head++];
}

void TriangleListPushBack(struct triangle_list *triangleList, struct triangle in) {
        triangleList->triangles[triangleList->tail++] = in;
}

int TriangleListSize(struct triangle_list *triangleList) {
        return triangleList->tail - triangleList->head;
}
