#include <raylib.h>
#include <raymath.h> 
#include "ecs/components.h"
#include "resource_manager.h"
#include "systems.h"
#include "utility.h"
#include "ecs/systems.h"

// Movement
#define MAX_YAW_RAD           1.4f
#define MAX_PITCH_RAD         0.8f
#define THROTTLE_LERP_SPEED   4.5f 
#define TURN_LERP_SPEED       2.0f
#define VELOCITY_LERP_SPEED   2.0f
// Centering
#define CENTERING_TORSO_LEGS_SPEED 5.0f
#define CENTERING_LEGS_TORSO_SPEED 4.0f
// Zoom
#define ZOOM_FOV              20.0f
#define DEFAULT_FOV           60.0f
#define ZOOM_SPEED            10.0f
// Lock
#define MAX_LOCK_DISTANCE     1000.0f * 1000.0f
// Animation
#define MECH_HEIGHT     6.5f  
#define BOB_FREQUENCY   1.5f
#define BOB_AMPLITUDE   0.2f
#define SWAY_SPEED      4.5f
#define LEAN_TURN       -0.06f
#define LEAN_MOUSE      0.0f
#define LEAN_MOVE       0.02f  

// Input function
static void ProcessPlayerInput(struct Systems* systems, PlayerControlComponent* p, WeaponControlComponent* wc, float dt);

static void UpdatePlayerPhysics(PlayerControlComponent* p, TransformComponent* trans, PhysicsComponent* phys, float dt);

static void UpdateCockpitCamera(PlayerControlComponent* p, TransformComponent* trans, WeaponControlComponent* wc, float dt);

// Target Lock functions
static void UpdateTargetLock(struct Systems* systems, PlayerControlComponent* p, WeaponControlComponent* wc);
static bool GetTargetInfo(EntityManager* em, Entity target, Vector3 myPos, Vector3 myAim, float* outDistSq, float* outDot);

void PlayerControlSystem(struct Systems* systems) {
  uint32_t mask = COMPONENT_PLAYER_CONTROL | COMPONENT_PHYSICS | COMPONENT_TRANSFORM | COMPONENT_WEAPON_CONTROL; 

  EntityManager* em = &systems->entityManager;

  float dt = systems->delta_time;

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {

      PlayerControlComponent* p = &em->playerControlComponents[i];
      PhysicsComponent* phys    = &em->physicsComponents[i];
      TransformComponent* trans = &em->transformComponents[i];
      WeaponControlComponent* wc = &em->weaponControlComponents[i];

      ProcessPlayerInput(systems, p, wc, dt);

      UpdatePlayerPhysics(p, trans, phys, dt);

      UpdateCockpitCamera(p, trans, wc, dt);
    }
  }
}  


void PlayerAudioSystem(struct Systems* systems) {
  uint32_t mask = COMPONENT_PLAYER_CONTROL;

  EntityManager* em = &systems->entityManager;

  Sound sfxFootstep  = *GetSound(&systems->resourceManager, SOUND_ID_MECHA_FOOTSTEP);
  Sound sfxZoom = *GetSound(&systems->resourceManager, SOUND_ID_MECHA_ZOOM);

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
          float stepVolume = 0.3f + (intensity * 0.3f); 
          SetSoundVolume(sfxFootstep, stepVolume * systems->configManager.musicVolume);

          // Randomized pitch for variance in sound
          float pitchVar = 0.95f + ((float)GetRandomValue(-5, 5) / 100.0f);
          SetSoundPitch(sfxFootstep, pitchVar);

          PlaySound(sfxFootstep);
        }
      }

      if (p->isZooming && !p->wasZooming) {

        float vol = systems->configManager.musicVolume;
        SetSoundVolume(sfxZoom, vol);
        SetSoundPitch(sfxZoom, 1.0f + ((float)GetRandomValue(-5, 5)/100.0f));

        if (!IsSoundPlaying(sfxZoom)) {
          PlaySound(sfxZoom);
        }
      }


      // Updates timer/state variables
      if (p->headTimer < p->lastHeadTimer) p->lastHeadTimer = p->headTimer; else p->lastHeadTimer = p->headTimer;
      p->wasZooming = p->isZooming;
    }
    

  }
}

