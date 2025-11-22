#include <raylib.h>
#include <raymath.h> 
#include "ecs/components.h"
#include "resource_manager.h"
#include "systems.h"
#include "utility.h"
#include "ecs/systems.h"

void PlayerControlSystem(struct Systems* systems) {
  uint32_t mask = COMPONENT_PLAYER_CONTROL | COMPONENT_PHYSICS | COMPONENT_TRANSFORM | COMPONENT_WEAPON_CONTROL; 

  EntityManager* em = &systems->entityManager;
  InputSystem* keys = &systems->configManager.KeyMap;

  const float MAX_PITCH_RAD = 1.4f; 

  const float BOB_FREQUENCY = 0.55f;
  const float BOB_AMPLITUDE = 0.2f;

  const float SWAY_SPEED        = 4.5f;
  const float LEAN_TURN_FACTOR  = -0.06f;
  const float LEAN_MOUSE_FACTOR = 0.005f;
  const float LEAN_MOVE_FACTOR  = 0.02f;  

  const float THROTTLE_LERP_SPEED = 2.5f; 
  const float TURN_LERP_SPEED     = 1.0f; 
  const float VELOCITY_LERP_SPEED = 1.0f;

  const float DEFAULT_FOV = 60.0f;
  const float ZOOM_FOV = 20.0f;
  const float ZOOM_SPEED = 10.0f;

  float dt = systems->delta_time;

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {

      PlayerControlComponent* p = &em->playerControlComponents[i];
      PhysicsComponent* phys    = &em->physicsComponents[i];
      TransformComponent* trans = &em->transformComponents[i];
      WeaponControlComponent* wc = &em->weaponControlComponents[i];


      // Mouse Input
      Vector2 mouseDelta = GetMouseDelta();
      p->torsoYaw   += mouseDelta.x * p->mouseSensitivity;
      p->torsoPitch -= mouseDelta.y * p->mouseSensitivity; 
      p->torsoPitch = Clamp(p->torsoPitch, -MAX_PITCH_RAD, MAX_PITCH_RAD);

      float targetFOV = DEFAULT_FOV; 
      if (IsMouseButtonDown(keys->KeyZoom)){
        targetFOV = ZOOM_FOV;
        p->isZooming = true;
      }
      else{
        p->isZooming = false;
      }

      // Weapons Input

      // Aims always on the center of the screen
      Vector3 aimVector = Vector3Subtract(p->camera->target, p->camera->position); 
      wc->aimDirection = Vector3Normalize(aimVector);

      for (int group = 0; group < MAX_WEAPONS_GROUPS; group++){
        if(IsKeyPressed(keys->KeyWeaponGroups[group])) wc->activeGroup[group] = !wc->activeGroup[group];
      }

      if(IsMouseButtonDown(keys->KeyShoot)) {
        wc->triggerPulled = true;
      }
      else wc->triggerPulled = false;

      // Keyboard Input
      float targetThrottle = 0.0f;
      float targetTurn = 0.0f;

      if (IsKeyDown(keys->KeyMoveForward))  targetThrottle = 1.0f;
      if (IsKeyDown(keys->KeyMoveBackward)) targetThrottle = -1.0f;
      if (IsKeyDown(keys->KeyTurnLeft))     targetTurn = 1.0f;
      if (IsKeyDown(keys->KeyTurnRight))    targetTurn = -1.0f;
      // Math for acceleration on forward movement(throttle) and rotation(turnstate) 
      p->throttle  = Lerp(p->throttle, targetThrottle, THROTTLE_LERP_SPEED * dt);
      p->turnState = Lerp(p->turnState, targetTurn, TURN_LERP_SPEED * dt);

      // Input flags
      p->isMoving = (fabs(p->throttle) > 0.01f);
      p->isRotating = (fabs(mouseDelta.x) > 0.1f || fabs(mouseDelta.y) > 0.1f || fabs(p->turnState) > 0.01f);

      // Physics Logic
      if (p->isMoving) {
        Quaternion rot = QuaternionFromAxisAngle((Vector3){0,1,0}, p->turnState * p->turnSpeed * dt);
        trans->orientation = QuaternionMultiply(trans->orientation, rot);
      }

      Vector3 forward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);
      Vector3 desiredVelocity = Vector3Scale(forward, p->throttle * p->maxSpeed);

      phys->velocity = Vector3Lerp(phys->velocity, desiredVelocity, VELOCITY_LERP_SPEED * dt);

      // Cockpit Animation (Sway & Lean)
      // Bobbing
      if (fabs(p->throttle) > 0.1f) {
        p->headTimer += dt * BOB_FREQUENCY;
        p->walkLerp = Lerp(p->walkLerp, 1.0f, 5.0f * dt);
      } else {
        p->walkLerp = Lerp(p->walkLerp, 0.0f, 5.0f * dt);
        if (p->walkLerp < 0.01f) p->headTimer = 0.0f;
      }

      // Lean
      float targetLeanX = (p->turnState * LEAN_TURN_FACTOR) + (mouseDelta.x * LEAN_MOUSE_FACTOR);
      float targetLeanY = p->throttle * LEAN_MOVE_FACTOR;

      p->lean.x = Lerp(p->lean.x, targetLeanX, SWAY_SPEED * dt);
      p->lean.y = Lerp(p->lean.y, targetLeanY, SWAY_SPEED * dt);  


      // Camera Calculation
      // Position
      float bobY = sinf(p->headTimer * PI * 2.0f) * BOB_AMPLITUDE * p->walkLerp;
      p->camera->position = Vector3Add(trans->position, (Vector3){0, MECH_HEIGHT + bobY, 0});

      // Target (Direction)
      float finalPitch = p->torsoPitch - p->lean.y;
      float finalYaw   = p->torsoYaw;

      Vector3 direction;
      direction.x = cosf(finalPitch) * sinf(finalYaw); 
      direction.y = sinf(finalPitch);
      direction.z = cosf(finalPitch) * cosf(finalYaw);

      direction = Vector3Normalize(direction);
      direction.z *= -1.0f; // Raylib Forward é -Z

      p->camera->target = Vector3Add(p->camera->position, direction);
      p->camera->fovy = Lerp(p->camera->fovy, targetFOV, ZOOM_SPEED * dt);

      // Up Vector (Roll Effect)
      float bobRoll = cosf(p->headTimer * PI) * 0.02f * p->walkLerp;
      float finalRoll = p->lean.x + bobRoll;

      p->camera->up = Vector3RotateByAxisAngle((Vector3){0,1,0}, direction, finalRoll);
    }
  }
}  


