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

// --- TIMELINE CONFIGURATION ---
// Total time for the system boot sequence
#define TIME_BOOT_DURATION    7.0f  

// HUD Appearance Timing (Sequential Boot)
#define BOOT_TIME_CROSSHAIR   1.0f
#define BOOT_TIME_RADAR       2.5f 
#define BOOT_TIME_GROUPS      3.5f
#define BOOT_TIME_HUD_BARS    4.5f
#define BOOT_TIME_HUD_3D      5.5f
#define BOOT_TIME_MSG         6.5f  

// Forward declarations
static void DrawLevel(struct Systems* systems, const Camera* camera);
static void ProcessBootLogic(struct Systems* systems, SecondLevelData* data);
static void ProcessGameplaySystems(struct Systems* systems);
static void CheckLevelCompletion(struct Systems* systems, SecondLevelData* data);
static void ProcessLevelFinish(struct Systems* systems, SecondLevelData* data);

void InitSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
  // Reset ECS
  InitEntityManager(&systems->entityManager);

  // Camera Configuration (Facing the Boss at -Z)
  data->camera.position = (Vector3){ 0.0f, 20.0f, 0.0f };
  data->camera.target = (Vector3){ 0.0f, 20.0f, -100.0f }; 
  data->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  data->camera.fovy = 60.0f;
  data->camera.projection = CAMERA_PERSPECTIVE;

  // Map Loading
  MapContext context;
  context.mainCamera = &data->camera;

  LoadMapFromText(&systems->entityManager, &systems->resourceManager, "resources/maps/level2.map", context);
  DisableCursor(); 

  data->levelFinished = false;
  data->finishTimer = 0.0f;

  data->dropTimer = 0.0f; 
  
  data->hasLanded = true;   
  data->canControl = false; 

  // Start Music
  Music* bgm = GetMusic(&systems->resourceManager, MUSIC_ID_SECOND_LEVEL);
  if (bgm) {
    if (systems->audioManager.playingNow != NULL) {
        StopMusicStream(*systems->audioManager.playingNow);
    }
    PlayMusicStream(*bgm);
    SetMusicVolume(*bgm, GetAudioVolume(&systems->configManager));
    systems->audioManager.playingNow = bgm; 
  }
  
  // Start Boot Sound Effect
  PlaySound(systems->resourceManager.sounds[SOUND_ID_STARTUP_SEQUENCE]);

  // Force first update to ensure AI/Physics orientation is correct on frame 0
  ProcessGameplaySystems(systems);
}

// Handles timer increment and unlocking controls
static void ProcessBootLogic(struct Systems* systems, SecondLevelData* data) {
  float dt = systems->delta_time;

  data->dropTimer += dt;

  // Unlock controls when boot sequence finishes
  if (data->dropTimer > TIME_BOOT_DURATION) {
    data->canControl = true;
  }
}

// Runs all autonomous systems (AI, Physics, Animations, Projectiles)
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
}

static void CheckLevelCompletion(struct Systems* systems, SecondLevelData* data) {
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

static void ProcessLevelFinish(struct Systems* systems, SecondLevelData* data) {
  if (!data->levelFinished) return;

  data->finishTimer -= systems->delta_time;

  // Play Success Sound
  if (data->finishTimer <= 2.5f && data->finishTimer > 2.4f) {
    Sound* endSfx = GetSound(&systems->resourceManager, SOUND_ID_MISSION_SUCCESS);
    SetSoundVolume(*endSfx, GetAudioVolume(&systems->configManager));
    PlaySound(*endSfx);
  }

  // Transition to End Screen
  if (data->finishTimer <= 0.0f) {
    RequestScreenChange(systems, SCREEN_MAIN_MENU);
  }
}

void UpdateSecondLevelScreen(struct Systems* systems, SecondLevelData* data) {
  if (data->levelFinished) systems->delta_time *= 0.5;

  ProcessBootLogic(systems, data);


  ProcessGameplaySystems(systems);

  if (data->canControl) {
    PlayerControlSystem(systems);
    if (IsKeyPressed(KEY_ENTER)) {
      EnableCursor();
      RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
  }

  CheckLevelCompletion(systems, data);
  ProcessLevelFinish(systems, data);
  ProcessGameEvents(systems);
}

void DrawSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
  // Timer is used directly for visual sequencing
  float bootTime = data->dropTimer;

  // Night Atmosphere (Dark Blue/Grey)
  ClearBackground((Color){ 15, 20, 30, 255 }); 
  
  BeginMode3D(data->camera);
  DrawLevel(systems, &data->camera);           
  RenderSystem(systems);  
  EffectSystem(systems, &data->camera);
  
  if (bootTime > BOOT_TIME_HUD_3D) Hud3DSystem(systems);
  EndMode3D();

  if (bootTime > BOOT_TIME_MSG) DrawLevelMessage(systems);
  if (bootTime > BOOT_TIME_RADAR) DrawMinimapSystem(systems);
  if (bootTime > BOOT_TIME_GROUPS) DrawWeaponGroups(systems);
  if (bootTime > BOOT_TIME_HUD_BARS) {
      DrawHPBar(systems);
      DrawHeatBar(systems);
  }
  if (bootTime > BOOT_TIME_CROSSHAIR) DrawCrosshair(systems);

  DrawFPS(10, 10);
}

void DestroySecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
}

static void DrawLevel(struct Systems* systems, const Camera* camera) {
  Model* terrain = GetModel(&systems->resourceManager, MODEL_ID_TERRAIN);
  if (terrain) DrawModel(*terrain, (Vector3){0, 0.0f, 0}, 1.0f, (Color){60, 60, 80, 255});

  // Distant Moon
  DrawSphere((Vector3){ 0.0f, 200.0f, -400.0f }, 80.0f, (Color){ 200, 200, 255, 255 });
}
