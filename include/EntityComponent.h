#ifndef ECS.H
#define ECS.H
#include <raylib.h>
#include <stdint.h>

#define MAX_ENTITIES 5000

typedef uint64_t Entity;

typedef struct
{
    Vector3 position;
    Vector3 velocity;
    Vector3 aceleration;
} MovementComponent;


#endif
