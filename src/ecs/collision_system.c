#include <raylib.h>
#include <raymath.h>
#include "ecs/entitymanager.h"
#include "ecs/components.h"
#include "ecs/systems.h"
#include "event_manager.h"
#include "systems.h"

// Converts local bounding box to world space
static BoundingBox GetWorldBoundingBox(BoundingBox hitbox, Vector3 position) {
  BoundingBox worldBox;
  worldBox.min = Vector3Add(hitbox.min, position);
  worldBox.max = Vector3Add(hitbox.max, position);
  return worldBox;
}

// Processes collisions between projectiles and other entities
static void CollisionProjectileSystem(struct Systems* systems) {
  uint32_t projectileMask = COMPONENT_COLLISION | COMPONENT_PROJECTILE | COMPONENT_TRANSFORM;
  EntityManager* em = &systems->entityManager;
  uint32_t targetMask = COMPONENT_COLLISION | COMPONENT_TRANSFORM;

  for (Entity bullet = 0; bullet < em->numEntities; bullet++) {
    if ((em->componentMasks[bullet] & projectileMask) == projectileMask && em->componentMasks[bullet] != COMPONENT_NONE) {
      ProjectileComponent* pj = &em->projectileComponents[bullet];
      TransformComponent* ts = &em->transformComponents[bullet];

      for (Entity target = 0; target < em->numEntities; target++) {
        if ((em->componentMasks[target] & targetMask) == targetMask) {
          // Skip projectiles and owner
          if ((em->componentMasks[target] & COMPONENT_PROJECTILE) != COMPONENT_PROJECTILE && target != pj->owner) {

            // Check collision
            TransformComponent* targetTrans = &em->transformComponents[target];
            CollisionComponent* targetCol = &em->collisionComponents[target];
            BoundingBox boxTarget = GetWorldBoundingBox(targetCol->hitbox, targetTrans->position);
            BoundingBox boxProj = GetWorldBoundingBox(em->collisionComponents[bullet].hitbox, ts->position);

            if (CheckCollisionBoxes(boxProj, boxTarget)) {
              EventData data;
              data.projectileCollisionDetected.attacker = pj->owner;
              data.projectileCollisionDetected.victim = target;
              data.projectileCollisionDetected.damageAmount = pj->damage;
              data.projectileCollisionDetected.impactPoint = ts->position;
              data.projectileCollisionDetected.type = pj->type;

              PushEvent(systems, EVENT_PROJECTILE_COLLISION, data);

              if (pj->destroyOnHit) {
                DestroyEntity(em, bullet);
              }
            }
          }
        }
      }
    }
  }
}

void CollisionSystem(struct Systems* systems) {
  CollisionProjectileSystem(systems);
}
