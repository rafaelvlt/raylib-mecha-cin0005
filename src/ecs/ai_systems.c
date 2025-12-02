#include "systems.h"
#include "ecs/entitymanager.h"
#include <raymath.h>

#define AI_TURN_SPEED      1.5f  
#define AI_MOVE_SPEED      12.5f 
#define AI_FOV_THRESHOLD   0.8f
#define AI_AIM_ERROR       0.1

static void AiMovementControl(TransformComponent* trans, PhysicsComponent* phys, Vector3 targetPos, float speed, float dt);

void AIControlSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  float dt = systems->delta_time;

  Vector3 targetPos = {0};
  bool playerFound = false;
  Entity playerID = MAX_ENTITIES;

  // Finds Player if they have the Player Control Component
  for (int i = 0; i < em->numEntities && !playerFound; i++) {
    if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
      targetPos = em->transformComponents[i].position;
      playerFound = true;
    }
  }

  if (!playerFound) return; 
  uint32_t aiMask = COMPONENT_AI_CONTROL | COMPONENT_TRANSFORM | COMPONENT_PHYSICS;
  for (int e = 0; e < em->numEntities; e++) {

    if ((em->componentMasks[e] & aiMask) == aiMask) 
    {
      AIControlComponent* ai = &em->aiControlComponents[e];
      TransformComponent* transform = &em->transformComponents[e];
      PhysicsComponent* phys = &em->physicsComponents[e];

      WeaponControlComponent* wc = &em->weaponControlComponents[e];
      // Trigger reset
      wc->triggerPulled = false;


      float distSq = Vector3DistanceSqr(transform->position, targetPos);
      float dist = sqrtf(distSq);

      bool canSeePlayer = false;
      if (dist <= ai->sightRadius) {
        Vector3 toPlayer = Vector3Normalize(Vector3Subtract(targetPos, transform->position));
        Vector3 forward = Vector3RotateByQuaternion((Vector3){0,0,1}, transform->orientation);

        float dot = Vector3DotProduct(forward, toPlayer);

        if (ai->state != 0 || dot > AI_FOV_THRESHOLD) {
          canSeePlayer = true;
        }
      }

      if (ai->state == 0) {
         // IDLE / PATROL
        if (canSeePlayer || (em->healthComponents[e].hasTakenDamage && dist <= ai->sightRadius)) ai->state = 1; 
        if (em->healthComponents[e].currentHealth < em->healthComponents[e].maxHealth && dist <= ai->sightRadius) {
          em->healthComponents[e].hasTakenDamage = true;
        }
        else em->healthComponents[e].hasTakenDamage = false;
      }
      else if (ai->state == 1) { // CHASE
        if (dist <= ai->attackRange) ai->state = 2; 
        if (dist > ai->sightRadius * 1.6f) ai->state = 0;
      }
      else if (ai->state == 2) { // ATTACK
        if (dist > ai->attackRange * 1.2f) ai->state = 1;
      }

      // Estado 0 = patrol, 1 = perseguindo, 2 = atacando
      /*
      if (dist >= ai->sightRadius) {
        ai->state = 0; // patrol
      }
      if (dist < ai->sightRadius) {
        ai->state = 1;  // perseguir
      }

      if (dist < ai->attackRange) {
        ai->state = 2; // atacar
      }*/

      /*Vector3 dirToTarget = Vector3Subtract(targetPos, transform->position);
      Vector3 foward = {
        2.0f * (transform->orientation.x * transform->orientation.z + transform->orientation.w * transform->orientation.y),
        2.0f * (transform->orientation.y * transform->orientation.z - transform->orientation.w * transform->orientation.x),
        1.0f - 2.0f * (transform->orientation.x * transform->orientation.x + transform->orientation.y * transform->orientation.y)
      }; 
      float angleToTarget = Vector3Angle(foward, Vector3Normalize(dirToTarget)) * (180.0f / PI); // Convert to degrees

      //optional: simplificação da lógica acima
      ai->state = ((dist<ai->attackRange)+(dist<ai->sightRadius));

      if (angleToTarget > 90.0f && angleToTarget < 270.0f) {
        ai->state = 0; // Volta para patrulha se o player sair do campo de visão
      }*/


      switch (ai->state){
        case 0: // Patrol
          // Implementar patrulha se houver pontos de patrulha
          if (ai->numPatrolPoints > 0 && ai->patrolPoints != NULL) {
            Vector3 patrolTarget = ai->patrolPoints[ai->currentPatrolIndex];
            AiMovementControl(transform, phys, patrolTarget, AI_MOVE_SPEED * 0.5f, dt); // Patrulha devagar

            //phys->velocity = Vector3Scale(dir, AI_MOVE_SPEED);

            // Verifica se chegou perto o suficiente do ponto de patrulha
            if (Vector3Distance(transform->position, patrolTarget) < 8.0f) {
              // Move para o próximo ponto de patrulha
              ai->currentPatrolIndex++;
              if (ai->currentPatrolIndex >= ai->numPatrolPoints) {
                ai->currentPatrolIndex = 0; // Loop back to the first patrol point
              }
            }
          } else {
            // Sem pontos de patrulha, permanece parado
            phys->velocity = Vector3Zero();
          }
          wc->triggerPulled = false; // Não atira enquanto patrulha
          break;
        case 1: // Chase
          // Lógica de perseguição já implementada abaixo
          AiMovementControl(transform, phys, targetPos, AI_MOVE_SPEED, dt);
          break;
        case 2: // Attack
          // Lógica de ataque já implementada abaixo
          // Freia até parar
          phys->velocity = Vector3Lerp(phys->velocity, Vector3Zero(), dt * 5.0f);

          // Rotação
          Vector3 toPlayer = Vector3Subtract(targetPos, transform->position);
          Vector3 currentFwd = Vector3RotateByQuaternion((Vector3){0,0, 1}, transform->orientation);
          Vector3 newDir = Vector3Lerp(currentFwd, Vector3Normalize(toPlayer), AI_TURN_SPEED * dt);
          transform->orientation = QuaternionFromVector3ToVector3((Vector3){0,0,1}, Vector3Normalize(newDir));

          // Attack Logic
          Vector3 error = { 
            (float)GetRandomValue(-5,5)/100.0f * AI_AIM_ERROR, 
            (float)GetRandomValue(-5,5)/100.0f * AI_AIM_ERROR, 
            (float)GetRandomValue(-5,5)/100.0f * AI_AIM_ERROR 
          };

          wc->aimDirection = Vector3Normalize(Vector3Add(toPlayer, error));
          wc->triggerPulled = true;
          wc->lockedTarget= playerID;
          break;
        default:
          // Idle - Garante que ele pare se o player fugir
          phys->velocity = Vector3Zero();
          break;
      }
    }
  }
}



