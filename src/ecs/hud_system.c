#include <raylib.h>
#include <raymath.h>
#include <rlgl.h> 
#include <string.h>
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"
#include "utility.h"
#include <stdio.h>
#include "ecs/components.h"
#include "state_manager.h"
#include "screens/screen_first_level.h"
#include "screens/screen_second_level.h"

// HUD Visuals
#define BAR_WIDTH 400.0f
#define BAR_HEIGHT 25.0f
#define SQUARE_SIDE 75.0f
#define SPACE_BTW_SQUARES 15.0f
#define MARGIN_TOP 15.0f
#define COOLDOWN_BAR_HEIGHT 5.0f // Nova constante para a altura da barra de cooldown
#define TARGET_MARKER_HEIGHT_OFFSET 2.0f

// Helper Functions
static void DrawBar(float x, float y, float width, float height, float current, float max, Color fillColor, Color outlineColor, Color background, const char* label, float fontSize, const char* unit);
static void DrawLBracket(Vector3 corner, Vector3 rightEnd, Vector3 upEnd, Color color, float lineThickness, Vector3 camUp, Vector3 camRight, float groundLevel);
static Vector3 FindPlayerPosition(EntityManager* em, float* outYaw, Camera* camera);
static void DrawMinimapBackground(int mapCenterX, int mapCenterY, int mapSize, int mapX, int mapY);
static void DrawMinimapEntities(EntityManager* em, Entity player, Vector3 playerPos, float cosYaw, float sinYaw, float scaleFactor, int mapCenterX, int mapCenterY, int mapSize);


// Target lock helper functions
static void CalculateCameraBasis(Camera* camera, Vector3* outForward, Vector3* outRight, Vector3* outUp);
static Vector3 ClampToGround(Vector3 pos, float minHeight);
static void DrawCornerBracket(Vector3 center, Vector3 rightDir, Vector3 upDir, float rightScale, float upScale, float bracketLength, Color color, float lineThickness, Vector3 camUp, Vector3 camRight, float groundLevel, float minHeight);



//  =======
//  HP BAR
//  =======


void DrawHPBar(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  uint32_t neededMask = COMPONENT_PLAYER_CONTROL | COMPONENT_HEALTH;

  for (Entity e = 0; e < em->numEntities; e++) {
    if ((em->componentMasks[e] & neededMask) == neededMask) {
      HealthComponent* hp = &em->healthComponents[e];
      float screenW = (float)GetScreenWidth();
      float x_center = (screenW - BAR_WIDTH) / 2.0f;
      float y_pos = MARGIN_TOP;

      Color outlineColor = HUD_CYAN_OUTLINE;
      Color fillColor = HUD_BLUE_FILL;

      if (hp->currentHealth < hp->maxHealth * 0.6f) {
        fillColor = HUD_ORANGE_FILL;
        outlineColor = ORANGE;
      }

      if (hp->currentHealth < hp->maxHealth * 0.3f) {
        fillColor = HUD_RED_FILL;
        outlineColor = RED;
      }

      DrawBar(x_center, y_pos, BAR_WIDTH, BAR_HEIGHT, hp->currentHealth, hp->maxHealth, fillColor, outlineColor, Fade(BLACK, 0.6f), "HP", 20.0, "%");
    }
  }
}


//  ========
//  HEAT BAR
//  ========


void DrawHeatBar(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  uint32_t neededMask = COMPONENT_PLAYER_CONTROL | COMPONENT_HEALTH;

  for (Entity e = 0; e < em->numEntities; e++) {
    if ((em->componentMasks[e] & neededMask) == neededMask) {
      CockpitHUDComponent* cock = &em->cockpitHUDComponents[e];
      float screenW = (float)GetScreenWidth();
      float x_center = (screenW - BAR_WIDTH) / 2.0f;
      float y_pos =  (float)GetScreenHeight() - 3 * MARGIN_TOP;

      Color outlineColor = WHITE;
      Color fillColor = HUD_GRAY_FILL;

      if (cock->currentHeat >= cock->maxHeat * 0.75f) {
        fillColor = HUD_ORANGE_FILL;
        outlineColor = RED;
      }

      if (cock->currentHeat < cock->maxHeat * 0.75f) {
        fillColor = HUD_ORANGE_FILL;
        outlineColor = ORANGE;
      }

      DrawBar(x_center, y_pos, BAR_WIDTH, BAR_HEIGHT / 2.0, cock->currentHeat, cock->maxHeat, fillColor, outlineColor, Fade(BLACK, 0.6f), "HEAT", 14.0, "%");
    }
  }
}



