#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include "systems.h"
#include "ecs/entitymanager.h"
#include "ecs/types.h"
#include "resource_manager.h"

// Configuration Constants
#define AI_TURN_SPEED                2.0f   
#define AI_MOVE_SPEED                12.5f
#define AI_FOV_THRESHOLD             0.5f   
#define AI_FOV_EXTENDED              -0.2f 
#define AI_AIM_ERROR                 0.1f
#define AI_DAMAGE_FACING_THRESHOLD   0.7f

// Alignment when reaching patrol point
#define AI_ALIGN_START_TURN          0.6f 
#define AI_ALIGN_STOP_TURN           0.95f
#define AI_PATROL_LOOK_CANCEL        0.5f 

#define AI_ORBIT_DISTANCE            90.0f

//Helper functions Prototypes
static Entity FindPlayer(EntityManager* em, Vector3* outPos);
static bool IsAnimFinished(AnimationComponent* anim, ResourceManager* rm);
static Vector3 GetTorsoForward(TransformComponent* trans, TorsoState torsoState);
static bool CheckFOV(TransformComponent* trans, TorsoState torsoState, Vector3 targetPos, float sightRadius);
static void ManageTorsoAnimation(AnimationComponent* anim, PhysicsComponent* phys, TorsoState* tState, float dt, ResourceManager* rm);
static bool IsTorsoTransitioning(TorsoState state);

// Behaviors for each state
static void BehaviorPatrol(AIControlComponent* ai, TransformComponent* trans, PhysicsComponent* phys, float dt);
static void BehaviorChase(AIControlComponent* ai, TransformComponent* trans, PhysicsComponent* phys, Vector3 targetPos, float dt);
static void BehaviorAttack(AIControlComponent* ai, TransformComponent* trans, PhysicsComponent* phys, Vector3 targetPos, float dt);


// ----------------------------------------------------------------------------
// Main System Loop
// ----------------------------------------------------------------------------
void AIControlSystem(struct Systems* systems) {
    EntityManager* em = &systems->entityManager;
    ResourceManager* rm = &systems->resourceManager;
    float dt = systems->delta_time;

    Vector3 playerPos;
    Entity playerID = FindPlayer(em, &playerPos);
    bool playerExists = (playerID != MAX_ENTITIES);

    uint32_t mask = COMPONENT_AI_CONTROL | COMPONENT_TRANSFORM | COMPONENT_PHYSICS;

    for (int e = 0; e < em->numEntities; e++) {
        if ((em->componentMasks[e] & mask) != mask) continue;

        AIControlComponent* ai = &em->aiControlComponents[e];
        TransformComponent* trans = &em->transformComponents[e];
        PhysicsComponent* phys = &em->physicsComponents[e];
        
        AnimationComponent* anim = NULL;
        if (em->componentMasks[e] & COMPONENT_ANIMATION) {
            anim = &em->animationComponents[e];
        }

        WeaponControlComponent* wc = NULL;
        if (em->componentMasks[e] & COMPONENT_WEAPON_CONTROL) {
            wc = &em->weaponControlComponents[e];
        }

        HealthComponent* health = NULL;
        if (em->componentMasks[e] & COMPONENT_HEALTH) {
            health = &em->healthComponents[e];
        }
        
        if (wc) wc->triggerPulled = false;

        // Check visibility
        float distToPlayer = 9999.0f;
        bool canSeePlayer = false;
        if (playerExists) {
            distToPlayer = Vector3Distance(trans->position, playerPos);
            if (distToPlayer <= ai->sightRadius) {
                canSeePlayer = CheckFOV(trans, ai->torsoState, playerPos, ai->sightRadius);
            }
        }

        // React to damage
        if (health && health->hasTakenDamage) {
            ai->state = AI_STATE_CHASE; 
            health->hasTakenDamage = false; 
        }

        // Run behavior based on current state
        switch (ai->state) {
            case AI_STATE_PATROL:
                if (canSeePlayer) ai->state = AI_STATE_CHASE;
                BehaviorPatrol(ai, trans, phys, dt);
                break;

            case AI_STATE_CHASE:
                if (!canSeePlayer && distToPlayer > ai->sightRadius * 1.5f) {
                    ai->state = AI_STATE_PATROL;
                } else if (distToPlayer <= ai->attackRange) {
                    ai->state = AI_STATE_ATTACK;
                } else {
                    BehaviorChase(ai, trans, phys, playerPos, dt);
                }
                break;

            case AI_STATE_ATTACK:
                if (distToPlayer > ai->attackRange * 1.2f) {
                    ai->state = AI_STATE_CHASE;
                } else {
                    BehaviorAttack(ai, trans, phys, playerPos, dt);
                    
                    if (wc) {
                        // Apply random error to aim
                        float err = 0.01f * AI_AIM_ERROR;
                        Vector3 error = {
                            (float)GetRandomValue(-5, 5) * err,
                            (float)GetRandomValue(-5, 5) * err,
                            (float)GetRandomValue(-5, 5) * err
                        };

                        Vector3 torsoFwd = GetTorsoForward(trans, ai->torsoState);
                        Vector3 toPlayer = Vector3Normalize(Vector3Subtract(playerPos, trans->position));
                        
                        // Only shoot if torso is aligned with target
                        if (Vector3DotProduct(torsoFwd, toPlayer) > 0.8f) {
                            wc->triggerPulled = true;
                        }
                        
                        wc->aimDirection = Vector3Normalize(Vector3Add(Vector3Subtract(playerPos, trans->position), error));
                        wc->lockedTarget = playerID;
                    }
                }
                break;
        }

        // Update animation logic
        if (anim) {
            ManageTorsoAnimation(anim, phys, &ai->torsoState, dt, rm);
        }
    }
}

