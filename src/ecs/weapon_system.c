#include <raylib.h>
#include <raymath.h>
#include "resource_manager.h"
#include "utility.h"
#include "systems.h"
#include "ecs/types.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"

#define CONVERGENCE_POINT           125.0f
#define CONVERGENCE_START_RATE      8.0f
#define CONVERGENCE_END_RATE        1.0f
#define MISSILE_MIN_RANGE           100.0f
#define MISSILE_SPREAD              0.8f
#define CAMERA_SPAWN_DISTANCE       15.0f
#define MISSILE_LIFETIME            7.5f
#define LASER_LIFETIME              1.0f

// Helper Functions
static void ProcessWeapon(EntityManager* em, struct Systems* systems, Entity mecha, Entity weaponID, int weaponIndex, float dt);
static bool ValidateMissileFire(EntityManager* em, WeaponControlComponent* wc, Entity mecha, struct Systems* systems);
static Vector3 GetCameraMuzzlePosition(Camera* camera, Vector3 localOffset, WeaponType weaponType);
static Vector3 CalculateProjectileSpawnPosition(EntityManager* em, WeaponControlComponent* wc, Entity mecha, Entity weaponID);
static Vector3 CalculateProjectileDirection(EntityManager* em, WeaponControlComponent* wc, Entity mecha, Vector3 spawnPos);
static void ProcessWeaponFire(struct Systems* systems, Entity mecha, Entity weaponID, WeaponComponent* weapon, WeaponControlComponent* wc, Vector3 spawnPos, Vector3 direction);
static void UpdateWeaponCooldowns(WeaponComponent* weapon, float dt);

// Factory functions
static Entity SpawnProjectileCore(struct Systems* systems, Vector3 pos, Vector3 direction, Entity owner, float speed, float damage, WeaponType type);
static Entity SpawnLaserPulseProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats);
static Entity SpawnMissileProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats, Entity target);

// Processes a single weapon for a mecha entity
static void ProcessWeapon(EntityManager* em, struct Systems* systems, Entity mecha, Entity weaponID, int weaponIndex, float dt) {
  WeaponControlComponent* wc = &em->weaponControlComponents[mecha];
  WeaponComponent* weapon = &em->weaponComponents[weaponID];
  HeatComponent* hc = &em->heatComponents[mecha];

  UpdateWeaponCooldowns(weapon, dt);

  // Se estiver superaquecida, impede o disparo e zera os contadores de burst.
  if ((em->componentMasks[mecha] & COMPONENT_HEAT) && hc->isOverheated) {
      if (hc->isOverheated) {
          // Impede o processamento e disparo da arma se superaquecido.
          return; 
      }
  }


  int group = wc->weaponsGroupMap[weaponIndex];

  // Check if weapon should start firing
  if (wc->triggerPulled && wc->activeGroup[group] && weapon->cooldownTimer <= 0 && weapon->burstCount == 0) {
    bool canFire = true;

    if (weapon->type == WEAPON_MISSILE_LAUNCHER && wc->aimMode == AIM_MODE_CAMERA) {
      canFire = ValidateMissileFire(em, wc, mecha, systems);
    }

    if (canFire) {
      weapon->burstCount = weapon->burstTotal;
      weapon->burstTimer = 0.0f;
      weapon->cooldownTimer = 9999.0f;
    }
  }

  // Process burst fire
  if (weapon->burstCount > 0 && weapon->burstTimer <= 0) {
    Vector3 spawnPos = CalculateProjectileSpawnPosition(em, wc, mecha, weaponID);
    Vector3 direction = CalculateProjectileDirection(em, wc, mecha, spawnPos);

    // Apply missile spread
    if (weapon->type == WEAPON_MISSILE_LAUNCHER) {
      spawnPos.x += ((float)GetRandomValue(-10, 10) / 10.0f) * MISSILE_SPREAD;
      spawnPos.y += ((float)GetRandomValue(-10, 10) / 10.0f) * MISSILE_SPREAD;
    }

    ProcessWeaponFire(systems, mecha, weaponID, weapon, wc, spawnPos, direction);

    weapon->burstCount--;
    weapon->burstTimer = weapon->burstRate;

    if (weapon->burstCount <= 0) {
      weapon->cooldownTimer = weapon->firingRate;
    }
  }
}

void WeaponSystem(struct Systems* systems) {
  const uint32_t mask = COMPONENT_WEAPON_CONTROL;
  EntityManager* em = &systems->entityManager;
  float dt = systems->delta_time;

  for (Entity mecha = 0; mecha < em->numEntities; mecha++) {
    if ((em->componentMasks[mecha] & mask) == mask) {
      WeaponControlComponent* wc = &em->weaponControlComponents[mecha];

      for (int idx = 0; idx < MAX_WEAPONS_EQUIP; idx++) {
        Entity weaponID = wc->weaponsSlots[idx];

        // Empty Slot or not a Weapon
        uint32_t weapon_mask = (COMPONENT_WEAPON | COMPONENT_ATTACHMENT);
        if (weaponID != MAX_ENTITIES && (em->componentMasks[weaponID] & weapon_mask) == weapon_mask) {
          ProcessWeapon(em, systems, mecha, weaponID, idx, dt);
        }
      }
    }
  }
}

