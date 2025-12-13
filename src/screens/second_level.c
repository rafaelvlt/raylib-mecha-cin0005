#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h> 
#include <math.h>
#include "../include/ecs/entitymanager.h"
#include "../include/ecs/systems.h"
#include "../include/ecs/types.h"
#include "../include/event_manager.h"
#include "../include/resource_manager.h"
#include "../include/state_manager.h"
#include "../include/systems.h"
#include "../include/utility.h"
#include "../include/map_loader.h"

#define BRIEFING_DURATION     22.0f  
#define MECHA_WALK_SPEED      6.0f   
#define START_Z_POS           (MECHA_WALK_SPEED * BRIEFING_DURATION) 

#define BOOT_DURATION         7.0f   

#define MECH_HEIGHT           20.0f
#define BOB_FREQUENCY         1.5f
#define BOB_AMPLITUDE         0.2f

#define BOOT_TIME_CROSSHAIR   1.0f
#define BOOT_TIME_RADAR       2.5f 
#define BOOT_TIME_GROUPS      3.5f
#define BOOT_TIME_HUD_BARS    4.5f
#define BOOT_TIME_HUD_3D      5.5f
#define BOOT_TIME_MSG         6.5f  

typedef enum {
    STATE_BRIEFING, 
    STATE_BOOT,     
    STATE_GAMEPLAY, 
    STATE_VICTORY   
} LevelState;

static LevelState currentLevelState = STATE_BRIEFING;

static void DrawLevel(struct Systems* systems, const Camera* camera);
static void ProcessBriefing(struct Systems* systems, SecondLevelData* data);
static void ProcessBootLogic(struct Systems* systems, SecondLevelData* data);
static void ProcessGameplaySystems(struct Systems* systems);
static void CheckLevelCompletion(struct Systems* systems, SecondLevelData* data);
static void ProcessLevelFinish(struct Systems* systems, SecondLevelData* data);
static void SyncCameraToPlayer(struct Systems* systems, SecondLevelData* data);

void InitSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
  InitEntityManager(&systems->entityManager);
  
  currentLevelState = STATE_BRIEFING;
  data->dropTimer = 0.0f;
  data->canControl = false;
  data->levelFinished = false;
  data->finishTimer = 0.0f;

  data->camera.position = (Vector3){ 0.0f, 25.0f, START_Z_POS + 40.0f }; 
  data->camera.target = (Vector3){ 0.0f, 10.0f, START_Z_POS - 50.0f }; 
  data->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  data->camera.fovy = 60.0f;
  data->camera.projection = CAMERA_PERSPECTIVE;

  MapContext context;
  context.mainCamera = &data->camera;
  LoadMapFromText(&systems->entityManager, &systems->resourceManager, "resources/maps/level2.map", context);
  DisableCursor(); 

  EntityManager* em = &systems->entityManager;
  for(int i=0; i < em->numEntities; i++) {
      if (em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) {
          if (em->componentMasks[i] & COMPONENT_TRANSFORM) {
              em->transformComponents[i].position = (Vector3){ 0.0f, 0.0f, START_Z_POS };
              em->transformComponents[i].orientation = QuaternionIdentity();
              
              PlayerControlComponent* p = &em->playerControlComponents[i];
              p->headTimer = 0.0f;
              p->lastHeadTimer = 0.0f;
              p->walkLerp = 0.0f;
          }
          break;
      }
  }

  if (systems->audioManager.playingNow != NULL) {
      StopMusicStream(*systems->audioManager.playingNow);
      systems->audioManager.playingNow = NULL;
  }

  Sound* briefingAudio = GetSound(&systems->resourceManager, SOUND_ID_BOSS_BRIEFING); 
  if (briefingAudio) {
      SetSoundVolume(*briefingAudio, GetSoundVolume(&systems->configManager)); 
      PlaySound(*briefingAudio);
  }
  
  ProcessGameplaySystems(systems);
}

