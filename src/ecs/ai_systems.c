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

static Vector3 FindPlayerPosition(EntityManager* em);
static bool CanSeePlayer(EntityManager* em, AIControlComponent* ai, TransformComponent* transform, Vector3 targetPos, float dist);
static void UpdateAIState(EntityManager* em, Entity entity, AIControlComponent* ai, TransformComponent* transform, Vector3 targetPos, float dist, bool canSeePlayer);
static void ProcessAIState(EntityManager* em, Entity entity, AIControlComponent* ai, TransformComponent* transform, PhysicsComponent* phys, AnimationComponent* anim, WeaponControlComponent* wc, Vector3 targetPos, Entity playerID, float dt);
static void AiMovementControl(Entity entity, EntityManager* em, TransformComponent* trans, PhysicsComponent* phys, AnimationComponent* anim,
                              Vector3 targetPos, float speed, float dt);
static void UpdateDamageReactionTimer(HealthComponent* health, float dt);
static int CalculateDamageReactionAnimation(TransformComponent* trans, HealthComponent* health);
static void SetIdleOrWalkAnimation(AnimationComponent* anim, PhysicsComponent* phys);
static bool HandleDamageReactionAnimations(Entity entity, EntityManager* em, TransformComponent* trans, 
                                           PhysicsComponent* phys, AnimationComponent* anim, HealthComponent* health, float dt);

static Vector3 FindPlayerPosition(EntityManager* em) {
  for (int i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) 
      return em->transformComponents[i].position;
  }
  return (Vector3){0, 0, 0};
}

static bool CanSeePlayer(EntityManager* em, AIControlComponent* ai, TransformComponent* transform, Vector3 targetPos, float dist) {
  if (dist > ai->sightRadius) return false;

  Vector3 toPlayer = Vector3Normalize(Vector3Subtract(targetPos, transform->position));
  Vector3 forward = Vector3RotateByQuaternion((Vector3){0,0,-1}, transform->orientation);
  float dot = Vector3DotProduct(forward, toPlayer);

  return (ai->state != AI_STATE_PATROL || dot > AI_FOV_THRESHOLD);
}

static void UpdateAIState(EntityManager* em, Entity entity, AIControlComponent* ai, TransformComponent* transform, Vector3 targetPos, float dist, bool canSeePlayer) {
  if (ai->state == AI_STATE_PATROL) {
    if (canSeePlayer || (em->healthComponents[entity].hasTakenDamage && dist <= ai->sightRadius)) {
      ai->state = AI_STATE_CHASE;
    }
    
    if (em->healthComponents[entity].currentHealth < em->healthComponents[entity].maxHealth && dist <= ai->sightRadius) {
      em->healthComponents[entity].hasTakenDamage = true;
    }
    else {
      em->healthComponents[entity].hasTakenDamage = false;
    }
  } 
  else if (ai->state == AI_STATE_CHASE) {
    // Attack
    if (dist <= ai->attackRange) ai->state = AI_STATE_ATTACK;

    // Lose trail
    if (dist > ai->sightRadius * 1.6f) ai->state = AI_STATE_PATROL;
  }
  else if (ai->state == AI_STATE_ATTACK) {
    // Out of Range
    if (dist > ai->attackRange * 1.2f) ai->state = AI_STATE_CHASE;
  }
}

