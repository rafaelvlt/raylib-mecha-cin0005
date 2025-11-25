#include <raylib.h>
#include <raymath.h>
#include "resource_manager.h"
#include "utility.h"
#include "systems.h"
#include "ecs/types.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"

#define CONVERGENCE_POINT 75.0f

// Helper functions
static Vector3 GetCameraMuzzlePosition(Camera* camera, Vector3 localOffset);

// Factory functions
static Entity SpawnProjectileCore(struct Systems* systems, Vector3 pos, Vector3 direction, Entity owner, float speed, float damage, WeaponType type);
static Entity SpawnLaserPulseProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats);
static Entity SpawnMissileProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats, Entity target);

void WeaponSystem(struct Systems* systems){
  const uint32_t mask = COMPONENT_WEAPON_CONTROL;
  EntityManager* em = &systems->entityManager;

  float dt = systems->delta_time;
  for (Entity mecha = 0; mecha < em->numEntities; mecha++){
    if((em->componentMasks[mecha] & mask) == mask){

      WeaponControlComponent* wc = &em->weaponControlComponents[mecha];

      for (int idx = 0; idx < MAX_WEAPONS_EQUIP; idx++){
        Entity WeaponID = wc->weaponsSlots[idx];

        // Empty Slot or not a Weapon
        uint32_t weapon_mask = (COMPONENT_WEAPON | COMPONENT_ATTACHMENT);
        if (WeaponID == MAX_ENTITIES || (em->componentMasks[WeaponID] & (weapon_mask)) != weapon_mask);
        else{
          WeaponComponent* weapon = &em->weaponComponents[WeaponID];

          if (weapon->cooldownTimer > 0) weapon->cooldownTimer -= dt;
          if (weapon->burstTimer > 0) weapon->burstTimer -= dt;

          int group = wc->weaponsGroupMap[idx];

          if (wc->triggerPulled && wc->activeGroup[group] && weapon->cooldownTimer <= 0 && weapon->burstCount == 0){

            bool canFire = true;

            if (weapon->type == WEAPON_MISSILE_LAUNCHER) {
              if (wc->aimMode == AIM_MODE_CAMERA) {
                Sound failSound = *GetSound(&systems->resourceManager, SOUND_ID_MISSILE_FAILED); 
                float finalVolume = systems->configManager.audioVolume;
                SetSoundVolume(failSound, finalVolume);
                float pitch = 0.95f + ((float)GetRandomValue(-5, 5) / 100.0f);
                SetSoundPitch(failSound, pitch);
                if (wc->lockedTarget >= MAX_ENTITIES) {
                  canFire = false;                
                  weapon->cooldownTimer = 0.5;
                  PlaySound(failSound);
                }
                else{
                  Vector3 myPos = em->transformComponents[mecha].position;
                  Vector3 targetPos = em->transformComponents[wc->lockedTarget].position;

                  float distSq = Vector3LengthSqr(Vector3Subtract(targetPos, myPos));
                  const float MIN_RANGE = 100.0f;

                  if (distSq < (MIN_RANGE * MIN_RANGE)) {
                    canFire = false;
                    weapon->cooldownTimer = 0.5f;
                    PlaySound(failSound);
                  }
                }
              }
            }
            if (canFire) {
              weapon->burstCount = weapon->burstTotal; 
              weapon->burstTimer = 0.0f;
              weapon->cooldownTimer = 9999.0f;
            }
          }
          if (weapon->burstCount > 0 && weapon->burstTimer <= 0){

            Vector3 spawnPos = {0};
            Vector3 shootDir = wc->aimDirection;
            weapon->cooldownTimer = weapon->firingRate;

            if (wc->aimMode == AIM_MODE_CAMERA){
              PlayerControlComponent* pc = &em->playerControlComponents[mecha];
              Vector3 offset = em->attachmentComponents[WeaponID].offsetPosition;

              spawnPos = GetCameraMuzzlePosition(pc->camera, offset);
              Vector3 aimPoint = Vector3Add(pc->camera->position, Vector3Scale(wc->aimDirection, CONVERGENCE_POINT));
              shootDir = Vector3Normalize(Vector3Subtract(aimPoint, spawnPos));
            }
            else{
              spawnPos = em->transformComponents[WeaponID].position;
            }

            Entity targetToLock = MAX_ENTITIES;

            if (wc->aimMode == AIM_MODE_CAMERA) {
              targetToLock = wc->lockedTarget;
            }

            if (weapon->type == WEAPON_MISSILE_LAUNCHER) {
              float spread = 0.8f; 
              spawnPos.x += ((float)GetRandomValue(-10, 10)/10.0f) * spread;
              spawnPos.y += ((float)GetRandomValue(-10, 10)/10.0f) * spread;
            }


            // Event logic
            EventData data;
            data.weaponFired.owner = mecha;
            data.weaponFired.position = spawnPos; 
            data.weaponFired.direction = shootDir;
            data.weaponFired.weapon = weapon->type; 
            if (weapon->type == WEAPON_PULSE_LASER){ 
              data.weaponFired.projectileEntity = SpawnLaserPulseProjectile(systems, spawnPos, shootDir, mecha, weapon);
            }
            else if (weapon->type == WEAPON_MISSILE_LAUNCHER){
              data.weaponFired.projectileEntity = SpawnMissileProjectile(systems, spawnPos, shootDir, mecha, weapon, targetToLock);
            }
            PushEvent(systems, EVENT_WEAPON_FIRED, data);


            weapon->burstCount--;

            weapon->burstTimer = weapon->burstRate; 

            if (weapon->burstCount <= 0) {
              weapon->cooldownTimer = weapon->firingRate; }
          }
        }
      }
    }
  }
}


