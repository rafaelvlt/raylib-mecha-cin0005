#include <raylib.h>
#include <raymath.h>
#include "ecs/entitymanager.h"
#include "ecs/systems.h"
#include "ecs/types.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"



void Hudsystem(struct Systems* systems){
  ConfigManager* cfg = &systems->configManager;
  Texture crosshair = *GetTexture(&systems->resourceManager, TEXTURE_ID_CROSSHAIR_SPRITE);

  int screenW = cfg->screenResolution.x;
  int screenH = cfg->screenResolution.y;

  DrawTexture(crosshair, screenW/2 - crosshair.width/2, screenH/2 - crosshair.height/2, WHITE);
}