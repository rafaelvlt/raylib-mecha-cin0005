#include <raylib.h>
#include <raymath.h>
#include "utility.h"
#include "systems.h"
#include "ecs/types.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"

#define CONVERGENCE_POINT 75.0f

// Helper functions
static void SpawnProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats);
static Vector3 GetCameraMuzzlePosition(Camera* camera, Vector3 localOffset);


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

          int group = wc->weaponsGroupMap[idx];

          if (wc->triggerPulled && wc->activeGroup[group] && weapon->cooldownTimer <= 0){
            Vector3 spawnPos = {0};
            Vector3 shootDir = wc->aimDirection;

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


            weapon->cooldownTimer = weapon->firingRate;
            SpawnProjectile(systems, spawnPos, shootDir, mecha, weapon);

            // Event logic
            EventData data;
            data.weaponFired.owner = mecha;
            data.weaponFired.position = spawnPos; 
            data.weaponFired.direction = shootDir;
            data.weaponFired.weapon = weapon->type;
            PushEvent(systems, EVENT_WEAPON_FIRED, data);
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

static void SpawnProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats) {
  EntityManager* em = &systems->entityManager;
  ResourceManager* rm = &systems->resourceManager;

  Entity bullet = CreateEntity(em);
  if (bullet >= MAX_ENTITIES) return;

  // Gives the bullet position and direction, and rotates it in relation to the camera
  // Fixes the problem with bullets facing the same side while leaving the muzzle
  AddTransformComponent(em, bullet, position);
  Vector3 modelForward = { 0.0f, 0.0f, -1.0f }; 
  Vector3 targetDirection = Vector3Normalize(direction);
  Quaternion rotation = QuaternionFromVector3ToVector3(modelForward, targetDirection);
  em->transformComponents[bullet].orientation = rotation;

  // Gives the projectile speed, and increases it by the owner velocity too!
  Vector3 velocity = Vector3Scale(direction, stats->projectileSpeed);
  if (em->componentMasks[owner] & COMPONENT_PHYSICS) {
    Vector3 ownerVel = em->physicsComponents[owner].velocity;
    velocity = Vector3Add(velocity, ownerVel);
  }
  // No air drag for the bullets
  AddPhysicsComponent(em, bullet, velocity, 0.0f);

  // Hitbox
  BoundingBox box = (BoundingBox){(Vector3){-0.2f, -0.2f, -0.2f}, (Vector3){0.2f, 0.2f, 0.2f}};
  AddCollisionComponent(em, bullet, box, false, true);

  //Projectile behaviour
  AddProjectileComponent(em, bullet, owner, stats->projectileDamage, true, 0.0f, (Effect)0, stats->type);

  // Gets model for visuals
  Model* bulletModel = GetModel(rm, stats->projectileModelID);
  AddRenderComponent(em, bullet, bulletModel, LASER_BLUE);
  //Temporary component
  AddLifetimeComponent(em, bullet, 1.5f); 
}
