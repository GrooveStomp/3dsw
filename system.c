#include <stdlib.h> // malloc, free
#include <string.h> // memset

#include "math.h"

struct system {
        struct mesh *mesh;
        float viewNear;
        float viewFar;
        float viewFOV;
        float viewAspectRatio;
};

struct system *SystemDeinit(struct system *system) {
        if (NULL != system) {
                free(system);
        }
}

struct system *SystemInit() {
        struct system *system = (struct system *)malloc(sizeof(struct system));
        memset(system, 0, sizeof(struct system));

        return system;
}
