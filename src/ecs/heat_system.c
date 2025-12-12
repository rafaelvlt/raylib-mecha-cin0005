#include "systems.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h" 
#include <raylib.h>
#include <math.h>

#define DEFAULT_MAX_HEAT 100.0f
#define DEFAULT_DISSIPATION_RATE 15.0f // 15 de calor dissipado por segundo
#define DEFAULT_PENALTY_DURATION 2.0f  // 5 segundos de penalidade de superaquecimento


// Máscara para entidades que o sistema de calor deve processar
#define HEAT_SYSTEM_MASK (COMPONENT_HEAT)

// =================================================================
// FUNÇÕES DE UTILIDADE
// =================================================================

/**
 * @brief Adiciona calor a uma entidade, aplicando penalidade se superaquecer.
 * @param systems O struct de sistemas.
 * @param entity A entidade a ser afetada.
 * @param amount A quantidade de calor a adicionar.
 */
void AddHeat(struct Systems* systems, Entity entity, float amount) {
    EntityManager* em = &systems->entityManager;
    uint32_t mask = em->componentMasks[entity];

    if ((mask & COMPONENT_HEAT) == COMPONENT_HEAT) {
        HeatComponent* hc = &em->heatComponents[entity];
        
        // Só adiciona calor se não estiver atualmente penalizado
        if (hc->penaltyTimer <= 0.0f) {
            hc->currentHeat += amount;
            
            // Garante que o calor não exceda o máximo e aplica penalidade
            if (hc->currentHeat >= hc->maxHeat) {
                hc->currentHeat = hc->maxHeat;
                hc->penaltyTimer = hc->heatPenaltyDuration;
                hc->isOverheated = true; // Define o flag de superaquecimento
                TraceLog(LOG_WARNING, "Entity %d OVERHEATED! Penalty applied for %.1fs.", entity, hc->penaltyTimer);
                
            }
        }
        
        // Sincroniza HUD 
        if ((mask & COMPONENT_COCKPIT_HUD) == COMPONENT_COCKPIT_HUD) {
            em->cockpitHUDComponents[entity].currentHeat = hc->currentHeat;
            em->cockpitHUDComponents[entity].maxHeat = hc->maxHeat;
        }
    }
}

// =================================================================
// INICIALIZAÇÃO
// =================================================================

void InitHeatSystem(struct Systems* systems) {
    TraceLog(LOG_INFO, "HeatSystem initialized.");
}

// =================================================================
// PROCESSAMENTO DE EVENTOS
// Reage ao disparo de armas para adicionar calor.
// =================================================================

void HeatSystemOnEvent(struct Systems* systems, Event event) {
    EntityManager* em = &systems->entityManager;
    
    switch (event.type) {
        case EVENT_WEAPON_FIRED: {
            Entity owner = event.data.weaponFired.owner;
            
            if ((em->componentMasks[owner] & COMPONENT_HEAT) == COMPONENT_HEAT) {
                float heatToAdd = 1.0f;
                
                // Busca o valor heatGenerated da arma que disparou.
                if ((em->componentMasks[owner] & COMPONENT_WEAPON_CONTROL) == COMPONENT_WEAPON_CONTROL) {
                    WeaponControlComponent* wcc = &em->weaponControlComponents[owner];

                    // Itera para tentar encontrar a WeaponComponent
                    // (O ideal é que o evento contenha a Entity ID da arma)
                    for (int i = 0; i < MAX_WEAPONS_EQUIP; i++) {
                        Entity slot = wcc->weaponsSlots[i];
                        if (slot != MAX_ENTITIES && (em->componentMasks[slot] & COMPONENT_WEAPON)) {
                            WeaponComponent* wc = &em->weaponComponents[slot];
                            if (wc->type == event.data.weaponFired.weapon) {
                                heatToAdd = wc->heatGenerated;
                                break; // Encontrou a arma, sai do loop
                            }
                        }
                    }
                }
                
                if (heatToAdd > 0.0f) {
                    AddHeat(systems, owner, heatToAdd);
                } 
                // NOTA: Se heatToAdd for 0 (arma sem calor ou não encontrada), não adiciona nada.
            }
            break;
        }
        default:
            break;
    }
}

// =================================================================
// ATUALIZAÇÃO DO SISTEMA
// Processa dissipação de calor e penalidades (loop principal)
// =================================================================

void HeatSystemUpdate(struct Systems* systems) {
    EntityManager* em = &systems->entityManager;
    float dt = systems->delta_time;
    
    for (Entity e = 0; e < em->numEntities; e++) {
        uint32_t mask = em->componentMasks[e];
        if ((mask & HEAT_SYSTEM_MASK) == COMPONENT_HEAT) {
            HeatComponent* hc = &em->heatComponents[e];
            
            // 1. Processa Penalidade de Superaquecimento
            if (hc->penaltyTimer > 0.0f) {
                hc->penaltyTimer -= dt;
                
                if (hc->penaltyTimer <= 0.0f) {
                    hc->penaltyTimer = 0.0f;
                    hc->isOverheated = false; // Penalidade terminou
                    TraceLog(LOG_INFO, "Entity %d penalty over. Ready to fire/move.", e);
                }
            }
            
            // Dissipação de Calor
            if (hc->currentHeat > 0.0f) {
                float dissipation = hc->dissipationRate;

                // Reduz o calor pela taxa de dissipação * delta time
                hc->currentHeat -= dissipation * dt;
                
                if (hc->currentHeat < 0.0f) {
                    hc->currentHeat = 0.0f;
                }
            }
            
            // Sincroniza com HUD
            if ((mask & COMPONENT_COCKPIT_HUD) == COMPONENT_COCKPIT_HUD) {
                em->cockpitHUDComponents[e].currentHeat = hc->currentHeat;
            }
        }
    }
}