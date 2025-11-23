#include "map_loader.h"
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

// Helper para converter string do arquivo em WeaponType
WeaponType ParseWeaponType(const char* str) {
    if (strcmp(str, "WEAPON_PULSE_LASER") == 0) return WEAPON_PULSE_LASER;
    // Adicione outros tipos aqui (Missil, Canhao, etc)
    return 0;
}

void LoadMapFromText(EntityManager* em, ResourceManager* rm, const char* filename, MapContext context) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        TraceLog(LOG_ERROR, "MAP: ERRO FATAL - Arquivo nao encontrado: %s", filename);
        return;
    }

    char line[256];
    char command[64];
    
    Entity currentEntity = MAX_ENTITIES;
    
    // Alocação dinâmica para evitar Stack Overflow
    EntityMap* idMap = (EntityMap*)malloc(MAX_ENTITIES * sizeof(EntityMap));
    int mapCount = 0;

    if (idMap == NULL) {
        TraceLog(LOG_ERROR, "MAP: Erro de memoria ao alocar idMap");
        fclose(file);
        return;
    }

    // =========================================================
    // PASSO 1: Criar Entidades e Componentes Básicos
    // =========================================================
    while (fgets(line, sizeof(line), file)) {
        // Ignora linhas curtas ou comentários
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
                Vector3 vel;
                float drag;
                sscanf(line, "%*s %f %f %f %f", &vel.x, &vel.y, &vel.z, &drag);
                AddPhysicsComponent(em, currentEntity, vel, drag);
            }
            else if (strcmp(command, "RENDER") == 0) {
                // CORREÇÃO: Lê o ID do modelo como inteiro
                int modelIdVal; 
                int r, g, b, a;
                // Formato: RENDER <ID> <R> <G> <B> <A>
                sscanf(line, "%*s %d %d %d %d %d", &modelIdVal, &r, &g, &b, &a);
                
                // Busca o modelo usando o ID do Enum
                Model* m = GetModel(rm, (AssetModelID)modelIdVal); 
                
                if (m != NULL) {
                    AddRenderComponent(em, currentEntity, m, (Color){r,g,b,a});
                } else {
                    TraceLog(LOG_WARNING, "MAP: Modelo ID %d invalido ou nao carregado.", modelIdVal);
                }
            }
            else if (strcmp(command, "PLAYER_CONTROL") == 0) {
                if (context.mainCamera != NULL) {
                    AddPlayerControlComponent(em, currentEntity, context.mainCamera);
                    AddWeaponControlComponent(em, currentEntity, AIM_MODE_CAMERA);
                }
            }
            else if (strcmp(command, "AI_CONTROL") == 0) {
                float sight, range;
                sscanf(line, "%*s %f %f", &sight, &range);
                AddAIControlComponent(em, currentEntity, sight, range);
                // Inimigos também precisam de WeaponControl para atirar
                AddWeaponControlComponent(em, currentEntity, 0);
            }
            else if (strcmp(command, "HEALTH") == 0) {
                float hp;
                sscanf(line, "%*s %f", &hp);
                AddHealthComponent(em, currentEntity, hp);
            }
            else if (strcmp(command, "COLLISION") == 0) {
                 Vector3 min, max;
                 sscanf(line, "%*s %f %f %f %f %f %f", &min.x, &min.y, &min.z, &max.x, &max.y, &max.z);
                 AddCollisionComponent(em, currentEntity, (BoundingBox){min, max}, false, false);
            }
            else if (strcmp(command, "WEAPON") == 0) {
                char typeStr[64];
                float rate, speed, dmg, range, heat;
                int sndId, mdlId; // Agora são inteiros para os IDs
                
                // Formato: WEAPON <TYPE> <RATE> <SPEED> <DMG> <RANGE> <HEAT> <SND_ID> <MDL_ID>
                sscanf(line, "%*s %s %f %f %f %f %f %d %d", 
                       typeStr, &rate, &speed, &dmg, &range, &heat, &sndId, &mdlId);
                
                AddWeaponComponent(em, currentEntity, ParseWeaponType(typeStr), 
                                   rate, speed, dmg, range, heat, 
                                   (AssetSoundID)sndId, (AssetModelID)mdlId);
            }
        }
    }

    // =========================================================
    // PASSO 2: Links e Attachments (Rewind)
    // =========================================================
    rewind(file); 
    currentEntity = MAX_ENTITIES; 
    char tempName[64];

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 2 || line[0] == '#') continue;
        sscanf(line, "%s", command);

        if (strcmp(command, "ENTITY") == 0) {
            sscanf(line, "%*s %s", tempName);
            currentEntity = FindId(idMap, mapCount, tempName);
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
                char s1[64], s2[64];
                int c = sscanf(line, "%*s %s %s", s1, s2);
                WeaponControlComponent* wc = &em->weaponControlComponents[currentEntity];
                
                if (c >= 1) { wc->weaponsSlots[0] = FindId(idMap, mapCount, s1); wc->activeGroup[0] = true; }
                if (c >= 2) { wc->weaponsSlots[1] = FindId(idMap, mapCount, s2); }
            }
        }
    }

    free(idMap);
    fclose(file);
    TraceLog(LOG_INFO, "MAP: Mapa carregado com sucesso.");
}