// ----------------------------------------------------------------------------
// AI Behaviors
// ----------------------------------------------------------------------------

// Moves between patrol points and occasionally looks around
static void BehaviorPatrol(AIControlComponent* ai, TransformComponent* trans, PhysicsComponent* phys, float dt) {
    if (ai->numPatrolPoints <= 0 || !ai->patrolPoints) { 
        phys->velocity = Vector3Zero(); 
        return; 
    }

    Vector3 target = ai->patrolPoints[ai->currentPatrolIndex];
    Vector3 toTarget = Vector3Subtract(target, trans->position);
    toTarget.y = 0; 
    float distSq = Vector3LengthSqr(toTarget);

    // Check if reached destination
    if (distSq < 25.0f) { 
        ai->currentPatrolIndex = (ai->currentPatrolIndex + 1) % ai->numPatrolPoints;
        phys->velocity = Vector3Zero();
        return;
    }

    Vector3 desiredDir = Vector3Normalize(toTarget);
    Vector3 currentForward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);
    float alignment = Vector3DotProduct(currentForward, desiredDir);
    
    // Determine stabilization threshold based on movement
    float threshold;
    if (Vector3LengthSqr(phys->velocity) > 0.1f) {
        threshold = AI_ALIGN_START_TURN;
    } else {
        threshold = AI_ALIGN_STOP_TURN;
    }

    if (alignment < threshold) {
        // Stop and turn
        phys->velocity = Vector3Zero();
        Vector3 newDir = Vector3Lerp(currentForward, desiredDir, AI_TURN_SPEED * dt);
        
        if (Vector3LengthSqr(newDir) > 0.001f) {
            trans->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, Vector3Normalize(newDir));
        }
        
        // Cancel looking sideways if the turn is too sharp
        if (alignment < AI_PATROL_LOOK_CANCEL && !IsTorsoTransitioning(ai->torsoState)) {
            if (ai->torsoState == TORSO_LOOKING_L) ai->torsoState = TORSO_RETURNING_L;
            if (ai->torsoState == TORSO_LOOKING_R) ai->torsoState = TORSO_RETURNING_R;
        }
    } else {
        // Move forward
        phys->velocity = Vector3Scale(desiredDir, AI_MOVE_SPEED * 0.5f); 

        // Randomly look around
        if (ai->torsoState == TORSO_CENTER && ai->torsoTimer <= 0.0f) {
            if (GetRandomValue(0, 100) < 2) { 
                if (GetRandomValue(0, 1) == 0) ai->torsoState = TORSO_TWISTING_L;
                else ai->torsoState = TORSO_TWISTING_R;
                ai->torsoTimer = (float)GetRandomValue(20, 50) / 10.0f;
            }
        }
        
        // Handle look timer
        if (ai->torsoState == TORSO_LOOKING_L || ai->torsoState == TORSO_LOOKING_R) {
            ai->torsoTimer -= dt;
            if (ai->torsoTimer <= 0.0f) {
                if (ai->torsoState == TORSO_LOOKING_L) ai->torsoState = TORSO_RETURNING_L;
                else ai->torsoState = TORSO_RETURNING_R;
                ai->torsoTimer = 2.0f; 
            }
        } else if (ai->torsoState == TORSO_CENTER && ai->torsoTimer > 0.0f) {
            ai->torsoTimer -= dt; 
        }
    }
}