void PlayerAudioSystem(struct Systems* systems) {
  uint32_t mask = COMPONENT_PLAYER_CONTROL;

  EntityManager* em = &systems->entityManager;

  Sound sfxFootstep  = systems->resourceManager.sounds[SOUND_ID_MECHA_FOOTSTEP];

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {

      PlayerControlComponent* p = &em->playerControlComponents[i];

      // Maintaining a diff between currentstep and laststep helps select when to replay the sound
      float currentStep = p->headTimer * 1.0f; 
      float lastStep    = p->lastHeadTimer * 1.0f;

      // If the current step goes into the ground, play the sound
      if ((int)currentStep > (int)lastStep) {

        // Volume goes with the velocity
        float intensity = fabs(p->throttle); 

        if (p->isMoving) {
          float stepVolume = 0.3f + (intensity * 0.7f); 
          SetSoundVolume(sfxFootstep, stepVolume * systems->configManager.audioVolume);

          // Randomized pitch for variance in sound
          float pitchVar = 0.95f + ((float)GetRandomValue(-5, 5) / 100.0f);
          SetSoundPitch(sfxFootstep, pitchVar);

          PlaySound(sfxFootstep);
        }
      }

      // Updates timer variables
      if (p->headTimer < p->lastHeadTimer) p->lastHeadTimer = p->headTimer;
      else p->lastHeadTimer = p->headTimer;
    }
  }
}
