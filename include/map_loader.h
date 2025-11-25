#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <raylib.h>
#include "ecs/entitymanager.h"
#include "resource_manager.h"

typedef struct {
    Camera* mainCamera;
} MapContext;


// Simple table for id -> name 
typedef struct {
    char name[64];
    Entity id;
} EntityMap;

typedef struct {
    Vector3 position;
    float size;
    float speedMultiplier;
} CloudData;

void LoadMapFromText(EntityManager* em, ResourceManager* rm, const char* filename, MapContext context);

#endif