// Validates if missile can be fired (target locked and in range)
static bool ValidateMissileFire(EntityManager* em, WeaponControlComponent* wc, Entity mecha, struct Systems* systems) {
  if (wc->lockedTarget >= MAX_ENTITIES) {
    Sound failSound = *GetSound(&systems->resourceManager, SOUND_ID_MISSILE_FAILED);
    float finalVolume = GetAudioVolume(&systems->configManager);
    SetSoundVolume(failSound, finalVolume);
    float pitch = 0.95f + ((float)GetRandomValue(-5, 5) / 100.0f);
    SetSoundPitch(failSound, pitch);
    PlaySound(failSound);
    return false;
  }

  Vector3 myPos = em->transformComponents[mecha].position;
  Vector3 targetPos = em->transformComponents[wc->lockedTarget].position;
  float distSq = Vector3LengthSqr(Vector3Subtract(targetPos, myPos));
  float minRangeSq = MISSILE_MIN_RANGE * MISSILE_MIN_RANGE;

  if (distSq < minRangeSq) {
    Sound failSound = *GetSound(&systems->resourceManager, SOUND_ID_MISSILE_FAILED);
    float finalVolume = GetAudioVolume(&systems->configManager);
    SetSoundVolume(failSound, finalVolume);
    float pitch = 0.95f + ((float)GetRandomValue(-5, 5) / 100.0f);
    SetSoundPitch(failSound, pitch);
    PlaySound(failSound);
    return false;
  }

  return true;
}

// Calculates projectile spawn position from camera using attachment offset
static Vector3 GetCameraMuzzlePosition(Camera* camera, Vector3 localOffset, WeaponType weaponType) {

  // Get forward facing vector
  Vector3 forward = Vector3Subtract(camera->target, camera->position);
  forward = Vector3Normalize(forward);

  // Get right vector using cross product between up vector and forward vector
  Vector3 right = Vector3CrossProduct(forward, (Vector3){0.0f, 1.0f, 0.0f});
  right = Vector3Normalize(right);

  // Guarantee that the up vector is really pointing up to the camera (in case the camera is leaning)
  Vector3 up = Vector3CrossProduct(right, forward);

  // Calculate screen corner offset based on FOV and screen dimensions
  float distance = 15.0f;
  float fovRad = camera->fovy * DEG2RAD;
  float aspect = (float)GetScreenWidth() / (float)GetScreenHeight();
  float halfHeight = tanf(fovRad * 0.5f) * distance;
  float halfWidth = halfHeight * aspect;

  float cornerX;
  if (localOffset.x < 0) cornerX = -halfWidth;
  else cornerX = halfWidth;

  float cornerY;
  if (weaponType == WEAPON_MISSILE_LAUNCHER) cornerY = halfHeight;
  else cornerY = -halfHeight;

  Vector3 spawnPos = camera->position;
  spawnPos = Vector3Add(spawnPos, Vector3Scale(forward, distance));
  spawnPos = Vector3Add(spawnPos, Vector3Scale(right, cornerX));
  spawnPos = Vector3Add(spawnPos, Vector3Scale(up, cornerY));

  return spawnPos;
}

// Calculates projectile spawn position based on aim mode
static Vector3 CalculateProjectileSpawnPosition(EntityManager* em, WeaponControlComponent* wc, Entity mecha, Entity weaponID) {
  Vector3 spawnPos;
  if (wc->aimMode == AIM_MODE_CAMERA) {
    PlayerControlComponent* pc = &em->playerControlComponents[mecha];
    Vector3 offset = em->attachmentComponents[weaponID].offsetPosition;
    WeaponComponent* weapon = &em->weaponComponents[weaponID];
    spawnPos = GetCameraMuzzlePosition(pc->camera, offset, weapon->type);
  }
  else {
    // For AI: use weapon's world position (already updated by AttachmentSystem with rotation)
    spawnPos = em->transformComponents[weaponID].position;
  }
  return spawnPos;
}

static Vector3 CalculateProjectileDirection(EntityManager* em, WeaponControlComponent* wc, Entity mecha, Vector3 spawnPos) {
  if (wc->aimMode == AIM_MODE_CAMERA) {
    PlayerControlComponent* pc = &em->playerControlComponents[mecha];
    Vector3 aimPoint = Vector3Add(pc->camera->position, Vector3Scale(wc->aimDirection, CONVERGENCE_POINT));
    return Vector3Normalize(Vector3Subtract(aimPoint, spawnPos));
  }
  else {
    if (wc->lockedTarget != MAX_ENTITIES) {
        Vector3 targetPos = em->transformComponents[wc->lockedTarget].position;

        targetPos.y += 10.0;
        Vector3 trueDirection = Vector3Subtract(targetPos, spawnPos);
        
        return Vector3Normalize(trueDirection);
    }
    
    return wc->aimDirection;
  }
}

