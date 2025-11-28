#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "ecs/entitymanager.h"
#include "ecs/systems.h"
#include "ecs/types.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "state_manager.h"
#include "systems.h"
#include "map_loader.h"

static void DrawLevel(struct Systems* systems, const Camera* camera);

void InitFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
    // 1. Reset ECS
    InitEntityManager(&systems->entityManager);

    // Camera Configuration
    data->camera.position = (Vector3){ 0.0f, 2.5f, 0.0f };
    data->camera.target = (Vector3){ 0.0f, 2.5f, 1.0f };
    data->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    data->camera.fovy = 60.0f;
    data->camera.projection = CAMERA_PERSPECTIVE;

    // Map Loading
    MapContext context;
    context.mainCamera = &data->camera;
    
    // Loads everything from level1.map file
    LoadMapFromText(&systems->entityManager, &systems->resourceManager, "resources/maps/level1.map", context);
    DisableCursor(); 

  data->levelFinished = false;
  data->finishTimer = 0.0f;
}


void UpdateFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{

  systems->delta_time = GetFrameTime(); 

  // Run Gameplay Systems
  PlayerControlSystem(systems);
  AIControlSystem(systems);
  LifetimeSystem(systems); 
  MissileSystem(systems);
  TrailSystem(systems);
  MovementSystem(systems);  
  AttachmentSystem(systems);
  WeaponSystem(systems); 
  CollisionSystem(systems);
  HealthSystem(systems);
  PlayerAudioSystem(systems);

  if (IsKeyPressed(KEY_ENTER)) 
  {
    EnableCursor();
    RequestScreenChange(systems, SCREEN_MAIN_MENU);
  }

  if (!data->levelFinished) {
    EventManager* em = &systems->eventManager;
    for (int i = 0; i < em->eventCounter; i++) {
      Event event = em->eventQueue[i];
      if (event.type == EVENT_ENTITY_DEATH){
        if (event.data.deathEvent.type == ENTITY_TURRET_STRUCTURE){
          data->levelFinished = true;
          data->finishTimer = 5.0f; 
        }
      }
    }
  }
  if (data->levelFinished) {
    data->finishTimer -= systems->delta_time;
    
    if (data->finishTimer <= 2.5f){
      Sound* endSfx = GetSound(&systems->resourceManager, SOUND_ID_MISSION_SUCCESS);
      SetSoundVolume(*endSfx, systems->configManager.audioVolume);
      PlaySound(*endSfx);
    }
    if (data->finishTimer <= 0.0f) {
      RequestScreenChange(systems, SCREEN_SECOND_LEVEL); 
    }
  }

  ProcessGameEvents(systems);
}

void DrawFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  ClearBackground(SKYBLUE);
  BeginMode3D(data->camera);
  DrawLevel(systems, &data->camera);           
  RenderSystem(systems);  
  EffectSystem(systems, &data->camera);
  Hud3DSystem(systems);
  EndMode3D();

  //Desenha o HUD e minimapa
  DrawHUDSystem(systems);
  DrawCrosshair(systems);
  DrawMinimapSystem(systems, data);
  DrawLevelMessage(systems);

  DrawFPS(10, 10);
}

void DestroyFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{

}

static void DrawLevel(struct Systems* systems, const Camera* camera) {
    Model* terrain = GetModel(&systems->resourceManager, MODEL_ID_TERRAIN);
    if (terrain) DrawModel(*terrain, (Vector3){0, -0.1f, 0}, 1.0f, WHITE);

    DrawSphere((Vector3){ 300.0f, 300.0f, 0.0f }, 100.0f, (Color){ 255, 200, 50, 255 });

}
