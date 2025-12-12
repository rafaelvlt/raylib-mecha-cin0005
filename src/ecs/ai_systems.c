#include "systems.h"
#include "ecs/entitymanager.h"
#include "ecs/types.h"
#include "resource_manager.h"
#include <raymath.h>

#define AI_TURN_SPEED                1.5f
#define AI_MOVE_SPEED                12.5f
#define AI_FOV_THRESHOLD             0.5f
#define AI_AIM_ERROR                 0.1f
#define AI_DAMAGE_FACING_THRESHOLD   0.7f

// Helper functions
static Entity FindPlayer(EntityManager* em, Vector3* outPos);
static bool CanSeePlayer(AIControlComponent* ai, TransformComponent* transform, Vector3 targetPos, float dist);
static void UpdateAIState(EntityManager* em, Entity entity, AIControlComponent* ai, Vector3 targetPos, float dist, bool canSeePlayer);
static void ProcessAIState(EntityManager* em, ResourceManager* rm, Entity entity, AIControlComponent* ai, TransformComponent* transform, PhysicsComponent* phys, AnimationComponent* anim, WeaponControlComponent* wc, Vector3 targetPos, Entity playerID, float dt);
static void AiMovementControl(Entity entity, EntityManager* em, ResourceManager* rm, TransformComponent* trans, PhysicsComponent* phys, AnimationComponent* anim, Vector3 targetPos, float speed, float dt);
static int CalculateDamageReactionAnimation(TransformComponent* trans, HealthComponent* health);
static void SetIdleOrWalkAnimation(AnimationComponent* anim, PhysicsComponent* phys);
static bool HandleDamageReactionAnimations(Entity entity, EntityManager* em, ResourceManager* rm, TransformComponent* trans, PhysicsComponent* phys, AnimationComponent* anim, HealthComponent* health, float dt);

// Finds player entity and returns its position
static Entity FindPlayer(EntityManager* em, Vector3* outPos) {
  for (int i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
      *outPos = em->transformComponents[i].position;
      return i;
    }
  }
  *outPos = (Vector3){0, 0, 0};
  return MAX_ENTITIES;
}

static bool CanSeePlayer(AIControlComponent* ai, TransformComponent* transform, Vector3 targetPos, float dist) {
  if (dist > ai->sightRadius) return false;
  if (ai->state != AI_STATE_PATROL) return true; // In chase/attack, can always "see"

  Vector3 toPlayer = Vector3Normalize(Vector3Subtract(targetPos, transform->position));
  Vector3 forward = Vector3RotateByQuaternion((Vector3){0,0,-1}, transform->orientation);
  return Vector3DotProduct(forward, toPlayer) > AI_FOV_THRESHOLD;
}

static void UpdateAIState(EntityManager* em, Entity entity, AIControlComponent* ai, Vector3 targetPos, float dist, bool canSeePlayer) {
  HealthComponent* health = &em->healthComponents[entity];
  
  switch (ai->state) {
    case AI_STATE_PATROL:
      // Update damage state
      health->hasTakenDamage = (health->currentHealth < health->maxHealth && dist <= ai->sightRadius);
      // Start chase if can see player or took damage
      if (canSeePlayer || (health->hasTakenDamage && dist <= ai->sightRadius))
        ai->state = AI_STATE_CHASE;
      break;
      
    case AI_STATE_CHASE:
      if (dist <= ai->attackRange) ai->state = AI_STATE_ATTACK;
      else if (dist > ai->sightRadius * 1.6f) ai->state = AI_STATE_PATROL;
      break;
      
    case AI_STATE_ATTACK:
      if (dist > ai->attackRange * 1.2f) ai->state = AI_STATE_CHASE;
      break;
  }
}


