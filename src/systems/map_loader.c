#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "map_loader.h"

// Forward declarations
static Entity FindId(EntityMap* map, int count, const char* name);
static WeaponType ParseWeaponType(const char* str);

// First pass: creates entities and adds components
static void ProcessFirstPass(FILE* file, EntityManager* em, ResourceManager* rm, MapContext context, EntityMap* idMap, int* mapCount) {
  char line[1024];
  char command[64];
  Entity currentEntity = MAX_ENTITIES;

  while (fgets(line, sizeof(line), file)) {
    if (strlen(line) >= 2 && line[0] != '#') {
      sscanf(line, "%s", command);

      if (strcmp(command, "ENTITY") == 0) {
        char uid[64];
        sscanf(line, "%*s %s", uid);
        currentEntity = CreateEntity(em);
        if (*mapCount < MAX_ENTITIES) {
          strcpy(idMap[*mapCount].name, uid);
          idMap[*mapCount].id = currentEntity;
          (*mapCount)++;
        }
      }
      else if (currentEntity != MAX_ENTITIES) {
        if (strcmp(command, "TRANSFORM") == 0) {
          Vector3 pos;
          float yaw = 0.0f;
          // TRANSFORM x y z [yaw] - yaw is optional rotation in radians (can use "PI" for 3.14159)
          char yawStr[64] = {0};
          int count = sscanf(line, "%*s %f %f %f %s", &pos.x, &pos.y, &pos.z, yawStr);
          AddTransformComponent(em, currentEntity, pos);
          if (count == 4 && em->componentMasks[currentEntity] & COMPONENT_TRANSFORM) {
            if (strcmp(yawStr, "PI") == 0) {
              yaw = PI;
            }
            else {
              yaw = strtof(yawStr, NULL);
            }
            em->transformComponents[currentEntity].orientation = QuaternionFromAxisAngle((Vector3){0, 1, 0}, yaw);
          }
        }
        else if (strcmp(command, "PHYSICS") == 0) {
          Vector3 vel; float drag;
          sscanf(line, "%*s %f %f %f %f", &vel.x, &vel.y, &vel.z, &drag);
          AddPhysicsComponent(em, currentEntity, vel, drag);
        }
        else if (strcmp(command, "RENDER") == 0) {
          int modelIdVal, r, g, b, a;
          sscanf(line, "%*s %d %d %d %d %d", &modelIdVal, &r, &g, &b, &a);
          AssetModelID modelId = (AssetModelID)modelIdVal;
          Model* m = GetModel(rm, modelId);
          if (m) {
            AddRenderComponent(em, currentEntity, m, (Color){r,g,b,a});
            if (modelId == MODEL_ID_ENEMY_SCOUT || modelId == MODEL_ID_ENEMY_FIGHTER || modelId == MODEL_ID_ENEMY_BOSS) {
              AddAnimationComponent(em, currentEntity, modelId, 0, 48.0f, true);
            }
          }
        }
        else if (strcmp(command, "COLLISION") == 0) {
          Vector3 min, max; int isStat, isTrig;
          sscanf(line, "%*s %f %f %f %f %f %f %d %d", &min.x, &min.y, &min.z, &max.x, &max.y, &max.z, &isStat, &isTrig);
          AddCollisionComponent(em, currentEntity, (BoundingBox){min, max}, isStat, isTrig);
        }
        else if (strcmp(command, "PLAYER_CONTROL") == 0) {
          if (context.mainCamera != NULL) {
            AddPlayerControlComponent(em, currentEntity, context.mainCamera);
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
          AddAIControlComponent(em, currentEntity, sight, range, NULL, 0);
          AddWeaponControlComponent(em, currentEntity, AIM_MODE_PHYSICAL);
          // Orientation will be set via TRANSFORM command in level1.map if needed
        }
        else if (strcmp(command, "AI_PATROL") == 0) {
          int count;
          char* ptr = line + strlen("AI_PATROL");
          count = (int)strtof(ptr, &ptr);
          if (count > 0) {
            Vector3* points = (Vector3*)malloc(sizeof(Vector3) * count);
            for(int i=0; i<count; i++) {
              points[i].x = strtof(ptr, &ptr);
              points[i].y = strtof(ptr, &ptr);
              points[i].z = strtof(ptr, &ptr);
            }
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
          sscanf(line, "%*s %s %f %f %f %f %f %d %f", typeStr, &rate, &speed, &dmg, &range, &heat, &bTotal, &bRate);
          AddWeaponComponent(em, currentEntity, ParseWeaponType(typeStr), rate, speed, dmg, range, heat, bTotal, bRate);
        }
      }
    }
  }
}

// Second pass: handles attachments and weapon links
static void ProcessSecondPass(FILE* file, EntityManager* em, EntityMap* idMap, int mapCount) {
  char line[1024];
  char command[64];
  Entity currentEntity = MAX_ENTITIES;

  while (fgets(line, sizeof(line), file)) {
    if (strlen(line) >= 2 && line[0] != '#') {
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
          char s[6][64];
          for(int i=0; i<6; i++) s[i][0] = '\0';
          int count = sscanf(line, "%*s %s %s %s %s %s %s", s[0], s[1], s[2], s[3], s[4], s[5]);
          WeaponControlComponent* wc = &em->weaponControlComponents[currentEntity];
          for(int i=0; i<count; i++) {
            Entity weaponID = FindId(idMap, mapCount, s[i]);
            if (weaponID != MAX_ENTITIES) {
              wc->weaponsSlots[i] = weaponID;
              if (i < 2) {
                wc->weaponsGroupMap[i] = 0;
              }
              else {
                wc->weaponsGroupMap[i] = 1;
              }
              wc->activeGroup[wc->weaponsGroupMap[i]] = true;
            }
          }
        }
        else if (strcmp(command, "LINK_WEAPON") == 0) {
          char ownerName[64], weaponName[64];
          int slot, group, isActive;
          int matches = sscanf(line, "%*s %s %d %s %d %d", ownerName, &slot, weaponName, &group, &isActive);

          if (matches == 5) {
            Entity ownerID = FindId(idMap, mapCount, ownerName);
            Entity weaponID = FindId(idMap, mapCount, weaponName);

            if (ownerID >= MAX_ENTITIES || weaponID >= MAX_ENTITIES) {
              TraceLog(LOG_WARNING, "MAP: LINK_WEAPON failed. Entities '%s' or '%s' not found.", ownerName, weaponName);
            }
            else {
              if (!(em->componentMasks[ownerID] & COMPONENT_WEAPON_CONTROL)) {
                AddWeaponControlComponent(em, ownerID, AIM_MODE_PHYSICAL);
              }

              WeaponControlComponent* wc = &em->weaponControlComponents[ownerID];
              if (slot >= 0 && slot < MAX_WEAPONS_EQUIP) {
                wc->weaponsSlots[slot] = weaponID;
                wc->weaponsGroupMap[slot] = group;
                if (group >= 0 && group < MAX_WEAPONS_GROUPS && isActive) {
                  wc->activeGroup[group] = true;
                }
              }
            }
          }
        }
      }
    }
  }
}

