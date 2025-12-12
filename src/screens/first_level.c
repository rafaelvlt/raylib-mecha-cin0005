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
#include "utility.h"
#include "map_loader.h"

#define TIME_DROP_DURATION    8.0f 
#define TIME_BOOT_DURATION    7.0f  
#define TIME_GAMEPLAY_START   (TIME_DROP_DURATION + TIME_BOOT_DURATION)

#define BOOT_TIME_CROSSHAIR   1.5f
#define BOOT_TIME_HUD         3.0f  
#define BOOT_TIME_RADAR       4.5f 
#define BOOT_TIME_MSG         6.0f  
#define BOOT_TIME_GROUPS      2.0f


// Forward declarations
static void DrawLevel(struct Systems* systems, const Camera* camera);
static void ProcessStartupSequence(struct Systems* systems, FirstLevelData* data);
static void ProcessGameplaySystems(struct Systems* systems);
static void CheckLevelCompletion(struct Systems* systems, FirstLevelData* data);
static void ProcessLevelFinish(struct Systems* systems, FirstLevelData* data);

void InitFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  // Reset ECS
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

  // Entry Sequence
  data->dropTimer = 0.0f;
  data->hasLanded = false;
  data->canControl = false;
  Sound* skySound = GetSound(&systems->resourceManager, SOUND_ID_SKYDROP);
  if (skySound) {
    float volume = GetAudioVolume(&systems->configManager);
    StopSound(*skySound); 
    SetSoundVolume(*skySound, volume); 
    PlaySound(*skySound);
  }
}


// Processes startup sequence (drop and boot phases)
static void ProcessStartupSequence(struct Systems* systems, FirstLevelData* data) {
  if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
    data->dropTimer = TIME_GAMEPLAY_START + 0.1f;
    data->hasLanded = true;
    data->canControl = true;
    data->camera.position.x = 0.0f;

    Music* bgm = GetMusic(&systems->resourceManager, MUSIC_ID_FIRST_LEVEL);
    if (bgm) {
      StopSound(*GetSound(&systems->resourceManager, SOUND_ID_SKYDROP));
      StopMusicStream(*bgm);
      PlayMusicStream(*bgm);
      SetMusicVolume(*bgm, GetAudioVolume(&systems->configManager));
    }
    return;
  }

  data->dropTimer += systems->delta_time;

  if (data->dropTimer < TIME_DROP_DURATION) {
    StopMusicStream(*systems->audioManager.playingNow);
    float intensity = data->dropTimer / TIME_DROP_DURATION;
    data->camera.position.x = (GetRandomValue(-1,1)/100.0f) * intensity;
  }
  else if (!data->hasLanded) {
    data->hasLanded = true;
    PlaySound(systems->resourceManager.sounds[SOUND_ID_STARTUP_SEQUENCE]);
    data->camera.position.x = 0.0f;
    PlayerControlSystem(systems);
  }
  else if (data->dropTimer > TIME_GAMEPLAY_START) {
    data->canControl = true;
    Music* bgm = GetMusic(&systems->resourceManager, MUSIC_ID_FIRST_LEVEL);
    if (bgm) {
      StopMusicStream(*bgm);
      PlayMusicStream(*bgm);
      SetMusicVolume(*bgm, GetAudioVolume(&systems->configManager));
    }
  }
}

// Runs all gameplay systems
static void ProcessGameplaySystems(struct Systems* systems) {
  AIControlSystem(systems);
  AnimationSystem(systems);
  LifetimeSystem(systems);
  MissileSystem(systems);
  TrailSystem(systems);
  MovementSystem(systems);
  AttachmentSystem(systems);
  WeaponSystem(systems);
  CollisionSystem(systems);
  HealthSystem(systems);
  PlayerAudioSystem(systems);
  HeatSystemUpdate(systems);
}