static void ProcessAIState(EntityManager* em, ResourceManager* rm, Entity entity, AIControlComponent* ai, TransformComponent* transform, PhysicsComponent* phys, AnimationComponent* anim, WeaponControlComponent* wc, Vector3 targetPos, Entity playerID, float dt) {
  switch (ai->state) {
    case AI_STATE_PATROL: {
      if (ai->numPatrolPoints > 0 && ai->patrolPoints != NULL) {
        // Clamp patrol index to valid range
        if (ai->currentPatrolIndex < 0 || ai->currentPatrolIndex >= ai->numPatrolPoints)
          ai->currentPatrolIndex = 0;

        Vector3 patrolTarget = ai->patrolPoints[ai->currentPatrolIndex];
        Vector3 toPatrol = Vector3Subtract(patrolTarget, transform->position);
        toPatrol.y = 0;
        
        if (Vector3LengthSqr(toPatrol) > 25.0f) // 5.0f squared
          AiMovementControl(entity, em, rm, transform, phys, anim, patrolTarget, AI_MOVE_SPEED * 0.5f, dt);
        else {
          phys->velocity = Vector3Lerp(phys->velocity, Vector3Zero(), dt * 5.0f);
          if (anim && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK) && anim->currentAnim != SCOUT_ANIM_IDLE) {
            anim->currentAnim = SCOUT_ANIM_IDLE;
            anim->currentTime = 0.0f;
          }
          ai->currentPatrolIndex = (ai->currentPatrolIndex + 1) % ai->numPatrolPoints;
        }
      }
      else {
        phys->velocity = Vector3Lerp(phys->velocity, Vector3Zero(), dt * 5.0f);
        if (anim && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK) && anim->currentAnim != SCOUT_ANIM_IDLE) {
          anim->currentAnim = SCOUT_ANIM_IDLE;
          anim->currentTime = 0.0f;
        }
      }
      
      wc->triggerPulled = false;
      break;
    }
    
    case AI_STATE_CHASE:
      AiMovementControl(entity, em, rm, transform, phys, anim, targetPos, AI_MOVE_SPEED, dt);
      break;
    
    case AI_STATE_ATTACK: {
      phys->velocity = Vector3Lerp(phys->velocity, Vector3Zero(), dt * 5.0f);

      HealthComponent* health = NULL;
      if (em->componentMasks[entity] & COMPONENT_HEALTH) {
        health = &em->healthComponents[entity];
        if (health->damageReactionTimer > 0.0f) {
          health->damageReactionTimer -= dt;
          if (health->damageReactionTimer < 0.0f) health->damageReactionTimer = 0.0f;
        }
      }

      if (anim) {
        bool isInDamageReaction = HandleDamageReactionAnimations(entity, em, rm, transform, phys, anim, health, dt);
        if (!isInDamageReaction && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK))
          SetIdleOrWalkAnimation(anim, phys);
      }

      Vector3 toPlayer = Vector3Subtract(targetPos, transform->position);
      if (Vector3LengthSqr(toPlayer) > 0.001f) {
        Vector3 currentFwd = Vector3RotateByQuaternion((Vector3){0,0,-1}, transform->orientation);
        Vector3 desiredDir = Vector3Normalize(toPlayer);
        Vector3 newDir = Vector3Lerp(currentFwd, desiredDir, AI_TURN_SPEED * dt);
        if (Vector3LengthSqr(newDir) > 0.001f)
          transform->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, Vector3Normalize(newDir));
      }

      Vector3 error = {
        (float)GetRandomValue(-5,5) * 0.01f * AI_AIM_ERROR,
        (float)GetRandomValue(-5,5) * 0.01f * AI_AIM_ERROR,
        (float)GetRandomValue(-5,5) * 0.01f * AI_AIM_ERROR
      };
      wc->aimDirection = Vector3Normalize(Vector3Add(toPlayer, error));
      wc->triggerPulled = true;
      wc->lockedTarget = playerID;
      break;
    }
    
    default:
      phys->velocity = Vector3Zero();
      if (anim && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK) && anim->currentAnim != SCOUT_ANIM_IDLE) {
        anim->currentAnim = SCOUT_ANIM_IDLE;
        anim->currentTime = 0.0f;
      }
      break;
  }
}

void AIControlSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  float dt = systems->delta_time;

  Vector3 targetPos;
  Entity playerID = FindPlayer(em, &targetPos);
  if (playerID == MAX_ENTITIES || Vector3LengthSqr(targetPos) < 0.001f) return;

  uint32_t aiMask = COMPONENT_AI_CONTROL | COMPONENT_TRANSFORM | COMPONENT_PHYSICS;
  for (int e = 0; e < em->numEntities; e++) {
    if ((em->componentMasks[e] & aiMask) != aiMask) continue;

    AIControlComponent* ai = &em->aiControlComponents[e];
    TransformComponent* transform = &em->transformComponents[e];
    PhysicsComponent* phys = &em->physicsComponents[e];
    AnimationComponent* anim = (em->componentMasks[e] & COMPONENT_ANIMATION) ? &em->animationComponents[e] : NULL;
    WeaponControlComponent* wc = &em->weaponControlComponents[e];
    
    wc->triggerPulled = false;

    float dist = Vector3Distance(transform->position, targetPos);
    bool canSeePlayer = CanSeePlayer(ai, transform, targetPos, dist);

    // Turn towards player if in patrol and within sight radius but not facing them
    if (ai->state == AI_STATE_PATROL && dist <= ai->sightRadius) {
      Vector3 toPlayer = Vector3Normalize(Vector3Subtract(targetPos, transform->position));
      if (Vector3LengthSqr(toPlayer) > 0.001f) {
        Vector3 currentFwd = Vector3RotateByQuaternion((Vector3){0,0,-1}, transform->orientation);
        if (Vector3DotProduct(currentFwd, toPlayer) < AI_FOV_THRESHOLD) {
          Vector3 desiredDir = Vector3Normalize(toPlayer);
          Vector3 newDir = Vector3Lerp(currentFwd, desiredDir, AI_TURN_SPEED * dt);
          if (Vector3LengthSqr(newDir) > 0.001f)
            transform->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, Vector3Normalize(newDir));
        }
      }
    }

    UpdateAIState(em, e, ai, targetPos, dist, canSeePlayer);
    ProcessAIState(em, &systems->resourceManager, e, ai, transform, phys, anim, wc, targetPos, playerID, dt);
  }
}

// Controls AI entity movement, handles damage reactions, arrival detection and animation updates
static void AiMovementControl(Entity entity, EntityManager* em, ResourceManager* rm, TransformComponent* trans, PhysicsComponent* phys, AnimationComponent* anim,
                              Vector3 targetPos, float speed, float dt) {
  Vector3 myPos = trans->position;
  Vector3 toTarget = Vector3Subtract(targetPos, myPos);

  // Ignore Y component for ground-based movement (only move horizontally)
  toTarget.y = 0;

  // Update damage reaction timer if entity has health component
  HealthComponent* health = NULL;
  if (em->componentMasks[entity] & COMPONENT_HEALTH) {
    health = &em->healthComponents[entity];
    if (health->damageReactionTimer > 0.0f) {
      health->damageReactionTimer -= dt;
      if (health->damageReactionTimer < 0.0f) health->damageReactionTimer = 0.0f;
    }
  }

  // Check if entity is in damage reaction animation
  if (anim) {
    bool isInDamageReaction = HandleDamageReactionAnimations(entity, em, rm, trans, phys, anim, health, dt);
    if (isInDamageReaction) {
      return;  // Don't move during damage reaction
    }
  }

  // Check if close enough to target
  if (Vector3LengthSqr(toTarget) < 4.0f) {
    phys->velocity = Vector3Zero();
    if (anim && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK) && anim->currentAnim != SCOUT_ANIM_IDLE) {
      anim->currentAnim = SCOUT_ANIM_IDLE;
      anim->currentTime = 0.0f;
    }
    return;
  }

  // Calculate desired direction 
  Vector3 desiredDir = Vector3Normalize(toTarget);
  
  // if direction is invalid (zero vector), abort
  if (Vector3LengthSqr(desiredDir) < 0.001f) return;

  // Get current forward direction 
  Vector3 currentForward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);

  // Smooth change of direction
  Vector3 newDir = Vector3Lerp(currentForward, desiredDir, AI_TURN_SPEED * dt);
  if (Vector3LengthSqr(newDir) < 0.001f) newDir = currentForward;
  
  // Lerp doesn't preserve length
  newDir = Vector3Normalize(newDir);

  // Update entity orientation to face new direction
  trans->orientation = QuaternionFromVector3ToVector3((Vector3){0, 0, -1}, newDir);

  // Dot product: 1.0 = perfectly aligned, 0.0 = perpendicular, -1.0 = facing away
  float alignment = Vector3DotProduct(currentForward, desiredDir);

  // Only move forward if facing target (alignment > 0)
  float throttle;
  if (alignment > 0.0f) throttle = alignment;
  else throttle = 0.0f;

  // Using alligment as speed if it's good enough
  if (alignment > 0.5f) {
    throttle = alignment; 
  }

  // Apply velocity: direction * speed * throttle
  phys->velocity = Vector3Scale(newDir, speed * throttle);

  // Update animation based on movement speed
  if (anim && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK))
    SetIdleOrWalkAnimation(anim, phys);
}