void LoadMapFromText(EntityManager* em, ResourceManager* rm, const char* filename, MapContext context) {
  FILE* file = fopen(filename, "r");
  if (!file) {
    TraceLog(LOG_ERROR, "MAP: FATAL ERROR - FILE NOT FOUND: %s", filename);
    return;
  }

  EntityMap* idMap = (EntityMap*)malloc(MAX_ENTITIES * sizeof(EntityMap));
  if (idMap == NULL) {
    fclose(file);
    return;
  }

  int mapCount = 0;
  ProcessFirstPass(file, em, rm, context, idMap, &mapCount);

  rewind(file);
  ProcessSecondPass(file, em, idMap, mapCount);

  free(idMap);
  fclose(file);
  TraceLog(LOG_INFO, "MAP: Map '%s' loaded successfully!", filename);
}

// Finds ID by name
static Entity FindId(EntityMap* map, int count, const char* name) {
  for(int i=0; i<count; i++) {
    if(strcmp(map[i].name, name) == 0) return map[i].id;
  }
  return MAX_ENTITIES;
}

static WeaponType ParseWeaponType(const char* str) {
  if (strcmp(str, "WEAPON_PULSE_LASER") == 0) return WEAPON_PULSE_LASER;
  if (strcmp(str, "WEAPON_MISSILE_LAUNCHER") == 0) return WEAPON_MISSILE_LAUNCHER;
  if (strcmp(str, "WEAPON_MACHINE_GUN") == 0) return WEAPON_MACHINE_GUN;
  if (strcmp(str, "WEAPON_LASER_BEAM") == 0) return WEAPON_LASER_BEAM;
  return 0;
}
