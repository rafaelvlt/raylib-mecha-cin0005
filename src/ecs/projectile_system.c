#include <raylib.h>
#include <raymath.h>
#include "resource_manager.h"
#include "utility.h"
#include "systems.h"
#include "ecs/types.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"

void MissileSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;

  uint32_t mask = COMPONENT_HOMING | COMPONENT_PHYSICS | COMPONENT_TRANSFORM; float dt = systems->delta_time;

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {

      HomingComponent* homing = &em->homingComponents[i];
      PhysicsComponent* phys = &em->physicsComponents[i];
      TransformComponent* trans = &em->transformComponents[i];

      // Arming time logic, waits for a bit before going homing 
      if (homing->timer < homing->armingTime) {
        homing->timer += dt;
        
        // Javellin Effect: Goes up before arming
        phys->velocity.y += 15.0f * dt;
        if (Vector3LengthSqr(phys->velocity) > 0.1f) {
          Vector3 currentDir = Vector3Normalize(phys->velocity);
          trans->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, currentDir);
        }
      }
      else{
        bool targetInvalid = false;
        if (homing->target >= MAX_ENTITIES) targetInvalid = true;
        else if (em->componentMasks[homing->target] == COMPONENT_NONE) targetInvalid = true; // Alvo morreu

        // if target gets invalid(dies or anything else) takes out homing component
        if (targetInvalid) {
          em->componentMasks[i] &= ~COMPONENT_HOMING;
          phys->velocity.y -= 15.0f;
        }
        else{

          // Chase math
          Vector3 myPos = trans->position;
          Vector3 targetPos = em->transformComponents[homing->target].position;

          //TBD Aiming at the center of mass
          targetPos.y += 2.0f; 

          // Wanted vector(straight line to the target)
          Vector3 desiredDir = Vector3Normalize(Vector3Subtract(targetPos, myPos));

          // Current vector
          float speed = Vector3Length(phys->velocity);
          if (speed < 0.1f) speed = homing->speed; 

          Vector3 currentDir = Vector3Scale(phys->velocity, 1.0f / speed);

          // Suavization of the movement
          Vector3 newDir = Vector3Lerp(currentDir, desiredDir, homing->turnSpeed * dt);
          newDir = Vector3Normalize(newDir);

          // Applying everything
          phys->velocity = Vector3Scale(newDir, homing->speed);
          trans->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, newDir);
        }
      }
    }
  }
}

