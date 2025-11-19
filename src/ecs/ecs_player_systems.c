#include <raylib.h>
#include <raymath.h> 
#include "systems.h"
#include "ecs/ecs_systems.h"

#define MECH_HEIGHT     6.5f  // Camera Height
// Bobbing frequency parameters
#define BOB_FREQUENCY   2.0f  
#define BOB_AMPLITUDE   0.2f  
// Inclination parameters
#define SWAY_SPEED      4.5f 
#define MAX_PITCH_RAD   1.5f  // Anti neck-break (vertical 180*)

void PlayerControlSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  InputSystem* keys = &systems->configManager.KeyMap;

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & (COMPONENT_PLAYER_CONTROL | COMPONENT_PHYSICS | COMPONENT_TRANSFORM)) == 
      (COMPONENT_PLAYER_CONTROL | COMPONENT_PHYSICS | COMPONENT_TRANSFORM)) {

      // Shortcut 
      PlayerControlComponent* p = &em->playerControlComponents[i];
      PhysicsComponent* phys    = &em->physicsComponents[i];
      TransformComponent* trans = &em->transformComponents[i];
      float dt = systems->delta_time;

      // Mouse Input
      Vector2 mouseDelta = GetMouseDelta();
      p->torsoYaw   -= mouseDelta.x * p->mouseSensitivity;
      p->torsoPitch -= mouseDelta.y * p->mouseSensitivity;
      p->torsoPitch = Clamp(p->torsoPitch, -MAX_PITCH_RAD, MAX_PITCH_RAD);

      // Movement Input
      float targetThrottle = 0.0f;
      float targetTurn = 0.0f;
      if (IsKeyDown(keys->KeyMoveForward))  targetThrottle = 1.0f;
      if (IsKeyDown(keys->KeyMoveBackward)) targetThrottle = -1.0f;
      if (IsKeyDown(keys->KeyTurnLeft))     targetTurn = 1.0f;
      if (IsKeyDown(keys->KeyTurnRight))    targetTurn = -1.0f;

      p->throttle = Lerp(p->throttle, targetThrottle, 10.0f * dt);
      p->turnState = Lerp(p->turnState, targetTurn, 10.0f * dt);


      // Physics
      // Rotation (Yaw)
      // Rotates on y axis of the world
      if (fabs(p->turnState) > 0.01f) {
        Quaternion rot = QuaternionFromAxisAngle((Vector3){0,1,0}, p->turnState * p->turnSpeed * dt);
        trans->orientation = QuaternionMultiply(trans->orientation, rot);
      }
      
      // Foward Movement based on legs orientation
      Vector3 forward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);

      // Wanted velocity based on throttle
      Vector3 desiredVelocity = Vector3Scale(forward, p->throttle * p->maxSpeed);

      // Change physics component to be worked on by the Movement System later
      phys->velocity = Vector3Lerp(phys->velocity, desiredVelocity, 2.0f * dt);


      // Cockpit Camera Animation

      // Head Bobbing
      // Only bobs if the mech is moving, increase frequency when moving alot, and stops it when the mech stop
      if (fabs(p->throttle) > 0.1f) {
        p->headTimer += dt * BOB_FREQUENCY;
        p->walkLerp = Lerp(p->walkLerp, 1.0f, 5.0f * dt);
      } else {
        p->walkLerp = Lerp(p->walkLerp, 0.0f, 5.0f * dt);
        if (p->walkLerp < 0.01f) p->headTimer = 0.0f;
      }

      // Swaying 
      // Leans the cockpit based on acceleration and rotation
      p->lean.x = Lerp(p->lean.x, p->turnState * -0.05f, SWAY_SPEED * dt); 
      p->lean.y = Lerp(p->lean.y, p->throttle * 0.02f, SWAY_SPEED * dt);  

      
      // Camera alteration
      // Position + Mech Height + Bobbing + Lerp; 
      float bobY = sinf(p->headTimer * PI * 2.0f) * BOB_AMPLITUDE * p->walkLerp;
      p->camera->position = Vector3Add(trans->position, (Vector3){0, MECH_HEIGHT + bobY, 0});

      // Camera Target
      //Rotation Matrix based on Pitch, Yaw and Lean
      float finalPitch = p->torsoPitch - p->lean.y;
      float finalYaw   = p->torsoYaw; // Torso works solo, legs dont have influence on camera

      Matrix rotMatrix = MatrixRotateXYZ((Vector3){ finalPitch, finalYaw, 0.0f });

      // Raylib defaults forward to z = -1 
      Vector3 camForward = Vector3Transform((Vector3){0, 0, -1}, rotMatrix);

      // Position + Target 
      p->camera->target = Vector3Add(p->camera->position, camForward);

      // Bobbing + Lean
      float bobRoll = cosf(p->headTimer * PI) * 0.02f * p->walkLerp;
      float finalRoll = p->lean.x + bobRoll;

      // Rotates Default Up vector by rool
      p->camera->up = Vector3Transform((Vector3){0, 1, 0}, MatrixRotateZ(finalRoll));
    }
  }
}