// Moves directly towards the target
static void BehaviorChase(AIControlComponent* ai, TransformComponent* trans, PhysicsComponent* phys, Vector3 targetPos, float dt) {
    Vector3 toPlayer = Vector3Subtract(targetPos, trans->position);
    toPlayer.y = 0;
    
    phys->velocity = Vector3Scale(Vector3Normalize(toPlayer), AI_MOVE_SPEED); 
    
    Vector3 currentForward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);
    Vector3 newDir = Vector3Lerp(currentForward, Vector3Normalize(toPlayer), AI_TURN_SPEED * dt);
    
    if (Vector3LengthSqr(newDir) > 0.001f) {
        trans->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, Vector3Normalize(newDir));
    }

    // Keep torso centered while chasing
    if (ai->torsoState != TORSO_CENTER && !IsTorsoTransitioning(ai->torsoState)) {
        if (ai->torsoState == TORSO_LOOKING_L) ai->torsoState = TORSO_RETURNING_L;
        if (ai->torsoState == TORSO_LOOKING_R) ai->torsoState = TORSO_RETURNING_R;
    }
}

// Orbits around the target and aims the torso
static void BehaviorAttack(AIControlComponent* ai, TransformComponent* trans, PhysicsComponent* phys, Vector3 targetPos, float dt) {
    Vector3 toPlayer = Vector3Subtract(targetPos, trans->position);
    toPlayer.y = 0;
    float dist = Vector3Length(toPlayer);
    Vector3 dirToPlayer = Vector3Normalize(toPlayer);

    // Calculate orbit direction
    Vector3 orbitDir = Vector3CrossProduct((Vector3){0,1,0}, dirToPlayer);
    Vector3 finalMoveDir = orbitDir;
    
    // Adjust distance
    if (dist > AI_ORBIT_DISTANCE + 5.0f) {
        finalMoveDir = Vector3Add(orbitDir, Vector3Scale(dirToPlayer, 0.5f)); 
    } else if (dist < AI_ORBIT_DISTANCE - 5.0f) {
        finalMoveDir = Vector3Add(orbitDir, Vector3Scale(dirToPlayer, -0.5f));
    }
    
    phys->velocity = Vector3Scale(Vector3Normalize(finalMoveDir), AI_MOVE_SPEED);

    // Rotate legs to face movement
    Vector3 currentForward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);
    Vector3 newBodyDir = Vector3Lerp(currentForward, finalMoveDir, AI_TURN_SPEED * dt);
    
    if (Vector3LengthSqr(newBodyDir) > 0.001f) {
        trans->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,-1}, Vector3Normalize(newBodyDir));
    }

    if (IsTorsoTransitioning(ai->torsoState)) return;

    // Calculate aiming direction
    Vector3 bodyRight = Vector3CrossProduct(newBodyDir, (Vector3){0,1,0});
    float sideDot = Vector3DotProduct(bodyRight, dirToPlayer);
    float frontDot = Vector3DotProduct(newBodyDir, dirToPlayer);
    
    TorsoState desiredState = TORSO_CENTER;
    
    // If target is not in front, decide which way to look
    if (frontDot < 0.7f) { 
        if (sideDot > 0) desiredState = TORSO_LOOKING_R;
        else desiredState = TORSO_LOOKING_L;
    }

    // Apply state transitions
    if (ai->torsoState == TORSO_CENTER && desiredState != TORSO_CENTER) {
        if (desiredState == TORSO_LOOKING_L) ai->torsoState = TORSO_TWISTING_L;
        else ai->torsoState = TORSO_TWISTING_R;
    } else if ((ai->torsoState == TORSO_LOOKING_L || ai->torsoState == TORSO_LOOKING_R) && desiredState == TORSO_CENTER) {
        if (ai->torsoState == TORSO_LOOKING_L) ai->torsoState = TORSO_RETURNING_L;
        else ai->torsoState = TORSO_RETURNING_R;
    } else if (ai->torsoState == TORSO_LOOKING_L && desiredState == TORSO_LOOKING_R) {
        ai->torsoState = TORSO_RETURNING_L;
    } else if (ai->torsoState == TORSO_LOOKING_R && desiredState == TORSO_LOOKING_L) {
        ai->torsoState = TORSO_RETURNING_R;
    }
}

