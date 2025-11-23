#include <raylib.h>
#include <raymath.h>
#include "ecs/entitymanager.h"
#include "ecs/types.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"
#include "utility.h"

static BoundingBox GetWorldBoundingBox(BoundingBox hitbox, Vector3 position){
  BoundingBox worldBox;
  worldBox.min = Vector3Add(hitbox.min, position);
  worldBox.max = Vector3Add(hitbox.max, position);
  return worldBox;
}

static void CollisionProjectileSystem(struct Systems* systems){
  uint32_t projectileMask = COMPONENT_COLLISION | COMPONENT_PROJECTILE | COMPONENT_TRANSFORM;
  
  EntityManager* em = &systems->entityManager;
  
  for (Entity bullet = 0; bullet < em->numEntities; bullet++){
    if ((em->componentMasks[bullet] & projectileMask) ==  projectileMask){
      ProjectileComponent* pj = &em->projectileComponents[bullet];
      TransformComponent* ts = &em->transformComponents[bullet];
      BoundingBox boxProj = GetWorldBoundingBox(em->collisionComponents[bullet].hitbox, ts->position);
      
      uint32_t targetMask = COMPONENT_COLLISION | COMPONENT_TRANSFORM;
      for (Entity target = 0; target < em->numEntities && em->componentMasks[bullet] != COMPONENT_NONE; target++){
        if ((em->componentMasks[target] & targetMask) == targetMask){
          if ((em->componentMasks[target] & COMPONENT_PROJECTILE) == COMPONENT_PROJECTILE) ; //skip
          else if (target == pj->owner) ; // skip
          else{
            TransformComponent* targetTrans = &em->transformComponents[target];
            CollisionComponent* targetCol = &em->collisionComponents[target];
            BoundingBox boxTarget = GetWorldBoundingBox(targetCol->hitbox, targetTrans->position);

            if (CheckCollisionBoxes(boxProj, boxTarget)){

              EventData data;
              data.projectileCollisionDetected.attacker = pj->owner;
              data.projectileCollisionDetected.victim = target;
              data.projectileCollisionDetected.damageAmount = pj->damage;
              data.projectileCollisionDetected.impactPoint = ts->position;
              data.projectileCollisionDetected.type = pj->type;

              PushEvent(systems, EVENT_PROJECTILE_COLLISION, data);

              if (pj->destroyOnHit){
                DestroyEntity(&systems->entityManager, bullet);
              }
            }
          }
          
        }
      }
    }
  }
}

void CollisionSystem(struct Systems* systems){
  CollisionProjectileSystem(systems); 
}