/* ----------------------------------------
 * Calculate where the gunshot shoud leave based on camera
 * --------------------------------------- */
static Vector3 GetCameraMuzzlePosition(Camera* camera, Vector3 localOffset) {
  // Fixes offset to camera height
  localOffset.y -= MECH_HEIGHT; 
  // Get Forward facing vector(G.A B - A = line from point B to A)
  Vector3 forward = Vector3Subtract(camera->target, camera->position);
  forward = Vector3Normalize(forward);

  // Get right vector doing a Cross product between the up vector and forward vector
  Vector3 right = Vector3CrossProduct(forward, (Vector3){0.0f, 1.0f, 0.0f});
  right = Vector3Normalize(right);

  // Garantee that the up vector is really pointing up to the camera(in case the camera is leaning)
  Vector3 up = Vector3CrossProduct(right, forward);

  // Position = Camera + (right * offX) + (up * OffY) + (forward * OffZ)
  Vector3 spawnPos = camera->position;
  spawnPos = Vector3Add(spawnPos, Vector3Scale(right, localOffset.x));
  spawnPos = Vector3Add(spawnPos, Vector3Scale(up, localOffset.y));
  spawnPos = Vector3Add(spawnPos, Vector3Scale(forward, localOffset.z));

  return spawnPos;
}

/* -------------------------------------
 * Factory function to spawn projectiles
 * ------------------------------------- */
static Entity SpawnProjectileCore(struct Systems* systems, Vector3 pos, Vector3 direction, Entity owner, float speed, float damage, WeaponType type) {
  EntityManager* em = &systems->entityManager;

  Entity bullet = CreateEntity(em);
  if (bullet >= MAX_ENTITIES) return MAX_ENTITIES;

  Vector3 modelForward = { 0.0f, 0.0f, -1.0f }; 
  Vector3 targetDirection = Vector3Normalize(direction);
  Quaternion rotation = QuaternionFromVector3ToVector3(modelForward, targetDirection);

  AddTransformComponent(em, bullet, pos);
  em->transformComponents[bullet].orientation = rotation;

  Vector3 velocity = Vector3Scale(direction, speed);

  if (em->componentMasks[owner] & COMPONENT_PHYSICS) {
    Vector3 ownerVel = em->physicsComponents[owner].velocity;
    velocity = Vector3Add(velocity, ownerVel); // Adiciona velocidade do robô à bala
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
  //Temporary component
  AddLifetimeComponent(em, bullet, 2.5f); 
  return bullet;
}

static Entity SpawnMissileProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats, Entity target){

  EntityManager* em = &systems->entityManager;
  ResourceManager* rm = &systems->resourceManager;

  // Javellin Effect 
  Vector3 flatForward = (Vector3){direction.x, 0.0f, direction.z};
  flatForward = Vector3Normalize(flatForward);

  Vector3 upForce  = Vector3Scale((Vector3){0,1,0}, 0.45f); 
  Vector3 fwdForce = Vector3Scale(flatForward, 1.0f);

  Vector3 launchDir = Vector3Add(upForce, fwdForce);
  launchDir = Vector3Normalize(launchDir);

  Entity bullet = SpawnProjectileCore(systems, position, launchDir, owner, stats->projectileSpeed, stats->projectileDamage, stats->type);
  if (bullet == MAX_ENTITIES) return MAX_ENTITIES;


  if (target < MAX_ENTITIES) {
    AddHomingComponent(em, bullet, target, 3.0f, stats->projectileSpeed, 0.8f);
  }


  AddLifetimeComponent(em, bullet, 7.5f); 

  Model* missileModel = GetModel(rm, MODEL_ID_PROJECTILE_MISSILE_LAUNCHER);
  AddRenderComponent(em, bullet, missileModel, ORANGE);

  return bullet;
}