//  ==============================
//  AUXILIARY BAR DRAWING FUNCTION
//  ==============================


static void DrawBar(float x, float y, float width, float height, float current, float max, Color fillColor, Color outlineColor, Color background, const char* label, float fontSize, const char* unit) {
  if (max <= 0.0f) max = 1.0f;
  float percentage = current / max;
  if (percentage < 0.0f) percentage = 0.0f;
  if (percentage > 1.0f) percentage = 1.0f;
  float fillWidth = width * percentage;
  // Size of the decorative corner
  const float cornerSize = 12.0f;
  // Line thickness for the outline
  const float thickness = 2.0f;

  // Draw background and fill bar
  DrawRectangle(x, y, width, height, background);
  DrawRectangle(x, y, fillWidth, height, fillColor);

  // Draw decorative corner brackets: top and bottom horizontal lines (skip corners)
  DrawLineEx((Vector2){x + cornerSize, y}, (Vector2){x + width - cornerSize, y}, thickness, outlineColor);
  DrawLineEx((Vector2){x + cornerSize, y + height}, (Vector2){x + width - cornerSize, y + height}, thickness, outlineColor);

  // Left side: diagonal corner lines and vertical line
  DrawLineEx((Vector2){x, y + cornerSize}, (Vector2){x + cornerSize, y}, thickness, outlineColor);
  DrawLineEx((Vector2){x, y + height - cornerSize}, (Vector2){x + cornerSize, y + height}, thickness, outlineColor);
  DrawLineEx((Vector2){x, y + cornerSize}, (Vector2){x, y + height - cornerSize}, thickness, outlineColor);

  // Right side: diagonal corner lines and vertical line
  DrawLineEx((Vector2){x + width - cornerSize, y}, (Vector2){x + width, y + cornerSize}, thickness, outlineColor);
  DrawLineEx((Vector2){x + width - cornerSize, y + height}, (Vector2){x + width, y + height - cornerSize}, thickness, outlineColor);
  DrawLineEx((Vector2){x + width, y + cornerSize}, (Vector2){x + width, y + height - cornerSize}, thickness, outlineColor);

  // Text label: convert percentage to 0-100 range and center it
  char text[64];
  float value = percentage * 100.0f;
  snprintf(text, 64, "%s %.0f%s", label, value, unit);

  int textWidth = MeasureText(text, fontSize);
  // Draw text with shadow then white text on top
  DrawText(text, x + (width / 2) - (textWidth / 2) + 1, y + (height / 2) - (fontSize / 2) + 1, fontSize, BLACK);
  DrawText(text, x + (width / 2) - (textWidth / 2), y + (height / 2) - (fontSize / 2), fontSize, WHITE);
}


//  ==========
//  CROSSHAIR
//  ==========

// Simple crosshair draw
void DrawCrosshair(struct Systems* systems){
  ConfigManager* cfg = &systems->configManager;
  Texture crosshair = *GetTexture(&systems->resourceManager, TEXTURE_ID_CROSSHAIR_SPRITE);

  int screenW = cfg->screenResolution.x;
  int screenH = cfg->screenResolution.y;

  DrawTexture(crosshair, screenW/2 - crosshair.width/2, screenH/2 - crosshair.height/2, WHITE);
}


//  ================
//  MINIMAP SYSTEMS
//  ================


// Finds player position and calculates yaw from camera
static Vector3 FindPlayerPosition(EntityManager* em, float* outYaw, Camera* camera) {
  Vector3 playerPos = {0};
  *outYaw = 0.0f;

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
      if (em->componentMasks[i] & COMPONENT_TRANSFORM) {
        playerPos = em->transformComponents[i].position;
        // Calculate camera forward direction and convert to yaw angle
        // atan2 gives angle from X axis, we subtract PI/2 to align with game's coordinate system
        Vector3 forward = Vector3Subtract(camera->target, camera->position);
        *outYaw = -atan2f(forward.z, forward.x) - PI/2.0f;
        return playerPos;
      }
    }
  }
  return playerPos;
}