// Checks if level completion conditions are met
static void CheckLevelCompletion(struct Systems* systems, FirstLevelData* data) {
  if (data->levelFinished) return;

  EventManager* em = &systems->eventManager;
  for (int i = 0; i < em->eventCounter; i++) {
    Event event = em->eventQueue[i];
    if (event.type == EVENT_ENTITY_DEATH && event.data.deathEvent.type == ENTITY_OBJECTIVE) {
      data->levelFinished = true;
      data->finishTimer = 5.0f;
    }
  }
}

// Processes level finish sequence
static void ProcessLevelFinish(struct Systems* systems, FirstLevelData* data) {
  if (!data->levelFinished) return;

  data->finishTimer -= systems->delta_time;

  if (data->finishTimer <= 2.5f && data->finishTimer > 2.4f) {
    Sound* endSfx = GetSound(&systems->resourceManager, SOUND_ID_MISSION_SUCCESS);
    SetSoundVolume(*endSfx, GetAudioVolume(&systems->configManager));
    PlaySound(*endSfx);
  }

  if (data->finishTimer <= 0.0f) {
    RequestScreenChange(systems, SCREEN_SECOND_LEVEL);
  }
}

void UpdateFirstLevelScreen(struct Systems* systems, FirstLevelData* data) {
  if (data->levelFinished) systems->delta_time *= 0.5;

  if (!data->canControl) {
    ProcessStartupSequence(systems, data);
  }

  if (data->canControl) {
    PlayerControlSystem(systems);
    if (IsKeyPressed(KEY_ENTER)) {
      EnableCursor();
      RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
  }

  if (data->hasLanded) {
    ProcessGameplaySystems(systems);
  }

  CheckLevelCompletion(systems, data);
  ProcessLevelFinish(systems, data);
  ProcessGameEvents(systems);
}

void DrawFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  if (data->dropTimer < TIME_DROP_DURATION) {
    ClearBackground(BLACK);
    return;
  }
  float bootTime = data->dropTimer - TIME_DROP_DURATION;

  ClearBackground(SKYBLUE);
  BeginMode3D(data->camera);
  DrawLevel(systems, &data->camera);           
  RenderSystem(systems);  
  EffectSystem(systems, &data->camera);
  if (bootTime > BOOT_TIME_HUD) Hud3DSystem(systems);
  EndMode3D();

  if (!data->canControl) {
    float fadeDuration = 7.0f; 
    // Show skip option during startup
    if (bootTime < fadeDuration) {
      Vector2 textPos = {20, GetScreenHeight() - 40};
      DrawText("PRESS SPACE OR ENTER TO SKIP", textPos.x, textPos.y, 20, WHITE);
    }
    // Show skip option during startup
    if (bootTime < fadeDuration) {
      // Alpha vai de 1.0 (Preto) a 0.0 (Transparente)
      float alpha = 1.0f - (bootTime / fadeDuration);
      DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, alpha));
    }
  }

  // Desenha o HUD e minimapa
  if (bootTime > BOOT_TIME_CROSSHAIR) DrawCrosshair(systems);
  if (bootTime > BOOT_TIME_HUD) DrawHPBar(systems);
  if (bootTime > BOOT_TIME_RADAR) DrawMinimapSystem(systems);
  if (bootTime > BOOT_TIME_MSG) DrawLevelMessage(systems);
  if (bootTime > BOOT_TIME_GROUPS) DrawWeaponGroups(systems);
  if (bootTime > BOOT_TIME_GROUPS) DrawHeatBar(systems);

  

  DrawFPS(10, 10);
}

void DestroyFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{

}

static void DrawLevel(struct Systems* systems, const Camera* camera) {
  Model* terrain = GetModel(&systems->resourceManager, MODEL_ID_TERRAIN);
  if (terrain) DrawModel(*terrain, (Vector3){0, 0.0f, 0}, 1.0f, WHITE);

  DrawSphere((Vector3){ 300.0f, 300.0f, 0.0f }, 100.0f, SUN_COLOR);
}
