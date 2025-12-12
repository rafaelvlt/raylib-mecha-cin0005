#include "systems.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include <raylib.h>
#include <math.h>

#define DEFAULT_MAX_HEAT 100.0f
#define DEFAULT_DISSIPATION_RATE 15.0f 
#define DEFAULT_PENALTY_DURATION 2.0f 

#define HEAT_SYSTEM_MASK (COMPONENT_HEAT)


void AddHeat(struct Systems *systems, Entity entity, float amount)
{
    EntityManager *em = &systems->entityManager;
    uint32_t mask = em->componentMasks[entity];

    if ((mask & COMPONENT_HEAT) == COMPONENT_HEAT)
    {
        HeatComponent *hc = &em->heatComponents[entity];

        if (hc->penaltyTimer <= 0.0f)
        {
            hc->currentHeat += amount;

            if (hc->currentHeat >= hc->maxHeat)
            {
                hc->currentHeat = hc->maxHeat;
                hc->penaltyTimer = hc->heatPenaltyDuration;
                hc->isOverheated = true;
            }
        }

        if ((mask & COMPONENT_COCKPIT_HUD) == COMPONENT_COCKPIT_HUD)
        {
            em->cockpitHUDComponents[entity].currentHeat = hc->currentHeat;
            em->cockpitHUDComponents[entity].maxHeat = hc->maxHeat;
        }
    }
}



// Add heat on shot fired
void HeatSystemOnEvent(struct Systems *systems, Event event)
{
    EntityManager *em = &systems->entityManager;

    switch (event.type)
    {
    case EVENT_WEAPON_FIRED:
    {
        Entity owner = event.data.weaponFired.owner;

        if ((em->componentMasks[owner] & COMPONENT_HEAT) == COMPONENT_HEAT)
        {
            float heatToAdd = 0.0f;

            if ((em->componentMasks[owner] & COMPONENT_WEAPON_CONTROL) == COMPONENT_WEAPON_CONTROL)
            {
                WeaponControlComponent *wcc = &em->weaponControlComponents[owner];

                for (int i = 0; i < MAX_WEAPONS_EQUIP && heatToAdd == 0.0f; i++)
                {
                    Entity slot = wcc->weaponsSlots[i];
                    if (slot != MAX_ENTITIES && (em->componentMasks[slot] & COMPONENT_WEAPON))
                    {
                        WeaponComponent *wc = &em->weaponComponents[slot];
                        if (wc->type == event.data.weaponFired.weapon)
                        {
                            heatToAdd = wc->heatGenerated;
                            break;
                        }
                    }
                }
            }

            if (heatToAdd > 0.0f)
            {
                AddHeat(systems, owner, heatToAdd);
            }
        }
        break;
    }
    default:
        break;
    }
}

void HeatSystem(struct Systems *systems)
{
    EntityManager *em = &systems->entityManager;
    float dt = systems->delta_time;

    for (Entity e = 0; e < em->numEntities; e++)
    {
        uint32_t mask = em->componentMasks[e];
        if ((mask & HEAT_SYSTEM_MASK) == COMPONENT_HEAT)
        {
            HeatComponent *hc = &em->heatComponents[e];

            if (hc->penaltyTimer > 0.0f)
            {
                hc->penaltyTimer -= dt;

                if (hc->penaltyTimer <= 0.0f)
                {
                    hc->penaltyTimer = 0.0f;
                    hc->isOverheated = false;
                }
            }

            if (hc->currentHeat > 0.0f)
            {
                float dissipation = hc->dissipationRate;

                hc->currentHeat -= dissipation * dt;

                if (hc->currentHeat < 0.0f)
                {
                    hc->currentHeat = 0.0f;
                }
            }
            if ((mask & COMPONENT_COCKPIT_HUD) == COMPONENT_COCKPIT_HUD)
            {
                em->cockpitHUDComponents[e].currentHeat = hc->currentHeat;
            }
        }
    }
}