static void ProcessAIState(EntityManager* em, Entity entity, AIControlComponent* ai, TransformComponent* transform, PhysicsComponent* phys, AnimationComponent* anim, WeaponControlComponent* wc, Vector3 targetPos, Entity playerID, float dt) {
  if (ai->state == AI_STATE_PATROL) {
    if (ai->numPatrolPoints > 0 && ai->patrolPoints != NULL) {
      // Ensure patrol index is valid
      if (ai->currentPatrolIndex >= ai->numPatrolPoints || ai->currentPatrolIndex < 0) {
        ai->currentPatrolIndex = 0;
      }

      Vector3 patrolTarget = ai->patrolPoints[ai->currentPatrolIndex];
      Vector3 toPatrol = Vector3Subtract(patrolTarget, transform->position);
      toPatrol.y = 0;
      float distToPatrol = Vector3Length(toPatrol);

      // Move towards patrol point
      if (distToPatrol > 5.0f) {
        AiMovementControl(entity, em, transform, phys, anim, patrolTarget, AI_MOVE_SPEED * 0.5f, dt);
      }
      else {
        // Reached patrol point, move to next
        phys->velocity = Vector3Lerp(phys->velocity, Vector3Zero(), dt * 5.0f);
        ai->currentPatrolIndex++;
        
        if (ai->currentPatrolIndex >= ai->numPatrolPoints) {
          ai->currentPatrolIndex = 0;
        }
      }
    } 
    else {
      // No patrol points: stay idle
      phys->velocity = Vector3Lerp(phys->velocity, Vector3Zero(), dt * 5.0f);
      // Switch to idle animation if currently in idle or walk
      if (anim && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK)) {
        if (anim->currentAnim != SCOUT_ANIM_IDLE) {
          anim->currentAnim = SCOUT_ANIM_IDLE;
          anim->currentTime = 0.0f;
        }
      }
    }
    wc->triggerPulled = false;
  }
  else if (ai->state == AI_STATE_CHASE) {
    AiMovementControl(entity, em, transform, phys, anim, targetPos, AI_MOVE_SPEED, dt);
  }
  else if (ai->state == AI_STATE_ATTACK) {
    phys->velocity = Vector3Lerp(phys->velocity, Vector3Zero(), dt * 5.0f);

    HealthComponent* health = NULL;
    if (em->componentMasks[entity] & COMPONENT_HEALTH) {
      health = &em->healthComponents[entity];
      UpdateDamageReactionTimer(health, dt);
    }

    if (anim) {
      // Handle damage reaction animations (priority over idle/walk)
      bool isInDamageReaction = HandleDamageReactionAnimations(entity, em, transform, phys, anim, health, dt);
      // If not in damage reaction, set idle or walk based on movement
      if (!isInDamageReaction && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK)) {
        SetIdleOrWalkAnimation(anim, phys);
      }
    }

    Vector3 toPlayer = Vector3Subtract(targetPos, transform->position);
    Vector3 currentFwd = Vector3RotateByQuaternion((Vector3){0,0,-1}, transform->orientation);
    Vector3 newDir = Vector3Lerp(currentFwd, Vector3Normalize(toPlayer), AI_TURN_SPEED * dt);
    transform->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, Vector3Normalize(newDir));

    Vector3 error = {
      (float)GetRandomValue(-5,5)/100.0f * AI_AIM_ERROR,
      (float)GetRandomValue(-5,5)/100.0f * AI_AIM_ERROR,
      (float)GetRandomValue(-5,5)/100.0f * AI_AIM_ERROR
    };

    wc->aimDirection = Vector3Normalize(Vector3Add(toPlayer, error));
    wc->triggerPulled = true;
    wc->lockedTarget = playerID;
  } else {
    // Unknown state: stop and go to idle
    phys->velocity = Vector3Zero();
    if (anim && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK)) {
      if (anim->currentAnim != SCOUT_ANIM_IDLE) {
        anim->currentAnim = SCOUT_ANIM_IDLE;
        anim->currentTime = 0.0f;
      }
    }
  }
}

void AIControlSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  float dt = systems->delta_time;

  Vector3 targetPos = FindPlayerPosition(em);
  if (Vector3LengthSqr(targetPos) < 0.001f) return;

  Entity playerID = MAX_ENTITIES;
  for (int i = 0; i < em->numEntities && playerID == MAX_ENTITIES; i++) {
    if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
      playerID = i;
    }
  }

  uint32_t aiMask = COMPONENT_AI_CONTROL | COMPONENT_TRANSFORM | COMPONENT_PHYSICS;
  for (int e = 0; e < em->numEntities; e++) {
    if ((em->componentMasks[e] & aiMask) == aiMask) {
      AIControlComponent* ai = &em->aiControlComponents[e];
      TransformComponent* transform = &em->transformComponents[e];
      PhysicsComponent* phys = &em->physicsComponents[e];
      AnimationComponent* anim = NULL;
      if (em->componentMasks[e] & COMPONENT_ANIMATION) {
        anim = &em->animationComponents[e];
      }

      WeaponControlComponent* wc = &em->weaponControlComponents[e];
      wc->triggerPulled = false;

      float distSq = Vector3DistanceSqr(transform->position, targetPos);
      float dist = sqrtf(distSq);

      bool canSeePlayer = CanSeePlayer(em, ai, transform, targetPos, dist);

      // Turn towards player if within sight radius but not in FOV
      if (ai->state == AI_STATE_PATROL && dist <= ai->sightRadius) {

        Vector3 toPlayer = Vector3Normalize(Vector3Subtract(targetPos, transform->position));

        if (Vector3LengthSqr(toPlayer) > 0.0f) {

          Vector3 currentFwd = Vector3RotateByQuaternion((Vector3){0,0,-1}, transform->orientation);
          float dot = Vector3DotProduct(currentFwd, toPlayer);

          // If not facing player (dot < threshold), turn towards them
          if (dot < AI_FOV_THRESHOLD) {
            Vector3 newDir = Vector3Lerp(currentFwd, toPlayer, AI_TURN_SPEED * dt);
            transform->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, Vector3Normalize(newDir));
          }
        }
      }

      UpdateAIState(em, e, ai, transform, targetPos, dist, canSeePlayer);
      ProcessAIState(em, e, ai, transform, phys, anim, wc, targetPos, playerID, dt);
    }
  }
}