static void ProcessPlayerInput(struct Systems* systems, PlayerControlComponent* p, WeaponControlComponent* wc, float dt) {
  InputSystem* keys = &systems->configManager.KeyMap;

  Vector2 mouseDelta = GetMouseDelta();

  p->torsoYaw += mouseDelta.x * p->mouseSensitivity;
  p->torsoYaw = Clamp(p->torsoYaw, -MAX_YAW_RAD, MAX_YAW_RAD);

  p->torsoPitch -= mouseDelta.y * p->mouseSensitivity; 
  p->torsoPitch = Clamp(p->torsoPitch, -MAX_PITCH_RAD, MAX_PITCH_RAD);

  // Keyboard Input(Rotation and forward/backwards)
  float targetThrottle = 0.0f;
  float targetTurn = 0.0f;

  if (IsKeyDown(keys->KeyMoveForward))  targetThrottle = 1.0f;
  if (IsKeyDown(keys->KeyMoveBackward)) targetThrottle = -1.0f;
  if (IsKeyDown(keys->KeyTurnLeft))     targetTurn = 1.0f;
  if (IsKeyDown(keys->KeyTurnRight))    targetTurn = -1.0f;
  // Lerp for it to be gradual
  p->throttle  = Lerp(p->throttle, targetThrottle, THROTTLE_LERP_SPEED * dt);
  p->turnState = Lerp(p->turnState, targetTurn, TURN_LERP_SPEED * dt);


  // Center Torso to Legs
  if (!p->centeringTorsotoLegs) p->centeringTorsotoLegs = IsKeyPressed(keys->KeyCenterTorsoToLegs);
  if (p->centeringTorsotoLegs){
    p->torsoYaw = Lerp(p->torsoYaw, 0.0f, dt * 5.0f);
    p->torsoPitch = Lerp(p->torsoPitch, 0.0f, dt * 5.0f);

    // If close enough, makes it zero and turns off flag  
    if (fabs(p->torsoYaw) < 0.01f && fabs(p->torsoPitch) < 0.01f) {
      p->torsoYaw = 0.0f;
      p->torsoPitch = 0.0f;
      p->centeringTorsotoLegs = false;
    }

    // QOL: If the player tries to move alot during the centering, cancels it
    if (fabs(mouseDelta.x) > 1.0f || fabs(mouseDelta.y) > 1.0f) {
      p->centeringTorsotoLegs = false;
    }
  }

  // Center Legs to Torso
  if (!p->centeringLegstoTorso) p->centeringLegstoTorso = IsKeyPressed(keys->KeyCenterLegsToTorso);
  // The logic is dealt by the physics function

  // Zoom
  float targetFOV = IsMouseButtonDown(keys->KeyZoom) ? ZOOM_FOV : DEFAULT_FOV;
  p->isZooming = IsMouseButtonDown(keys->KeyZoom);
  p->camera->fovy = Lerp(p->camera->fovy, targetFOV, ZOOM_SPEED * dt);

  // Weapons input

  wc->triggerPulled = IsMouseButtonDown(keys->KeyShoot);
  // Weapon Groups
  for (int group = 0; group < MAX_WEAPONS_GROUPS; group++){
    if(IsKeyPressed(keys->KeyWeaponGroups[group])) wc->activeGroup[group] = !wc->activeGroup[group];
  }

  // Target Lock
  UpdateTargetLock(systems, p, wc);

  // Flags
  p->isMoving = (fabs(p->throttle) > 0.01f);
  p->isRotating = (fabs(p->turnState) > 0.01f);
}

static void UpdatePlayerPhysics(PlayerControlComponent* p, TransformComponent* trans, PhysicsComponent* phys, float dt) {
  // Centering Legs to Torso
  if (p->centeringLegstoTorso) {
    float oldTorsoYaw = p->torsoYaw;
    p->torsoYaw = Lerp(p->torsoYaw, 0.0f, CENTERING_LEGS_TORSO_SPEED * dt);
    float deltaAngle = p->torsoYaw - oldTorsoYaw;
    p->legAngle += deltaAngle;

    if (fabs(p->torsoYaw) < 0.01f) {
      p->torsoYaw = 0.0f;
      p->centeringLegstoTorso = false;
    }
  }
  // Leg rotation also counts for camera rotation
  else if (p->isRotating) {
    p->legAngle += p->turnState * p->turnSpeed * dt;
  }

  if (p->legAngle > PI * 2) p->legAngle -= PI * 2;
  if (p->legAngle < 0) p->legAngle += PI * 2;

  trans->orientation = QuaternionFromAxisAngle((Vector3){0, 1, 0}, p->legAngle);

  // Velocity (Raylib forward is z = -1);
  Vector3 forward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);
  Vector3 desiredVelocity = Vector3Scale(forward, p->throttle * p->maxSpeed);
  phys->velocity = Vector3Lerp(phys->velocity, desiredVelocity, VELOCITY_LERP_SPEED * dt);

}

