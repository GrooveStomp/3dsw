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