// Controls AI entity movement 
// Handles damage reactions, arrival detection and animation updates
// Movement Algorithm:
//   1. Check for damage reactions (stop movement if in reaction)
//   2. Check if close enough to target
//   3. Calculate desired direction to target 
//   4. Calculate throttle based on alignment (only move forward if facing target)
//   5. Apply velocity and update animations
static void AiMovementControl(Entity entity, EntityManager* em, TransformComponent* trans, PhysicsComponent* phys, AnimationComponent* anim,
                              Vector3 targetPos, float speed, float dt) {
  Vector3 myPos = trans->position;
  Vector3 toTarget = Vector3Subtract(targetPos, myPos);

  // Ignore Y component for ground-based movement (only move horizontally)
  toTarget.y = 0;

  // Update damage reaction timer if entity has health component
  HealthComponent* health = NULL;
  if (em->componentMasks[entity] & COMPONENT_HEALTH) {
    health = &em->healthComponents[entity];
    UpdateDamageReactionTimer(health, dt);
  }

  // Check if entity is in damage reaction animation
  if (anim) {
    bool isInDamageReaction = HandleDamageReactionAnimations(entity, em, trans, phys, anim, health, dt);
    if (isInDamageReaction) {
      return;  // Don't move during damage reaction
    }
  }

  // Arrival check: if close enough to target 
  // Uses squared distance to avoid expensive sqrt calculation
  if (Vector3LengthSqr(toTarget) < 4.0f) { 
    phys->velocity = Vector3Zero();
    // Switch to idle animation if currently walking
    if (anim && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK)) {
      if (anim->currentAnim != SCOUT_ANIM_IDLE) {
        anim->currentAnim = SCOUT_ANIM_IDLE;
        anim->currentTime = 0.0f;
      }
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

  // Update animation based on movement speed (idle or walk)
  if (anim && (anim->currentAnim == SCOUT_ANIM_IDLE || anim->currentAnim == SCOUT_ANIM_WALK)) {
    SetIdleOrWalkAnimation(anim, phys);
  }
}

// This timer controls how long damage reaction animations play
static void UpdateDamageReactionTimer(HealthComponent* health, float dt) {
  if (!health) return;

  // Countdown timer: decreases each frame
  if (health->damageReactionTimer > 0.0f) {
    health->damageReactionTimer -= dt;
    // Clamp to 0 to prevent negative values
    if (health->damageReactionTimer < 0.0f) {
      health->damageReactionTimer = 0.0f;
    }
  }
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
  } else {
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
static bool HandleDamageReactionAnimations(Entity entity, EntityManager* em, TransformComponent* trans, 
                                           PhysicsComponent* phys, AnimationComponent* anim, HealthComponent* health, float dt) {
  if (!anim) return false;

  // Check if currently playing a damage reaction animation
  if (health && health->damageReactionTimer > 0.0f && 
    anim->currentAnim >= SCOUT_ANIM_DAMAGE_RIGHT && anim->currentAnim <= SCOUT_ANIM_DAMAGE_FRONT_LEFT) {

    // Check if entity has turned to face the damage source
    Vector3 forward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);
    // Dot product: positive = facing towards damage source
    float facingDot = Vector3DotProduct(forward, Vector3Negate(health->lastDamageDirection));

    // If entity is facing the damage source cancel reaction
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
