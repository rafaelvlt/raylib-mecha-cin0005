#include <raylib.h>
#include "ecs/ecs_systems.h"
#include "raymath.h" 

// Query all components with Transform and Physics Components, make them move based on their data

void MovementSystem_Update(EntityManager* entityManager, float deltaTime) {
  const uint32_t mask = COMPONENT_TRANSFORM | COMPONENT_PHYSICS;

  for (Entity i = 0; i < entityManager->numEntities; i++) {
    // Bitmask check
    if ((entityManager->componentMasks[i] & mask) == mask) {

      // Gets the a pointer to the components from the Manager
      TransformComponent* transform = &(entityManager->transformComponents[i]);
      PhysicsComponent* physics = &(entityManager->physicsComponents[i]);

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

void RenderSystem_Draw(EntityManager* entityManager) {
    const uint32_t mask = COMPONENT_TRANSFORM | COMPONENT_RENDER;

    for (Entity i = 0; i < entityManager->numEntities; i++) {
        if ((entityManager->componentMasks[i] & mask) == mask) {
            
            RenderComponent* render = &entityManager->renderComponents[i];
            
            // If the attachment is destroyed, doesn't draw it
            if (!render->isVisible) continue;

            TransformComponent* transform = &entityManager->transformComponents[i];
            
            Vector3 axis;
            float angle;
            
            // Converts the quartenion to angle and axis, make it possible to work with DrawModelEx
            QuaternionToAxisAngle(transform->orientation, &axis, &angle);
            
            // Angle needs to be converted from Rad to Degree, the Vector3 is the scale component
            DrawModelEx(*(render->model), transform->position, axis, angle * RAD2DEG, (Vector3){ 1.0f, 1.0f, 1.0f }, render->tint);
        }
    }
}

void InputSystem_Update(EntityManager* entityManager) {
    const uint32_t mask = COMPONENT_PLAYER_CONTROL | COMPONENT_PHYSICS | COMPONENT_TRANSFORM;

    for (Entity i = 0; i < entityManager->numEntities; i++) {
        if ((entityManager->componentMasks[i] & mask) == mask) {
            
            TransformComponent* transform = &entityManager->transformComponents[i];
            PhysicsComponent* physics = &entityManager->physicsComponents[i];
            PlayerControlComponent* playerControl = &entityManager->playerControlComponents[i];
            
            Vector2 mouseDelta = GetMouseDelta();
            
            // Yaw = Horizontal ; Pitch = Vertical
            playerControl->cameraYaw -= mouseDelta.x * playerControl->mouseSensitivity;
            playerControl->cameraPitch -= -mouseDelta.y * playerControl->mouseSensitivity;
            
            // Limit on the pitch so that it does not tumble
            const float maxPitch = 1.55f; // ~ 89 deegre
            if (playerControl->cameraPitch > maxPitch) playerControl->cameraPitch = maxPitch;
            if (playerControl->cameraPitch < -maxPitch) playerControl->cameraPitch = -maxPitch;
            
            // Mech body can only rotate horizontally
            transform->orientation = QuaternionFromAxisAngle((Vector3){0, 1, 0}, playerControl->cameraYaw);
            
            // Movement Direction
            Vector3 moveDir = {0};
            if (IsKeyDown(KEY_W)) moveDir.z = 1.0f;
            if (IsKeyDown(KEY_S)) moveDir.z = -1.0f;
            if (IsKeyDown(KEY_A)) moveDir.x = 1.0f;
            if (IsKeyDown(KEY_D)) moveDir.x = -1.0f;

            Vector3 rotatedMoveDir = Vector3RotateByQuaternion(moveDir, transform->orientation);
            
            // Arbitrary Acceleration of 50.0f for now TBD
            physics->acceleration = Vector3Scale(rotatedMoveDir, 50.0f);
        }
    }
}

