#include "systems.h"
#include "ecs/ecs_entitymanager.h"
#include <raymath.h>

void AIControlSystem(struct Systems* systems)
{
    EntityManager* em = &systems->entityManager;

    for (int e = 0; e < em->numEntities; e++)
    {
        // Checa se esse entity tem AI + Transform
        if ((em->componentMasks[e] & COMPONENT_AI_CONTROL) &&
            (em->componentMasks[e] & COMPONENT_TRANSFORM))
        {
            AIControlComponent* ai = &em->aiControlComponents[e];
            TransformComponent* transform = &em->transformComponents[e];

            // --- Procurar Player ---
            Entity player = 0; // geralmente o player é o primeiro criado

            TransformComponent* playerTr = &em->transformComponents[player];

            float dist = Vector3Distance(transform->position, playerTr->position);

            // Estado 0 = idle, 1 = perseguindo, 2 = atacando
            if (dist < ai->sightRadius) {
                ai->state = 1;  // perseguir
            }

            if (dist < ai->attackRange) {
                ai->state = 2; // atacar
            }

            // --- Comportamento de cada estado ---
            if (ai->state == 1) {
                // Mover para o player
                Vector3 dir = Vector3Normalize(Vector3Subtract(playerTr->position, transform->position));
                transform->position = Vector3Add(transform->position, Vector3Scale(dir, systems->delta_time * 5.0f));
            }

            if (ai->state == 2) {
                // Aqui você chama sistema de tiro depois:
                // FireWeapon(e)
            }
        }
    }
}