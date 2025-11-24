#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include <raylib.h>
#include "ecs/entitymanager.h"
#include "resource_manager.h"

typedef struct {
    Camera* mainCamera;
} MapContext;

void LoadMapFromText(EntityManager* em, ResourceManager* rm, const char* filename, MapContext context);

#endif