// Draws minimap background and grid
static void DrawMinimapBackground(int mapCenterX, int mapCenterY, int mapSize, int mapX, int mapY) {
  DrawCircle(mapCenterX, mapCenterY, mapSize/2, Fade(DARKBLUE, 0.6f));
  DrawCircleLines(mapCenterX, mapCenterY, mapSize/2, Fade(SKYBLUE, 0.8f));
  DrawLine(mapCenterX - mapSize/2, mapCenterY, mapCenterX + mapSize/2, mapCenterY, Fade(SKYBLUE, 0.2f));
  DrawLine(mapCenterX, mapCenterY - mapSize/2, mapCenterX, mapCenterY + mapSize/2, Fade(SKYBLUE, 0.2f));
  DrawText("RADAR", mapX + mapSize/2 - MeasureText("RADAR", 10)/2, mapY + mapSize + 5, 10, SKYBLUE);
}

// Draws entities on the minimap
static void DrawMinimapEntities(EntityManager* em, Entity player, Vector3 playerPos, float cosYaw, float sinYaw, float scaleFactor, int mapCenterX, int mapCenterY, int mapSize) {
  uint32_t mapMask = COMPONENT_TRANSFORM;

  for (Entity e = 0; e < em->numEntities; e++) {
    if ((em->componentMasks[e] & mapMask) == mapMask && e != player) {
      TransformComponent* trans = &em->transformComponents[e];
      Vector3 worldPos = trans->position;

      // Calculate relative position from player 
      float dx = worldPos.x - playerPos.x;
      float dz = worldPos.z - playerPos.z;

      // Rotate coordinates to match player's facing direction
      float rotatedX = dx * cosYaw - dz * sinYaw;
      float rotatedY = dx * sinYaw + dz * cosYaw;

      // Convert world coordinates to screen coordinates on minimap
      int drawX = mapCenterX + (int)(rotatedX * scaleFactor);
      int drawY = mapCenterY + (int)(rotatedY * scaleFactor);

      // Only draw if entity is within minimap circle bounds
      float distFromCenter = sqrtf(powf(drawX - mapCenterX, 2) + powf(drawY - mapCenterY, 2));
      if (distFromCenter <= mapSize / 2) {
        // Draw enemy mechs as small red circles
        if (em->componentMasks[e] & COMPONENT_AI_CONTROL) {
          DrawCircle(drawX, drawY, 4.0f, RED);
        }

        // Objective structure (hardcoded position check for turret at Y=-45, Z=-650)
        if (worldPos.y == -45.0f && worldPos.z == -650.0f) {
          DrawCircle(drawX, drawY, 8.0f, ORANGE);
        }
      }
    }
  }
}


// Minimap system (Player-Centered)
void DrawMinimapSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  //Agora chama stateManager para poder depender da screen (level) atual
  StateManager* state = &systems->stateManager;
  Camera* currentCamera = NULL;

  if (state->currentScreen == SCREEN_FIRST_LEVEL) {
        //Camera level one
        currentCamera = &state->data.firstLevel.camera;
    } else if (state->currentScreen == SCREEN_SECOND_LEVEL) {
        //Camera level two
        currentCamera = &state->data.secondLevel.camera;
    } else {
        return;
    }

  // Get player position and camera yaw for rotation calculations
  float playerYaw = 0.0f;
  Vector3 playerPos = FindPlayerPosition(em, &playerYaw, currentCamera);
  if (Vector3LengthSqr(playerPos) < 0.001f) return;

  // Find player entity ID
  Entity player = MAX_ENTITIES;
  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) == COMPONENT_PLAYER_CONTROL) {
      player = i;
      break;
    }
  }

  // Minimap dimensions and positioning

  // Diameter of minimap circle in pixels
  const int mapSize = 180;
  // Distance from screen edges
  const int mapPadding = 20;
  // World space size represented by minimap (in game units)
  const float mapWorldSize = 1100.0f;

  // Position minimap in top-right corner
  int screenW = GetScreenWidth();
  int mapX = screenW - mapSize - mapPadding;
  // Small extra offset from top
  int mapY = mapPadding + 10;
  int mapCenterX = mapX + mapSize / 2;
  int mapCenterY = mapY + mapSize / 2;

  DrawMinimapBackground(mapCenterX, mapCenterY, mapSize, mapX, mapY);

  // Calculate scale factor: how many pixels per world unit
  // Pre-calculate sin/cos for rotation (used for all entities)
  float scaleFactor = (float)mapSize / mapWorldSize;
  float cosYaw = cosf(playerYaw);
  float sinYaw = sinf(playerYaw);

  DrawMinimapEntities(em, player, playerPos, cosYaw, sinYaw, scaleFactor, mapCenterX, mapCenterY, mapSize);

  // Draw player indicator as yellow triangle pointing up (3 sides, 6px radius, -90 degree rotation)
  Vector2 playerScreenPos = { (float)mapCenterX, (float)mapCenterY };
  DrawPoly(playerScreenPos, 3, 6.0f, -90.0f, YELLOW);
}