static void AiMovementControl(TransformComponent* trans, PhysicsComponent* phys, Vector3 targetPos, float speed, float dt) {
  Vector3 myPos = trans->position;
  Vector3 toTarget = Vector3Subtract(targetPos, myPos);
  toTarget.y = 0;

  if (Vector3LengthSqr(toTarget) < 4.0f) {
    phys->velocity = Vector3Zero();
    return;
  }

  Vector3 desiredDir = Vector3Normalize(toTarget);
  if (Vector3LengthSqr(desiredDir) < 0.001f) return;

  Vector3 currentForward = Vector3RotateByQuaternion((Vector3){0, 0, 1}, trans->orientation);

  Vector3 newDir = Vector3Lerp(currentForward, desiredDir, AI_TURN_SPEED * dt);
  if (Vector3LengthSqr(newDir) < 0.001f) newDir = currentForward;

  newDir = Vector3Normalize(newDir);

  trans->orientation = QuaternionFromVector3ToVector3((Vector3){0, 0, 1}, newDir);

  float alignment = Vector3DotProduct(currentForward, desiredDir);
  float throttle = (alignment > 0.0f) ? alignment : 0.0f;

  if (alignment > 0.5f) { 
    throttle = alignment; 
  }

  phys->velocity = Vector3Scale(newDir, speed * throttle);
}
