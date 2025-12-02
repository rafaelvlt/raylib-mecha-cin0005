#include <raylib.h>
#include <raymath.h>
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"
#include "ecs/types.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"


void HealthSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  EventManager* ev = &systems->eventManager;

  for (int i = 0; i < ev->eventCounter; i++) {
    Event event = ev->eventQueue[i];

    if (event.type == EVENT_PROJECTILE_COLLISION) {
      Entity attacker = event.data.projectileCollisionDetected.attacker;
      Entity victim = event.data.projectileCollisionDetected.victim;
      float damage = event.data.projectileCollisionDetected.damageAmount;

      if (victim < MAX_ENTITIES && 
        (em->componentMasks[victim] & COMPONENT_HEALTH) == COMPONENT_HEALTH) {

        HealthComponent* hp = &em->healthComponents[victim];
        
        // Calculate damage direction (normalized vector from attacker to victim)
        if (attacker < MAX_ENTITIES && 
            (em->componentMasks[attacker] & COMPONENT_TRANSFORM) == COMPONENT_TRANSFORM &&
            (em->componentMasks[victim] & COMPONENT_TRANSFORM) == COMPONENT_TRANSFORM) {
          Vector3 attackerPos = em->transformComponents[attacker].position;
          Vector3 victimPos = em->transformComponents[victim].position;
          hp->lastDamageDirection = Vector3Normalize(Vector3Subtract(victimPos, attackerPos));
          hp->damageReactionTimer = 1.0f;
        }
        
        hp->hasTakenDamage = true;
        hp->currentHealth -= damage;

        if (hp->currentHealth <= 0) {
          EventData deathData;
          deathData.deathEvent.owner = victim;
          deathData.deathEvent.killer = attacker;
          deathData.deathEvent.pos = em->transformComponents[victim].position;
          if ((em->componentMasks[victim] & COMPONENT_COLLISION) == COMPONENT_COLLISION){
            if (em->collisionComponents[victim].isTrigger) deathData.deathEvent.type = ENTITY_TURRET_STRUCTURE;
          }
          PushEvent(systems, EVENT_ENTITY_DEATH, deathData);

          DestroyEntity(em, victim);
        }
      }
    }
  }
}