//  =========================================
//  TARGET LOCK SYSTEMS & AUXILIAR FUNCTIONS
//  =========================================


// Calculates camera-relative coordinate system (forward, right, up vectors)
static void CalculateCameraBasis(Camera* camera, Vector3* outForward, Vector3* outRight, Vector3* outUp) {
  *outForward = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
  *outRight = Vector3Normalize(Vector3CrossProduct(*outForward, camera->up));
  *outUp = Vector3Normalize(Vector3CrossProduct(*outRight, *outForward));
}

// Clamps a position to be at least minHeight above ground
static Vector3 ClampToGround(Vector3 pos, float minHeight) {
  if (pos.y < minHeight) pos.y = minHeight;
  return pos;
}

// Draws a single L-bracket at a corner position
static void DrawLBracket(Vector3 corner, Vector3 rightEnd, Vector3 upEnd, Color color, float lineThickness, Vector3 camUp, Vector3 camRight, float groundLevel) {
  corner = ClampToGround(corner, groundLevel);
  rightEnd = ClampToGround(rightEnd, groundLevel);
  upEnd = ClampToGround(upEnd, groundLevel);

  // Draw 3 lines with offset to create thickness effect
  for (int offset = -1; offset <= 1; offset++) {
    Vector3 offsetRight = Vector3Scale(camUp, offset * lineThickness);
    Vector3 offsetUp = Vector3Scale(camRight, offset * lineThickness);
    Vector3 cornerOffset = ClampToGround(Vector3Add(corner, offsetRight), groundLevel);
    Vector3 rightEndOffset = ClampToGround(Vector3Add(rightEnd, offsetRight), groundLevel);
    Vector3 upEndOffset = ClampToGround(Vector3Add(upEnd, offsetUp), groundLevel);

    DrawLine3D(cornerOffset, rightEndOffset, color);
    DrawLine3D(cornerOffset, upEndOffset, color);
  }
}


// Draws a corner bracket at the specified position relative to center
// rightScale and upScale determine which corner (positive/negative values)
static void DrawCornerBracket(Vector3 center, Vector3 rightDir, Vector3 upDir, float rightScale, float upScale, float bracketLength, Color color, float lineThickness, Vector3 camUp, Vector3 camRight, float groundLevel, float minHeight) {
  Vector3 corner = Vector3Add(center, Vector3Add(Vector3Scale(rightDir, rightScale), Vector3Scale(upDir, upScale)));
  corner = ClampToGround(corner, minHeight);

  Vector3 rightEnd = Vector3Add(corner, Vector3Scale(rightDir, bracketLength * (rightScale > 0 ? -1.0f : 1.0f)));
  Vector3 upEnd = Vector3Add(corner, Vector3Scale(upDir, bracketLength * (upScale > 0 ? -1.0f : 1.0f)));

  rightEnd = ClampToGround(rightEnd, minHeight);
  upEnd = ClampToGround(upEnd, minHeight);

  DrawLBracket(corner, rightEnd, upEnd, color, lineThickness, camUp, camRight, groundLevel);
}

