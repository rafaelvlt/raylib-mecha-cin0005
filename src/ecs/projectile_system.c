#include <raylib.h>
#include <raymath.h>
#include "resource_manager.h"
#include "systems.h"
#include "ecs/types.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"
#include "event_manager.h"


// Forward declarations
static void ProcessMissileArming(EntityManager* em, Entity entity, HomingComponent* homing, PhysicsComponent* phys, TransformComponent* trans, float dt);
static bool IsTargetValid(EntityManager* em, Entity target);
static void ExplodeMissileMidAir(struct Systems* systems, Entity entity, TransformComponent* trans);
static Vector3 GetTargetCenterPosition(EntityManager* em, Entity target);
static void ProcessMissileHoming(struct Systems* systems, EntityManager* em, Entity entity, HomingComponent* homing, PhysicsComponent* phys, TransformComponent* trans, float dt);

// Time before seeks target
static void ProcessMissileArming(EntityManager* em, Entity entity, HomingComponent* homing, PhysicsComponent* phys, TransformComponent* trans, float dt) {
  homing->timer += dt;
  phys->velocity.y += 15.0f * dt;
  if (Vector3LengthSqr(phys->velocity) > 0.1f) {
    Vector3 currentDir = Vector3Normalize(phys->velocity);
    trans->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, currentDir);
  }
}

static bool IsTargetValid(EntityManager* em, Entity target) {
  if (target >= MAX_ENTITIES) return false;
  if (em->componentMasks[target] == COMPONENT_NONE) return false;
  return true;
}

// If target stops being valid
static void ExplodeMissileMidAir(struct Systems* systems, Entity entity, TransformComponent* trans) {
  EntityManager* em = &systems->entityManager;
  EventData data;
  data.projectileCollisionDetected.attacker = MAX_ENTITIES;
  data.projectileCollisionDetected.victim = MAX_ENTITIES;
  data.projectileCollisionDetected.damageAmount = 0.0f;
  data.projectileCollisionDetected.impactPoint = trans->position;
  data.projectileCollisionDetected.type = WEAPON_MISSILE_LAUNCHER;
  PushEvent(systems, EVENT_PROJECTILE_COLLISION, data);
  DestroyEntity(em, entity);
}

// Better aiming and hitting
static Vector3 GetTargetCenterPosition(EntityManager* em, Entity target) {
  TransformComponent* targetTrans = &em->transformComponents[target];
  Vector3 targetPos = targetTrans->position;

  if (em->componentMasks[target] & COMPONENT_COLLISION) {
    CollisionComponent* targetCol = &em->collisionComponents[target];
    BoundingBox worldBox;
    worldBox.min = Vector3Add(targetCol->hitbox.min, targetTrans->position);
    worldBox.max = Vector3Add(targetCol->hitbox.max, targetTrans->position);
    targetPos = Vector3Scale(Vector3Add(worldBox.min, worldBox.max), 0.5f);
  }

  return targetPos;
}

// Seeking property
static void ProcessMissileHoming(struct Systems* systems, EntityManager* em, Entity entity, HomingComponent* homing, PhysicsComponent* phys, TransformComponent* trans, float dt) {
  if (!IsTargetValid(em, homing->target)) {
    ExplodeMissileMidAir(systems, entity, trans);
    return;
  }

  Vector3 myPos = trans->position;
  Vector3 targetPos = GetTargetCenterPosition(em, homing->target);

  Vector3 desiredDir = Vector3Normalize(Vector3Subtract(targetPos, myPos));

  float speed = Vector3Length(phys->velocity);
  if (speed < 0.1f) speed = homing->speed;

  Vector3 currentDir = Vector3Scale(phys->velocity, 1.0f / speed);
  Vector3 newDir = Vector3Lerp(currentDir, desiredDir, homing->turnSpeed * dt);
  newDir = Vector3Normalize(newDir);

  phys->velocity = Vector3Scale(newDir, homing->speed);
  trans->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, newDir);
}

void MissileSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  uint32_t mask = COMPONENT_HOMING | COMPONENT_PHYSICS | COMPONENT_TRANSFORM;
  float dt = systems->delta_time;

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {
      HomingComponent* homing = &em->homingComponents[i];
      PhysicsComponent* phys = &em->physicsComponents[i];
      TransformComponent* trans = &em->transformComponents[i];

      if (homing->timer < homing->armingTime) {
        ProcessMissileArming(em, i, homing, phys, trans, dt);
      }
      else {
        ProcessMissileHoming(systems, em, i, homing, phys, trans, dt);
      }
    }
  }
}

