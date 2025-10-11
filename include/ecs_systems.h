#ifndef ECS_SYSTEMS_H
#define ECS_SYSTEMS_H
#include "ecs_entitymanager.h"

// Movement System: Update every position applying physics
void MovementSystem_Update(EntityManager* entityManager, float deltaTime);

// Drawing System: Draw every visible Entity
void RenderSystem_Draw(EntityManager* entityManager);

// Player Control
void InputSystem_Update(EntityManager* entityManager);

#endif // ECS_SYSTEMS_H