// Draws target lock brackets around a locked enemy
static void DrawTargetLock(EntityManager* em, Entity player, WeaponControlComponent* wc) {
  Entity target = wc->lockedTarget;
  if (target >= MAX_ENTITIES) return;
  if ((em->componentMasks[target] & (COMPONENT_TRANSFORM | COMPONENT_COLLISION)) != (COMPONENT_TRANSFORM | COMPONENT_COLLISION)) return;

  // Get target's world-space bounding box
  TransformComponent* tTarget = &em->transformComponents[target];
  CollisionComponent* cTarget = &em->collisionComponents[target];
  BoundingBox worldBox;
  worldBox.min = Vector3Add(cTarget->hitbox.min, tTarget->position);
  worldBox.max = Vector3Add(cTarget->hitbox.max, tTarget->position);
  Vector3 center = Vector3Scale(Vector3Add(worldBox.min, worldBox.max), 0.5f);

  PlayerControlComponent* playerComp = &em->playerControlComponents[player];
  Camera* camera = playerComp->camera;

  rlDisableDepthTest();

  // Draw target marker above the target
  Vector3 markerPos = (Vector3){ center.x, worldBox.max.y + TARGET_MARKER_HEIGHT_OFFSET, center.z };
  DrawCylinderWires(markerPos, 1.0f, 0.0f, 2.0f, 4, TARGET_LOCK_COLOR);

  // Calculate camera-relative coordinate system
  Vector3 camForward, camRight, camUp;
  CalculateCameraBasis(camera, &camForward, &camRight, &camUp);

  // Configuration constants
  const float padding = 1.5f;
  const float bracketLength = 1.2f;
  const float lineThickness = 0.25f;
  const float groundLevel = 0.0f;
  const float frontOffset = 1.5f;
  const float bottomBracketOffset = 10.0f;

  // Expand bounding box and calculate dimensions
  Vector3 expandedMin = Vector3Subtract(worldBox.min, (Vector3){padding, padding, padding});
  Vector3 expandedMax = Vector3Add(worldBox.max, (Vector3){padding, padding, padding});
  Vector3 boxSize = Vector3Subtract(expandedMax, expandedMin);

  float halfWidth = boxSize.x * 0.5f;
  float halfHeight = boxSize.y * 0.5f;
  Vector3 offsetCenter = Vector3Add(center, Vector3Scale(camForward, frontOffset));

  // Draw all 4 corner brackets using helper function
  // Top brackets use groundLevel, bottom brackets use bottomBracketOffset
  DrawCornerBracket(offsetCenter, camRight, camUp, -halfWidth, -halfHeight, bracketLength, TARGET_CROSSHAIR, lineThickness, camUp, camRight, groundLevel, groundLevel);
  DrawCornerBracket(offsetCenter, camRight, camUp, halfWidth, -halfHeight, bracketLength, TARGET_CROSSHAIR, lineThickness, camUp, camRight, groundLevel, groundLevel);
  DrawCornerBracket(offsetCenter, camRight, camUp, -halfWidth, halfHeight, bracketLength, TARGET_CROSSHAIR, lineThickness, camUp, camRight, groundLevel, groundLevel + bottomBracketOffset);
  DrawCornerBracket(offsetCenter, camRight, camUp, halfWidth, halfHeight, bracketLength, TARGET_CROSSHAIR, lineThickness, camUp, camRight, groundLevel, groundLevel + bottomBracketOffset);

  rlEnableDepthTest();
}

void Hud3DSystem(struct Systems* systems) {
  uint32_t mask = (COMPONENT_PLAYER_CONTROL | COMPONENT_WEAPON_CONTROL);
  EntityManager* em = &systems->entityManager;

  for (int i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {
      WeaponControlComponent* wc = &em->weaponControlComponents[i];
      DrawTargetLock(em, i, wc);
    }
  }
}


//  ===============
//  LEVEL MESSAGES
//  ===============


void DrawLevelMessage(struct Systems* systems) {
  StateManager* state = &systems->stateManager;

  int fontSize = 20;

  // Center text horizontally, position vertically using margin multiplier
  int screenX = GetScreenWidth() / 2.0f;
  int screenY = MARGIN_TOP;

  if (state->currentScreen == SCREEN_FIRST_LEVEL) {
    // Draw objective text centered, with title at 4x margin and description at 5.5x margin from top
    DrawText("OBJECTIVE:", screenX - MeasureText("OBJECTIVE:", fontSize) / 2.0, 4.0 * screenY, fontSize, WHITE);
    DrawText("Destroy the enemy structure", screenX - MeasureText("Destroy the enemy structure", fontSize) / 2.0, 5.5 * screenY, fontSize, WHITE);
  }
  if (state->currentScreen == SCREEN_SECOND_LEVEL) {
    // Draw objective text centered, with title at 4x margin and description at 5.5x margin from top
    DrawText("OBJECTIVE:", screenX - MeasureText("OBJECTIVE:", fontSize) / 2.0, 4.0 * screenY, fontSize, WHITE);
    DrawText("Destroy the enemy mecha", screenX - MeasureText("Destroy the enemy mecha", fontSize) / 2.0, 5.5 * screenY, fontSize, RED);
  }
}