// Determines which damage reaction animation to play based on incoming damage direction
static int CalculateDamageReactionAnimation(TransformComponent* trans, HealthComponent* health) {
  if (!health) return SCOUT_ANIM_IDLE;

  // Get entity right vector in world space by rotating local X axis (1,0,0)
  Vector3 right = Vector3RotateByQuaternion((Vector3){1, 0, 0}, trans->orientation);

  // Magnitude indicates how lateral the damage is (closer to 1 = more from side)
  float rightDot = Vector3DotProduct(right, health->lastDamageDirection);

  // Damage from side
  if (fabsf(rightDot) > 0.5f) {
    if (rightDot > 0.0f) return SCOUT_ANIM_DAMAGE_RIGHT;
    else  return SCOUT_ANIM_DAMAGE_LEFT;       

  }
  // Damage from front
  else {
    if (rightDot > 0.0f) return SCOUT_ANIM_DAMAGE_FRONT_RIGHT;
    else return SCOUT_ANIM_DAMAGE_FRONT_LEFT;
  }
}

// Automatically switches between idle and walk animations based on movement speed
static void SetIdleOrWalkAnimation(AnimationComponent* anim, PhysicsComponent* phys) {
  if (!anim || !phys) return;

  // speedSq = vx² + vy² + vz²
  float speedSq = Vector3LengthSqr(phys->velocity);

  int desiredAnim; 
  if (speedSq > 1.0f) {
    desiredAnim = SCOUT_ANIM_WALK; 
  }
  else {
    desiredAnim = SCOUT_ANIM_IDLE;
  }

  // Only switch if animation needs to change (prevents unnecessary updates)
  if (anim->currentAnim != desiredAnim) {
    anim->currentAnim = desiredAnim;
    // Reset animation time when switching (start from beginning)
    anim->currentTime = 0.0f;  
  }
}

// Manages damage reaction animations with priority over normal animations (idle/walk)
static bool HandleDamageReactionAnimations(Entity entity, EntityManager* em, ResourceManager* rm, TransformComponent* trans, 
                                          PhysicsComponent* phys, AnimationComponent* anim, HealthComponent* health, float dt) {
  if (!anim) return false;

  // Check if currently playing a damage reaction animation
  if (health && health->damageReactionTimer > 0.0f && 
      anim->currentAnim >= SCOUT_ANIM_DAMAGE_RIGHT && anim->currentAnim <= SCOUT_ANIM_DAMAGE_FRONT_LEFT) {

    // Check if animation has finished playing
    AssetModelID modelId = anim->modelId;
    if (modelId >= 0 && modelId < MODEL_ID_COUNT) {
      ModelAnimation* animations = rm->modelAnimations[modelId];
      int animCount = rm->modelAnimCounts[modelId];
      
      if (animations && animCount > 0 && anim->currentAnim < animCount) {
        ModelAnimation currentAnim = animations[anim->currentAnim];
        float totalFrames = (float)currentAnim.frameCount;
        
        // If animation has finished (currentTime >= totalFrames), return to normal animation
        if (anim->currentTime >= totalFrames) {
          SetIdleOrWalkAnimation(anim, phys);
          health->damageReactionTimer = 0.0f;
          return false;
        }
      }
    }

    // Check if entity has turned to face the damage source
    Vector3 forward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);
    float facingDot = Vector3DotProduct(forward, Vector3Negate(health->lastDamageDirection));

    // If entity is facing the damage source, cancel reaction
    if (facingDot > AI_DAMAGE_FACING_THRESHOLD) {
      SetIdleOrWalkAnimation(anim, phys);
      health->damageReactionTimer = 0.0f;
      return false;
    }
    
    // Continue playing damage reaction 
    return true;
  }

  // Trigger new damage reaction 
  if (health && health->hasTakenDamage && health->damageReactionTimer > 0.0f &&
      (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK)) {

    // Calculate which damage reaction animation to play based on damage direction
    int reactionAnim = CalculateDamageReactionAnimation(trans, health);

    // Switch to damage reaction animation
    anim->currentAnim = reactionAnim;
    anim->currentTime = 0.0f;

    // Stop movement during reaction (stun)
    phys->velocity = Vector3Zero();

    return true;
  }

  // Not in damage reaction
  return false;
}