// ----------------------------------------------------------------------------
// Helper Functions
// ----------------------------------------------------------------------------

static Entity FindPlayer(EntityManager* em, Vector3* outPos) {
    for (int i = 0; i < em->numEntities; i++) {
        if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
            *outPos = em->transformComponents[i].position;
            return i;
        }
    }
    *outPos = (Vector3){0};
    return MAX_ENTITIES;
}

static bool IsAnimFinished(AnimationComponent* anim, ResourceManager* rm) {
    if (anim->modelId < 0) return true;
    ModelAnimation* anims = rm->modelAnimations[anim->modelId];
    int count = rm->modelAnimCounts[anim->modelId];
    int idx = anim->currentAnim;
    
    if (!anims || count <= 0 || idx < 0 || idx >= count) return true;
    
    // Safety check for start of animation
    if (anims[idx].frameCount > 0 && anim->currentTime <= 0.1f) return false;

    return anim->currentTime >= (float)anims[idx].frameCount;
}

// Adjust angle based on torso state
static Vector3 GetTorsoForward(TransformComponent* trans, TorsoState torsoState) {
    Vector3 legsForward = Vector3RotateByQuaternion((Vector3){0, 0, -1}, trans->orientation);
    float angleOffset = 0.0f;
    
    if (torsoState == TORSO_LOOKING_L || torsoState == TORSO_TWISTING_L) angleOffset = 90.0f * DEG2RAD;
    else if (torsoState == TORSO_LOOKING_R || torsoState == TORSO_TWISTING_R) angleOffset = -90.0f * DEG2RAD;

    if (fabsf(angleOffset) > 0.001f) {
        return Vector3RotateByAxisAngle(legsForward, (Vector3){0,1,0}, angleOffset);
    }
    return legsForward;
}

static bool CheckFOV(TransformComponent* trans, TorsoState torsoState, Vector3 targetPos, float sightRadius) {
    Vector3 toTarget = Vector3Normalize(Vector3Subtract(targetPos, trans->position));
    Vector3 torsoFwd = GetTorsoForward(trans, torsoState);
    
    float threshold;
    if (torsoState == TORSO_CENTER) threshold = AI_FOV_THRESHOLD;
    else threshold = AI_FOV_EXTENDED;
    
    return Vector3DotProduct(torsoFwd, toTarget) > threshold;
}

static bool IsTorsoTransitioning(TorsoState state) {
    return (state == TORSO_TWISTING_L || state == TORSO_TWISTING_R || 
            state == TORSO_RETURNING_L || state == TORSO_RETURNING_R);
}

// Maps the current state to the correct animation ID
static int GetExpectedAnim(TorsoState state, bool isMoving) {
    switch (state) {
        case TORSO_CENTER: 
            if (isMoving) return MECHA_ANIM_WALK;
            return MECHA_ANIM_IDLE;
        
        case TORSO_TWISTING_L: 
            if (isMoving) return MECHA_ANIM_WALK_TWIST_C_TO_L;
            return MECHA_ANIM_IDLE_TWIST_C_TO_L;
        
        case TORSO_LOOKING_L: 
            if (isMoving) return MECHA_ANIM_WALK_LOOKING_L;
            return MECHA_ANIM_IDLE_LOOKING_L;
        
        case TORSO_RETURNING_L: 
            if (isMoving) return MECHA_ANIM_WALK_TWIST_L_TO_C;
            return MECHA_ANIM_IDLE_TWIST_L_TO_C;
        
        case TORSO_TWISTING_R: 
            if (isMoving) return MECHA_ANIM_WALK_TWIST_C_TO_R;
            return MECHA_ANIM_IDLE_TWIST_C_TO_R;
        
        case TORSO_LOOKING_R: 
            if (isMoving) return MECHA_ANIM_WALK_LOOKING_R;
            return MECHA_ANIM_IDLE_LOOKING_R;
        
        case TORSO_RETURNING_R: 
            if (isMoving) return MECHA_ANIM_WALK_TWIST_R_TO_C;
            return MECHA_ANIM_IDLE_TWIST_R_TO_C;
            
        default: return MECHA_ANIM_IDLE;
    }
}