static void ProcessBriefing(struct Systems* systems, SecondLevelData* data) {
    float dt = systems->delta_time;
    data->dropTimer += dt;

    EntityManager* em = &systems->entityManager;
    Entity player = MAX_ENTITIES;

    for(int i=0; i < em->numEntities; i++) {
        if (em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) {
            player = i;
            break;
        }
    }

    if (player != MAX_ENTITIES) {
        PlayerControlComponent* p = &em->playerControlComponents[player];
        Vector3 forward = (Vector3){ 0.0f, 0.0f, -1.0f };
        
        if (em->componentMasks[player] & COMPONENT_PHYSICS) {
            em->physicsComponents[player].velocity = Vector3Scale(forward, MECHA_WALK_SPEED);
        }

        p->lastHeadTimer = p->headTimer;
        p->headTimer += dt * BOB_FREQUENCY;
        p->walkLerp = Lerp(p->walkLerp, 1.0f, 5.0f * dt);

        if ((int)p->headTimer > (int)p->lastHeadTimer) { 
            Sound* step = GetSound(&systems->resourceManager, SOUND_ID_MECHA_FOOTSTEP);
            if (step) {
                float baseVol = GetSoundVolume(&systems->configManager);
                SetSoundVolume(*step, baseVol * 0.40f); 
                SetSoundPitch(*step, 0.85f); 
                PlaySound(*step);
            }
        }
        
        SyncCameraToPlayer(systems, data);
    }

    if (data->dropTimer > BRIEFING_DURATION) {
        if (player != MAX_ENTITIES) {
            em->physicsComponents[player].velocity = Vector3Zero();
            em->transformComponents[player].position = (Vector3){0,0,0}; 
            
            PlayerControlComponent* p = &em->playerControlComponents[player];
            p->walkLerp = 0.0f;
            p->headTimer = 0.0f;
            
            Vector3 pPos = em->transformComponents[player].position;
            data->camera.position = (Vector3){ pPos.x, pPos.y + 20.0f, pPos.z };
            data->camera.target = (Vector3){ pPos.x, pPos.y + 20.0f, pPos.z - 100.0f };
        }

        currentLevelState = STATE_BOOT;
        data->dropTimer = 0.0f; 
        
        Sound* bootSfx = GetSound(&systems->resourceManager, SOUND_ID_STARTUP_SEQUENCE);
        if (bootSfx) {
            SetSoundVolume(*bootSfx, GetSoundVolume(&systems->configManager));
            PlaySound(*bootSfx);
        }
    }
}

static void ProcessBootLogic(struct Systems* systems, SecondLevelData* data) {
  float dt = (systems->delta_time > 0.05f) ? 0.05f : systems->delta_time;
  data->dropTimer += dt;

  if (data->dropTimer > BOOT_DURATION) {
    currentLevelState = STATE_GAMEPLAY;
    data->canControl = true;

    Music* bgm = GetMusic(&systems->resourceManager, MUSIC_ID_SECOND_LEVEL);
    if (bgm) {
        StopMusicStream(*bgm); 
        PlayMusicStream(*bgm);
        SetMusicVolume(*bgm, GetMusicVolume(&systems->configManager));
        systems->audioManager.playingNow = bgm; 
    }
  }
}

static void SyncCameraToPlayer(struct Systems* systems, SecondLevelData* data) {
    EntityManager* em = &systems->entityManager;
    for(int i=0; i < em->numEntities; i++) {
        if (em->componentMasks[i] & COMPONENT_PLAYER_CONTROL) {
            TransformComponent* trans = &em->transformComponents[i];
            PlayerControlComponent* p = &em->playerControlComponents[i];
            
            float bobY = sinf(p->headTimer * PI * 2.0f) * BOB_AMPLITUDE * p->walkLerp;
            
            data->camera.position = Vector3Add(trans->position, (Vector3){0, MECH_HEIGHT + bobY, 0});

            Vector3 direction = (Vector3){ 0.0f, 0.0f, -1.0f };
            data->camera.target = Vector3Add(data->camera.position, direction);
            
            float bobRoll = cosf(p->headTimer * PI) * 0.02f * p->walkLerp;
            data->camera.up = Vector3RotateByAxisAngle((Vector3){0,1,0}, direction, bobRoll);
            
            break;
        }
    }
}

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
  
  if (currentLevelState == STATE_GAMEPLAY) {
      PlayerAudioSystem(systems);
  }
}

static void CheckLevelCompletion(struct Systems* systems, SecondLevelData* data) {
  if (data->levelFinished) return;

  EventManager* em = &systems->eventManager;
  for (int i = 0; i < em->eventCounter; i++) {
    Event event = em->eventQueue[i];
    if (event.type == EVENT_ENTITY_DEATH && event.data.deathEvent.type == ENTITY_OBJECTIVE) {
      data->levelFinished = true;
      data->finishTimer = 5.0f;
      currentLevelState = STATE_VICTORY;
    }
  }
}

