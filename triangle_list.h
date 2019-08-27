/******************************************************************************
  GrooveStomp's 3D Software Renderer
  Copyright (c) 2019 Aaron Oman (GrooveStomp)

  File: triangle_list.h
  Created: 2019-08-17
  Updated: 2019-08-27
  Author: Aaron Oman
  Notice: GNU GPLv3 License

  Based off of: One Lone Coder Console Game Engine Copyright (C) 2018 Javidx9
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it under certain
  conditions; See LICENSE for details.
 ******************************************************************************/

#ifndef TRIANGLE_LIST_VERSION
#define TRIANGLE_LIST_VERSION "0.1.0" //!< include guard

//! \file triangle_list.h
//! Ths interface is used exclusively in main.c, but is extracted here for
//! clarity.
//!
//! This is an extremely basic FIFO queue for triangles.  It performs no error
//! checking and is essentially use-once.
//!
//! It statically allocates a list of 256 triangles and continually shifts the
//! head and tail toward the end of the list until there is no capacity left.
//! There is no mechanism for resetting the list or copying to another list.
//!
//! This extremely simplistic implementation was chosen for two reasons:
//! - It suits the requirements of the program
//! - It is less complex, and thus the chance of introducing bugs is drastically reduced
//!
//! This is a prime candidate for building into a more robust interface.

#include "math.h"

//! \brief A FIFO queue for triangles
//!
//! Note that this structure uses copies of triangles and not pointers.
struct triangle_list {
        struct triangle triangles[256];
        unsigned char head;
        unsigned char tail;
};

//! \brief Initialize the triangle list
//!
//! \return Initialized triangle_list structure
struct triangle_list TriangleListInit() {
        struct triangle_list triangleList;
        triangleList.head = 0;
        triangleList.tail = 0;
        return triangleList;
}

//! \brief Pop the oldest triangle
//!
//! Returns a copy of the oldest triangle in the list and moves the head.
//!
//! Note: No error checking is performed!
//!
//! \param[in] triangleList The triangle_list structure to pop
//! \return A copy of the oldest triangle in the list
struct triangle TriangleListPopFront(struct triangle_list *triangleList) {
        if (triangleList->head == triangleList->tail) {
                return (struct triangle){0};
        }
        return triangleList->triangles[triangleList->head++];
}

//! \brief Push a new elemnt onto the triangle list
//!
//! Note: No error checking is performed!
//!
//! \param[in,out] triangleList the triangle_list structure to modify
//! \param[in] in the triangle to push onto triangleList
void TriangleListPushBack(struct triangle_list *triangleList, struct triangle in) {
        triangleList->triangles[triangleList->tail++] = in;
}

//! \brief Get the number of elements in the list
//!
//! \param[in] triangleList the triangle_list structure to read
//! \return number of elements in the list
int TriangleListSize(struct triangle_list *triangleList) {
        return triangleList->tail - triangleList->head;
}

#endif // TRIANGLE_LIST_VERSION