// Determines if frame time can be kept when switching animations
static bool CanPreserveFrame(int current, int expected) {
    // Check Walk/Idle pairs for Left Twist
    if ((expected == MECHA_ANIM_WALK_TWIST_C_TO_L && current == MECHA_ANIM_IDLE_TWIST_C_TO_L) ||
        (expected == MECHA_ANIM_IDLE_TWIST_C_TO_L && current == MECHA_ANIM_WALK_TWIST_C_TO_L)) return true;
        
    if ((expected == MECHA_ANIM_WALK_TWIST_C_TO_R && current == MECHA_ANIM_IDLE_TWIST_C_TO_R) ||
        (expected == MECHA_ANIM_IDLE_TWIST_C_TO_R && current == MECHA_ANIM_WALK_TWIST_C_TO_R)) return true;
        
    if ((expected == MECHA_ANIM_WALK_TWIST_L_TO_C && current == MECHA_ANIM_IDLE_TWIST_L_TO_C) ||
        (expected == MECHA_ANIM_IDLE_TWIST_L_TO_C && current == MECHA_ANIM_WALK_TWIST_L_TO_C)) return true;
        
    if ((expected == MECHA_ANIM_WALK_TWIST_R_TO_C && current == MECHA_ANIM_IDLE_TWIST_R_TO_C) ||
        (expected == MECHA_ANIM_IDLE_TWIST_R_TO_C && current == MECHA_ANIM_WALK_TWIST_R_TO_C)) return true;
    
    if ((expected == MECHA_ANIM_WALK_LOOKING_L && current == MECHA_ANIM_IDLE_LOOKING_L) ||
        (expected == MECHA_ANIM_IDLE_LOOKING_L && current == MECHA_ANIM_WALK_LOOKING_L)) return true;
        
    if ((expected == MECHA_ANIM_WALK_LOOKING_R && current == MECHA_ANIM_IDLE_LOOKING_R) ||
        (expected == MECHA_ANIM_IDLE_LOOKING_R && current == MECHA_ANIM_WALK_LOOKING_R)) return true;
        
    if ((expected == MECHA_ANIM_WALK && current == MECHA_ANIM_IDLE) ||
        (expected == MECHA_ANIM_IDLE && current == MECHA_ANIM_WALK)) return true;
        
    return false;
}

/*
 * 1. Find the correct animation for the current state and movement.
 * 2. Switch animation immediately if it does not match. Exit to play one frame.
 * 3. Only check if the transition finished if the correct animation is playing.
 */
static void ManageTorsoAnimation(AnimationComponent* anim, PhysicsComponent* phys, TorsoState* tState, float dt, ResourceManager* rm) {
    bool isMoving = Vector3LengthSqr(phys->velocity) > 0.1f;
    int expectedAnim = GetExpectedAnim(*tState, isMoving);

    // Force Animation Switch
    if (anim->currentAnim != expectedAnim) {
        bool preserve = CanPreserveFrame(anim->currentAnim, expectedAnim);
        anim->currentAnim = expectedAnim;
        
        if (!preserve) {
            anim->currentTime = 0.0f;
        }
        return; 
    }

    // Check for Completion
    if (IsTorsoTransitioning(*tState)) {
        if (IsAnimFinished(anim, rm)) {
            if (*tState == TORSO_TWISTING_L) *tState = TORSO_LOOKING_L;
            else if (*tState == TORSO_TWISTING_R) *tState = TORSO_LOOKING_R;
            else if (*tState == TORSO_RETURNING_L || *tState == TORSO_RETURNING_R) *tState = TORSO_CENTER;
        }
    }
}
