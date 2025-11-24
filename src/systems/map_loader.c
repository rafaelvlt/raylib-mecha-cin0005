#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map_loader.h"

// Tabela simples para mapear Nomes -> IDs
typedef struct {
    char name[64];
    Entity id;
} EntityMap;

// Helper para achar ID pelo nome
Entity FindId(EntityMap* map, int count, const char* name) {
    for(int i=0; i<count; i++) {
        if(strcmp(map[i].name, name) == 0) return map[i].id;
    }
    return MAX_ENTITIES;
}

WeaponType ParseWeaponType(const char* str) {
    if (strcmp(str, "WEAPON_PULSE_LASER") == 0) return WEAPON_PULSE_LASER;
    if (strcmp(str, "WEAPON_MISSILE_LAUNCHER") == 0) return WEAPON_MISSILE_LAUNCHER;
    if (strcmp(str, "WEAPON_MACHINE_GUN") == 0) return WEAPON_MACHINE_GUN;
    if (strcmp(str, "WEAPON_LASER_BEAM") == 0) return WEAPON_LASER_BEAM;
    return 0;
}

void LoadMapFromText(EntityManager* em, ResourceManager* rm, const char* filename, MapContext context) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        TraceLog(LOG_ERROR, "MAP: ERRO FATAL - Arquivo nao encontrado: %s", filename);
        return;
    }

    char line[1024]; // Buffer aumentado para caber os pontos de patrulha
    char command[64];
    
    Entity currentEntity = MAX_ENTITIES;
    
    EntityMap* idMap = (EntityMap*)malloc(MAX_ENTITIES * sizeof(EntityMap));
    int mapCount = 0;

    if (idMap == NULL) {
        fclose(file);
        return;
    }

    // =========================================================
    // PASSADA 1: Criar Entidades e Componentes
    // =========================================================
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 2 || line[0] == '#') continue;
        sscanf(line, "%s", command);

        if (strcmp(command, "ENTITY") == 0) {
            char uid[64];
            sscanf(line, "%*s %s", uid);
            
            currentEntity = CreateEntity(em);
            
            if (mapCount < MAX_ENTITIES) {
                strcpy(idMap[mapCount].name, uid);
                idMap[mapCount].id = currentEntity;
                mapCount++;
            }
        }
        else if (currentEntity != MAX_ENTITIES) {
            
            if (strcmp(command, "TRANSFORM") == 0) {
                Vector3 pos;
                sscanf(line, "%*s %f %f %f", &pos.x, &pos.y, &pos.z);
                AddTransformComponent(em, currentEntity, pos);
            }
            else if (strcmp(command, "PHYSICS") == 0) {
                Vector3 vel; float drag;
                sscanf(line, "%*s %f %f %f %f", &vel.x, &vel.y, &vel.z, &drag);
                AddPhysicsComponent(em, currentEntity, vel, drag);
            }
            else if (strcmp(command, "RENDER") == 0) {
                int modelIdVal, r, g, b, a;
                sscanf(line, "%*s %d %d %d %d %d", &modelIdVal, &r, &g, &b, &a);
                Model* m = GetModel(rm, (AssetModelID)modelIdVal); 
                if (m) AddRenderComponent(em, currentEntity, m, (Color){r,g,b,a});
            }
            else if (strcmp(command, "COLLISION") == 0) {
                 Vector3 min, max; int isStat, isTrig;
                 sscanf(line, "%*s %f %f %f %f %f %f %d %d", &min.x, &min.y, &min.z, &max.x, &max.y, &max.z, &isStat, &isTrig);
                 // Ajuste conforme a assinatura atual do seu AddCollisionComponent
                 AddCollisionComponent(em, currentEntity, (BoundingBox){min, max}, isStat, isTrig);
            }
            // --- COMPONENTES ESPECÍFICOS DE JOGO ---
            else if (strcmp(command, "PLAYER_CONTROL") == 0) {
                if (context.mainCamera != NULL) {
                    AddPlayerControlComponent(em, currentEntity, context.mainCamera);
                    // Player sempre tem controle de arma (Modo Camera)
                    AddWeaponControlComponent(em, currentEntity, AIM_MODE_CAMERA);
                }
            }
            else if (strcmp(command, "HUD") == 0) {
                 float maxH, heatS, cool;
                 sscanf(line, "%*s %f %f %f", &maxH, &heatS, &cool);
                 AddCockpitHUDComponent(em, currentEntity, maxH, heatS, cool);
            }
            else if (strcmp(command, "HEALTH") == 0) {
                float hp;
                sscanf(line, "%*s %f", &hp);
                AddHealthComponent(em, currentEntity, hp);
            }
            else if (strcmp(command, "AI_CONTROL") == 0) {
                float sight, range;
                sscanf(line, "%*s %f %f", &sight, &range);
                // Inicializa IA sem patrulha por padrão (será preenchido pelo AI_PATROL se houver)
                AddAIControlComponent(em, currentEntity, sight, range, NULL, 0);
                
                // Inimigos precisam de WeaponControl (Modo Físico)
                AddWeaponControlComponent(em, currentEntity, AIM_MODE_PHYSICAL);
            }
            else if (strcmp(command, "AI_PATROL") == 0) {
                // Lê pontos de patrulha: AI_PATROL <NumPoints> <x y z> <x y z> ...
                int count;
                char* ptr = line + strlen("AI_PATROL"); // Avança ponteiro na string
                count = (int)strtof(ptr, &ptr);

                if (count > 0) {
                    // Aloca memória para os pontos (Importante: O sistema de AI deve gerenciar isso ou ser um array fixo)
                    // Assumindo que AddAIControlComponent copia os dados ou aceita ponteiro.
                    // Se AIControlComponent usa ponteiro, este malloc deve ser liberado no fim do jogo.
                    Vector3* points = (Vector3*)malloc(sizeof(Vector3) * count);
                    
                    for(int i=0; i<count; i++) {
                        points[i].x = strtof(ptr, &ptr);
                        points[i].y = strtof(ptr, &ptr);
                        points[i].z = strtof(ptr, &ptr);
                    }

                    // Atualiza o componente de IA já existente
                    if (em->componentMasks[currentEntity] & COMPONENT_AI_CONTROL) {
                        em->aiControlComponents[currentEntity].patrolPoints = points;
                        em->aiControlComponents[currentEntity].numPatrolPoints = count;
                    }
                }
            }
            else if (strcmp(command, "WEAPON") == 0) {
                char typeStr[64];
                float rate, speed, dmg, range, heat, bRate;
                int bTotal;
                
                // Formato Completo: WEAPON TYPE RATE SPEED DMG RANGE HEAT BURST_TOT BURST_RATE SND MDL
                sscanf(line, "%*s %s %f %f %f %f %f %d %f", 
                       typeStr, &rate, &speed, &dmg, &range, &heat, &bTotal, &bRate);
                
                AddWeaponComponent(em, currentEntity, ParseWeaponType(typeStr), 
                                   rate, speed, dmg, range, heat, 
                                   bTotal, bRate);
            }
        }
    }

    // =========================================================
    // PASSADA 2: Vínculos (Attachments e Slots)
    // =========================================================
    rewind(file); 
    currentEntity = MAX_ENTITIES; 

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 2 || line[0] == '#') continue;
        sscanf(line, "%s", command);

        if (strcmp(command, "ENTITY") == 0) {
            char name[64];
            sscanf(line, "%*s %s", name);
            currentEntity = FindId(idMap, mapCount, name);
        }
        else if (currentEntity != MAX_ENTITIES) {
            
            if (strcmp(command, "ATTACHMENT") == 0) {
                char parentName[64];
                Vector3 off;
                sscanf(line, "%*s %s %f %f %f", parentName, &off.x, &off.y, &off.z);
                
                Entity parentID = FindId(idMap, mapCount, parentName);
                if (parentID != MAX_ENTITIES) {
                     AddAttachmentComponent(em, currentEntity, parentID, off, QuaternionIdentity());
                }
            }
            else if (strcmp(command, "WEAPON_SLOTS") == 0) {
                // Lê até 4 slots de armas
                char s[4][64];
                // Inicializa strings vazias
                for(int i=0; i<4; i++) s[i][0] = '\0';
                
                // Lê quantos slots tiver na linha
                int count = sscanf(line, "%*s %s %s %s %s", s[0], s[1], s[2], s[3]);
                
                WeaponControlComponent* wc = &em->weaponControlComponents[currentEntity];
                
                // Configura Slots e Grupos
                // Padrão: Slot 0,1 = Grupo 0 (Pri). Slot 2,3 = Grupo 1 (Sec).
                for(int i=0; i<count; i++) {
                    Entity weaponID = FindId(idMap, mapCount, s[i]);
                    if (weaponID != MAX_ENTITIES) {
                        wc->weaponsSlots[i] = weaponID;
                        // Lógica de grupos simples: 0 e 1 são G0, 2 e 3 são G1
                        int group = (i < 2) ? 0 : 1;
                        wc->weaponsGroupMap[i] = group;
                        wc->activeGroup[group] = true; // Ativa o grupo por padrão
                    }
                }
            }

            else if (strcmp(command, "LINK_WEAPON") == 0) {
                char ownerName[64];
                char weaponName[64];
                int slot, group, isActive;
                
                // Lê: NomeDono Slot NomeArma Grupo Ativo
                int matches = sscanf(line, "%*s %s %d %s %d %d", 
                                    ownerName, &slot, weaponName, &group, &isActive);
                
                if (matches == 5) {
                    // 1. Busca os IDs pelo nome
                    Entity ownerID = FindId(idMap, mapCount, ownerName);
                    Entity weaponID = FindId(idMap, mapCount, weaponName);
                    
                    // 2. Valida se existem e se o dono tem controle de armas
                    if (ownerID < MAX_ENTITIES && weaponID < MAX_ENTITIES) {
                        
                        // Garante que tem o componente (se não tiver, adiciona)
                        if (!(em->componentMasks[ownerID] & COMPONENT_WEAPON_CONTROL)) {
                            // Adiciona com um modo padrão se faltar (ex: CAMERA para player)
                            // Mas idealmente já foi adicionado na Passada 1 via "PLAYER_CONTROL" ou "WEAPON_CONTROL"
                            AddWeaponControlComponent(em, ownerID, AIM_MODE_PHYSICAL); 
                        }

                        WeaponControlComponent* wc = &em->weaponControlComponents[ownerID];
                        
                        // 3. Realiza o Vínculo (A lógica que estava hardcoded no Init)
                        if (slot >= 0 && slot < MAX_WEAPONS_EQUIP) {
                            wc->weaponsSlots[slot] = weaponID;
                            wc->weaponsGroupMap[slot] = group;
                            
                            if (group >= 0 && group < MAX_WEAPONS_GROUPS) {
                                if (isActive) {
                                    wc->activeGroup[group] = true;
                                }
                            }
                        }
                    } else {
                        TraceLog(LOG_WARNING, "MAP: LINK_WEAPON falhou. Entidades '%s' ou '%s' nao encontradas.", ownerName, weaponName);
                    }
                }
            }
        }
    }

    free(idMap);
    fclose(file);
    TraceLog(LOG_INFO, "MAP: Mapa '%s' carregado com sucesso.", filename);
}