static void ProcessLevelFinish(struct Systems* systems, SecondLevelData* data) {
  if (!data->levelFinished) return;

  data->finishTimer -= systems->delta_time;

  if (data->finishTimer <= 2.5f && data->finishTimer > 2.4f) {
    Sound* endSfx = GetSound(&systems->resourceManager, SOUND_ID_MISSION_SUCCESS);
    if (endSfx != NULL) { 
        SetSoundVolume(*endSfx, GetSoundVolume(&systems->configManager));
        PlaySound(*endSfx);
    }
  }

  if (data->finishTimer <= 0.0f) {
    RequestScreenChange(systems, SCREEN_END_GAME); 
  }
}

void UpdateSecondLevelScreen(struct Systems* systems, SecondLevelData* data) {
  if (data->levelFinished) systems->delta_time *= 0.5;

  switch (currentLevelState) {
      case STATE_BRIEFING:
          ProcessBriefing(systems, data);
          ProcessGameplaySystems(systems); 
          break;

      case STATE_BOOT:
          ProcessBootLogic(systems, data);
          ProcessGameplaySystems(systems); 
          break;

      case STATE_GAMEPLAY:
          ProcessGameplaySystems(systems);
          if (data->canControl) {
            PlayerControlSystem(systems); 
          }
          break;
          
      case STATE_VICTORY:
          ProcessGameplaySystems(systems);
          break;
  }

  if (IsKeyPressed(KEY_P) && currentLevelState == STATE_BRIEFING) {
      data->dropTimer = BRIEFING_DURATION + 0.1f;
  }

  if (IsKeyPressed(KEY_ENTER) && data->canControl) {
      EnableCursor();
      RequestScreenChange(systems, SCREEN_MAIN_MENU);
  }

  CheckLevelCompletion(systems, data);
  ProcessLevelFinish(systems, data);
  ProcessGameEvents(systems);
}

void DrawSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
  float timer = data->dropTimer;

  ClearBackground((Color){ 15, 20, 30, 255 }); 
  
  BeginMode3D(data->camera);
  DrawLevel(systems, &data->camera);           
  RenderSystem(systems);  
  EffectSystem(systems, &data->camera);
  
  if (currentLevelState == STATE_GAMEPLAY || 
     (currentLevelState == STATE_BOOT && timer > BOOT_TIME_HUD_3D)) {
      Hud3DSystem(systems);
  }
  
  EndMode3D();

  if (currentLevelState == STATE_BRIEFING) {
      // Fade Out Rápido (2 segundos)
      // Se timer < 2.0, alpha calcula de 1.0 até 0.0
      // Se timer > 2.0, alpha é 0.0 (não desenha)
      if (timer < 2.0f) {
          float alpha = 1.0f - (timer / 2.0f);
          DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, alpha));
      }
  }
  else if (currentLevelState == STATE_BOOT) {
      if (timer > BOOT_TIME_MSG) DrawLevelMessage(systems);
      if (timer > BOOT_TIME_RADAR) DrawMinimapSystem(systems);
      if (timer > BOOT_TIME_GROUPS) DrawWeaponGroups(systems);
      if (timer > BOOT_TIME_HUD_BARS) {
          DrawHPBar(systems);
          DrawHeatBar(systems);
      }
      if (timer > BOOT_TIME_CROSSHAIR) DrawCrosshair(systems);
  }
  else if (currentLevelState == STATE_GAMEPLAY || currentLevelState == STATE_VICTORY) {
      DrawLevelMessage(systems);
      DrawMinimapSystem(systems);
      DrawWeaponGroups(systems);
      DrawHPBar(systems);
      DrawHeatBar(systems);
      DrawCrosshair(systems);
  }

  DrawFPS(10, 10);
}

void DestroySecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
}

static void DrawLevel(struct Systems* systems, const Camera* camera) {
  Model* terrain = GetModel(&systems->resourceManager, MODEL_ID_TERRAIN);
  if (terrain && terrain->meshCount > 0) {
      DrawModel(*terrain, (Vector3){0, 0.0f, 0}, 1.0f, (Color){60, 60, 80, 255});
  }
  DrawSphere((Vector3){ 0.0f, 200.0f, -400.0f }, 80.0f, (Color){ 200, 200, 255, 255 });
}
