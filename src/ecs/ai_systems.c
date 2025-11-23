#include "systems.h"
#include "ecs/entitymanager.h"
#include <raymath.h>


void AIControlSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;

  Vector3 targetPos = {0};
  bool playerFound = false;

  const float AI_MOVE_SPEED = 2.5f;

  // Finds Player if they have the Player Control Component
  for (int i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
      targetPos = em->transformComponents[i].position;
      playerFound = true;
      break;
    }
  }

  if (!playerFound) return; 

  for (int e = 0; e < em->numEntities; e++) {

    if ((em->componentMasks[e] & (COMPONENT_AI_CONTROL | COMPONENT_TRANSFORM | COMPONENT_PHYSICS)) == 
      (COMPONENT_AI_CONTROL | COMPONENT_TRANSFORM | COMPONENT_PHYSICS)) 
    {
      AIControlComponent* ai = &em->aiControlComponents[e];
      TransformComponent* transform = &em->transformComponents[e];
      PhysicsComponent* phys = &em->physicsComponents[e];


      float dist = Vector3Distance(transform->position, targetPos);

      // Estado 0 = idle, 1 = perseguindo, 2 = atacando
      if (dist < ai->sightRadius) {
        ai->state = 1;  // perseguir
      }

      if (dist < ai->attackRange) {
        ai->state = 2; // atacar
      }

      switch (ai->state)
      {
      case 0: // Patrol
        // Implementar patrulha se houver pontos de patrulha
        if (ai->numPatrolPoints > 0) {
          Vector3 patrolTarget = ai->patrolPoints[ai->currentPatrolIndex];
          Vector3 dir = Vector3Normalize(Vector3Subtract(patrolTarget, transform->position));

          phys->velocity = Vector3Scale(dir, AI_MOVE_SPEED);

          // Rotates the object to look at the patrol point
          Matrix lookAt = MatrixLookAt(Vector3Zero(), dir, (Vector3){0,1,0});
          transform->orientation = QuaternionFromMatrix(lookAt);

          // Verifica se chegou perto o suficiente do ponto de patrulha
          if (Vector3Distance(transform->position, patrolTarget) < 1.0f) {
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
        break;
      case 1: // Chase
        // Lógica de perseguição já implementada abaixo
        Vector3 dir = Vector3Normalize(Vector3Subtract(targetPos, transform->position));

        phys->velocity = Vector3Scale(dir, AI_MOVE_SPEED);

        // Rotates the object to look at the player
        Matrix lookAt = MatrixLookAt(Vector3Zero(), dir, (Vector3){0,1,0});
        transform->orientation = QuaternionFromMatrix(lookAt);
        break;
      case 2: // Attack
        // Lógica de ataque já implementada abaixo
        phys->velocity = Vector3Zero();
        break;
      default:
        // Idle - Garante que ele pare se o player fugir
        phys->velocity = Vector3Zero();
        break;
      }
    }
  }
}