//  ===================
//  WEAPONS GROUP BOX
//  ===================


// Auxiliary function to translate enum -> str
const char* GetWeaponTypeName(WeaponType type) {
    const char* names[] = {
        "NONE",              // WEAPON_NONE
        "Laser Pulse",       // WEAPON_LASER_PULSE
        "Missile Launcher",  // WEAPON_MISSILE_LAUNCHER
        "Auto Cannon",       // WEAPON_AUTO_CANNON
        "Weapon Type 4",
        "Weapon Type 5"
    };

    size_t nameCount = sizeof(names) / sizeof(names[0]);


    if (type >= 0 && type < nameCount) {
        return names[type];
    }
    
    return "UNKNOWN";
}


void DrawWeaponGroups(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  WeaponControlComponent* wp_control = NULL;

  //Look for the player
  uint32_t neededMask = COMPONENT_PLAYER_CONTROL | COMPONENT_WEAPON_CONTROL;
  for (Entity e = 0; e < em->numEntities; e++) {
    if ((em->componentMasks[e] & neededMask) == neededMask) {
      wp_control = &em->weaponControlComponents[e];
      break;
      }
    }
  
  if (wp_control == NULL) return;

  //Calcula o tamanho total da area dos groups
  float x_size = (float) BAR_WIDTH / 2.0;
  float x_pos = MARGIN_TOP;
  float y_pos = 2.5 * MARGIN_TOP;

  float currentY = y_pos;

  char label[25];
  const char* weaponNameStr = "";


  for (int i = 0; i < MAX_WEAPONS_GROUPS; i++) {
    Color fillColor = HUD_BLUE_FILL;
    Color borderColor = WHITE;
    Color backgroundcolor;

    float currentCooldown = 0.0f;
    float maxCooldown = 0.0f;

    //  Clean label at each weapon found
    label[0] = '\0';

    // Try to locate the active weapons
    Entity weaponEntity = MAX_ENTITIES;
    for (int j = 0; j < MAX_WEAPONS_EQUIP; j++) {
        if (wp_control->weaponsGroupMap[j] == i && wp_control->activeGroup[i]) {
            weaponEntity = wp_control->weaponsSlots[j];
            if (weaponEntity != MAX_ENTITIES && (em->componentMasks[weaponEntity] & COMPONENT_WEAPON)) {
                WeaponComponent* wc = &em->weaponComponents[weaponEntity];
                
                if (wp_control->activeGroup[i] && wc->type == WEAPON_MISSILE_LAUNCHER) {
                  DrawText("Press TAB to lock target for Missel Launcher", 
                  (GetScreenWidth() - MeasureText("Press TAB to lock target for Missel Launcher", 18)) / 2.0,
                  GetScreenHeight() - 5.3 * MARGIN_TOP, 18, WHITE);
                }
                
                // Transform weapon type from enum to string
                weaponNameStr = GetWeaponTypeName(wc->type);

                // Grant a null termination in label
                strncpy(label, weaponNameStr, sizeof(label) - 1);
                label[sizeof(label) - 1] = '\0';

                currentCooldown = wc->cooldownTimer;
                maxCooldown = wc->firingRate;
                break; 
            }
        }
    }

    //Se estiver ativo, o grupo fica verde
    if (wp_control->activeGroup[i]) {
      borderColor = HUD_GREEN_FILL;
    }

    //Cor bonitinhas para o cooldown
    if (currentCooldown > 0.0 * maxCooldown) {
      fillColor = HUD_RED_FILL;
      backgroundcolor = HUD_GRAY_FILL;
    }
    
    else backgroundcolor = HUD_BLUE_FILL;
        
    DrawBar(x_pos, currentY, x_size, BAR_HEIGHT, currentCooldown, maxCooldown, fillColor, borderColor, backgroundcolor, label, 18.0, "");
    //Dá espaço para o proximo group
    currentY += BAR_HEIGHT + MARGIN_TOP;
  }
}