static void UpdateCockpitCamera(PlayerControlComponent* p, TransformComponent* trans, WeaponControlComponent* wc, float dt) {

  // Bobbing (Forward/Backward Movement)
  if (p->isMoving) {
    p->headTimer += dt * BOB_FREQUENCY;
    p->walkLerp = Lerp(p->walkLerp, 1.0f, 5.0f * dt);
  } else {
    p->walkLerp = Lerp(p->walkLerp, 0.0f, 5.0f * dt);
    if (p->walkLerp < 0.01f) p->headTimer = 0.0f;
  }

  // Lean (Horizontal Movement)
  Vector2 mouseDelta = GetMouseDelta();
  float targetLeanX = (p->turnState * LEAN_TURN) + (mouseDelta.x * LEAN_MOUSE);
  float targetLeanY = p->throttle * LEAN_MOVE;
  p->lean.x = Lerp(p->lean.x, targetLeanX, SWAY_SPEED * dt);
  p->lean.y = Lerp(p->lean.y, targetLeanY, SWAY_SPEED * dt);  

  // Camera position
  float bobY = sinf(p->headTimer * PI * 2.0f) * BOB_AMPLITUDE * p->walkLerp;
  p->camera->position = Vector3Add(trans->position, (Vector3){0, MECH_HEIGHT + bobY, 0});

  // Camera rotation
  float finalCameraYaw =  p->torsoYaw - p->legAngle; 
  float finalCameraPitch = p->torsoPitch - p->lean.y;

  // Sphere coordinate to cartesian
  Vector3 direction;
  direction.x = cosf(finalCameraPitch) * sinf(finalCameraYaw); 
  direction.y = sinf(finalCameraPitch);
  direction.z = cosf(finalCameraPitch) * cosf(finalCameraYaw);

  // Nroamlize the forward vector(Z = -1)
  direction = Vector3Normalize(direction);
  direction.z *= -1.0f; // Raylib Forward -Z

  //Updating Camera
  p->camera->target = Vector3Add(p->camera->position, direction);
  float bobRoll = cosf(p->headTimer * PI) * 0.02f * p->walkLerp;
  float finalRoll = p->lean.x + bobRoll;
  p->camera->up = Vector3RotateByAxisAngle((Vector3){0,1,0}, direction, finalRoll);

  //Updates Weapon aim Direction
  wc->aimDirection = direction;
}

static void UpdateTargetLock(struct Systems* systems, PlayerControlComponent* p, WeaponControlComponent* wc) {
  EntityManager* em = &systems->entityManager;
  InputSystem* keys = &systems->configManager.KeyMap;

  Vector3 myPos = p->camera->position;
  Vector3 aimDir = wc->aimDirection;
  float distSq, dot;

  // Unlock logic, checks every frame
  if (wc->lockedTarget != MAX_ENTITIES) {
    bool keepLock = false;
    // Checks if the target is valid
    if (em->componentMasks[wc->lockedTarget] != COMPONENT_NONE && GetTargetInfo(em, wc->lockedTarget, myPos, aimDir, &distSq, &dot)) {
      // Checks if it's in the lock range, and if it's in 180 degree forward cone
      if (distSq <= MAX_LOCK_DISTANCE && dot >= 0.0f) {
        keepLock = true;
      }
    }
    if (!keepLock) {
      wc->lockedTarget = MAX_ENTITIES; // Destrava
    }
  }

  // Lock target logic, checks only on key pressed
  if (IsKeyPressed(keys->KeyLockTarget)) {
    float bestDot = -1.0f;
    Entity bestTarget = MAX_ENTITIES;
    Entity currentTarget = wc->lockedTarget;

    for (Entity i = 0; i < em->numEntities; i++) {
      if (!(em->componentMasks[i] & COMPONENT_AI_CONTROL)) ;
      else {
        GetTargetInfo(em, i, myPos, aimDir, &distSq, &dot);
        if (distSq <= MAX_LOCK_DISTANCE && dot > 0.5f) {
          // Cycle logic, penalizes the dot of the current target to cycle to the next
          if (i == currentTarget) dot -= 2.0f;

          if (dot > bestDot) {
            bestDot = dot;
            bestTarget = i;
          }
        }
      }
    }
    // Locks into new target if he's found
    if (bestTarget != MAX_ENTITIES) {
      wc->lockedTarget = bestTarget;
    } else {
      wc->lockedTarget = MAX_ENTITIES;
    }
  }
}

// Only returns true if he's valid
static bool GetTargetInfo(EntityManager* em, Entity target, Vector3 myPos, Vector3 myAim, float* outDistSq, float* outDot) {

  // Mask Validation
  uint32_t reqMask = COMPONENT_TRANSFORM | COMPONENT_COLLISION;
  if ((em->componentMasks[target] & reqMask) != reqMask) return false;

  // Position
  Vector3 targetPos = em->transformComponents[target].position;

  // Distance
  Vector3 toTarget = Vector3Subtract(targetPos, myPos);
  *outDistSq = Vector3LengthSqr(toTarget);

  // Dot Product (Angle)
  Vector3 dirToTarget = Vector3Normalize(toTarget);
  *outDot = Vector3DotProduct(myAim, dirToTarget); 

  return true;
}