// Processes weapon fire event and spawns projectile
static void ProcessWeaponFire(struct Systems* systems, Entity mecha, Entity weaponID, WeaponComponent* weapon, WeaponControlComponent* wc, Vector3 spawnPos, Vector3 direction) {
  Entity targetToLock = MAX_ENTITIES;
  if (wc->aimMode == AIM_MODE_CAMERA) targetToLock = wc->lockedTarget;

  EventData data;
  data.weaponFired.owner = mecha;
  data.weaponFired.position = spawnPos;
  data.weaponFired.direction = direction;
  data.weaponFired.weapon = weapon->type;

  if (weapon->type == WEAPON_PULSE_LASER) {
    data.weaponFired.projectileEntity = SpawnLaserPulseProjectile(systems, spawnPos, direction, mecha, weapon);
  }
  else if (weapon->type == WEAPON_MISSILE_LAUNCHER) {
    data.weaponFired.projectileEntity = SpawnMissileProjectile(systems, spawnPos, direction, mecha, weapon, targetToLock);
  }

  PushEvent(systems, EVENT_WEAPON_FIRED, data);
}

// Updates weapon cooldown and burst timers
static void UpdateWeaponCooldowns(WeaponComponent* weapon, float dt) {
  if (weapon->cooldownTimer > 0) weapon->cooldownTimer -= dt;
  if (weapon->burstTimer > 0) weapon->burstTimer -= dt;
}

// Factory function to spawn projectiles
static Entity SpawnProjectileCore(struct Systems* systems, Vector3 pos, Vector3 direction, Entity owner, float speed, float damage, WeaponType type) {
  EntityManager* em = &systems->entityManager;

  Entity bullet = CreateEntity(em);
  if (bullet >= MAX_ENTITIES) return MAX_ENTITIES;

  Vector3 modelForward = (Vector3){ 0.0f, 0.0f, -1.0f };
  Vector3 targetDirection = Vector3Normalize(direction);
  Quaternion rotation = QuaternionFromVector3ToVector3(modelForward, targetDirection);

  AddTransformComponent(em, bullet, pos);
  em->transformComponents[bullet].orientation = rotation;

  Vector3 velocity = Vector3Scale(direction, speed);

  if (em->componentMasks[owner] & COMPONENT_PHYSICS) {
    Vector3 ownerVel = em->physicsComponents[owner].velocity;
    velocity = Vector3Add(velocity, ownerVel);
  }

  AddPhysicsComponent(em, bullet, velocity, 0.0f);

  BoundingBox box = (BoundingBox){(Vector3){-0.2f, -0.2f, -0.2f}, (Vector3){0.2f, 0.2f, 0.2f}};
  AddCollisionComponent(em, bullet, box, false, false);

  AddProjectileComponent(em, bullet, owner, damage, true, 0.0f, type);

  return bullet;
}

static Entity SpawnLaserPulseProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats) {
  EntityManager* em = &systems->entityManager;
  ResourceManager* rm = &systems->resourceManager;

  Entity bullet = SpawnProjectileCore(systems, position, direction, owner, stats->projectileSpeed, stats->projectileDamage, stats->type);
  if (bullet >= MAX_ENTITIES) return MAX_ENTITIES;

  Model* bulletModel = GetModel(rm, MODEL_ID_PROJECTILE_PULSE_LASER);
  AddRenderComponent(em, bullet, bulletModel, LASER_BLUE);
  AddLifetimeComponent(em, bullet, LASER_LIFETIME);

  return bullet;
}

static Entity SpawnMissileProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats, Entity target) {
  EntityManager* em = &systems->entityManager;
  ResourceManager* rm = &systems->resourceManager;

  // Javelin effect: launch with upward arc
  Vector3 flatForward = (Vector3){direction.x, 0.0f, direction.z};
  flatForward = Vector3Normalize(flatForward);

  Vector3 upForce = Vector3Scale((Vector3){0, 1, 0}, 0.45f);
  Vector3 fwdForce = Vector3Scale(flatForward, 1.0f);

  Vector3 launchDir = Vector3Add(upForce, fwdForce);
  launchDir = Vector3Normalize(launchDir);

  Entity bullet = SpawnProjectileCore(systems, position, launchDir, owner, stats->projectileSpeed, stats->projectileDamage, stats->type);
  if (bullet == MAX_ENTITIES) return MAX_ENTITIES;

  if (target < MAX_ENTITIES) AddHomingComponent(em, bullet, target, 3.0f, stats->projectileSpeed, 0.8f);

  AddLifetimeComponent(em, bullet, MISSILE_LIFETIME);

  Model* missileModel = GetModel(rm, MODEL_ID_PROJECTILE_MISSILE_LAUNCHER);
  AddRenderComponent(em, bullet, missileModel, ORANGE);

  return bullet;
}
