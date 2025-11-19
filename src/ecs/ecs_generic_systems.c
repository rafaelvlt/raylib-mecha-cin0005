#include <raylib.h>
#include "systems.h"
#include "ecs/ecs_systems.h"
#include "raymath.h" 

// Query all components with Transform and Physics Components, make them move based on their data

void MovementSystem(struct Systems* systems) {
  const uint32_t mask = COMPONENT_TRANSFORM | COMPONENT_PHYSICS;
  EntityManager* em = &(systems->entityManager);
  for (Entity i = 0; i < em->numEntities; i++) {
    // Bitmask check
    if ((em->componentMasks[i] & mask) == mask) {
      float deltaTime = systems->delta_time;

      // Gets the a pointer to the components from the Manager
      TransformComponent* transform = &(em->transformComponents[i]);
      PhysicsComponent* physics = &(em->physicsComponents[i]);

      // Vf = Vi + a*dt
      physics->velocity = Vector3Add(physics->velocity, Vector3Scale(physics->acceleration, deltaTime));

      // Pf = Pi + v*dt
      transform->position = Vector3Add(transform->position, Vector3Scale(physics->velocity, deltaTime));

      // Apply a little friction
      // TBD -> Make friction definite and a macro, just a dummy value
      physics->velocity = Vector3Scale(physics->velocity, 0.98f);

      // Zero out the acceleration vector
      physics->acceleration = Vector3Zero();
    }
  }
}

void RenderSystem(struct Systems* systems) {
    const uint32_t mask = COMPONENT_TRANSFORM | COMPONENT_RENDER;
    EntityManager* em = &(systems->entityManager);
    for (Entity i = 0; i < em->numEntities; i++) {
        if ((em->componentMasks[i] & mask) == mask) {
            
            RenderComponent* render = &em->renderComponents[i];
            
            // If the attachment is destroyed, doesn't draw it
            if (!render->isVisible) continue;

            TransformComponent* transform = &em->transformComponents[i];
            
            Vector3 axis;
            float angle;
            
            // Converts the quartenion to angle and axis, make it possible to work with DrawModelEx
            QuaternionToAxisAngle(transform->orientation, &axis, &angle);
            
            // Angle needs to be converted from Rad to Degree, the Vector3 is the scale component
            DrawModelEx(*(render->model), transform->position, axis, angle * RAD2DEG, (Vector3){ 1.0f, 1.0f, 1.0f }, render->tint);
        }
    }
}


