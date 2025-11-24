#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
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

        hp->currentHealth -= damage;

        TraceLog(LOG_INFO, "DMG: Entity %d took %.1f dmg. HP: %.1f/%0.1f", 
                 victim, damage, hp->currentHealth, hp->maxHealth);

        if (hp->currentHealth <= 0) {

          EventData deathData;
          deathData.deathEvent.owner = victim;
          deathData.deathEvent.killer = attacker;
          deathData.deathEvent.pos = em->transformComponents[victim].position;
          PushEvent(systems, EVENT_ENTITY_DEATH, deathData);

          DestroyEntity(em, victim);
          TraceLog(LOG_INFO, "KILL: Entity %d destroyed.", victim);
        }
      }
    }
  }
}
