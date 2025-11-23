#include <raylib.h>
#include <raymath.h>
#include "ecs/entitymanager.h"
#include "ecs/systems.h"
#include "ecs/types.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"
#include "map_loader.h"

// -------------------------------------------
// TEMPORARY FUNCTION WHILE THE MAP ISN'T READY 
// -------------------------------------------
static void DrawLevel(struct Systems* systems);

// -------------------------------------------
// TEMPORARY FUNCTION ONLY FOR DEBUGGING 
// -------------------------------------------
static void DrawTargetDebug(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;

  for (int i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & (COMPONENT_PLAYER_CONTROL | COMPONENT_WEAPON_CONTROL)) == 
      (COMPONENT_PLAYER_CONTROL | COMPONENT_WEAPON_CONTROL)) {

      WeaponControlComponent* wc = &em->weaponControlComponents[i];
      Entity target = wc->lockedTarget;
      if (target < MAX_ENTITIES && (em->componentMasks[target] & (COMPONENT_TRANSFORM | COMPONENT_COLLISION))) {

        TransformComponent* tTarget = &em->transformComponents[target];
        CollisionComponent* cTarget = &em->collisionComponents[target];

        BoundingBox worldBox;
        worldBox.min = Vector3Add(cTarget->hitbox.min, tTarget->position);
        worldBox.max = Vector3Add(cTarget->hitbox.max, tTarget->position);

        DrawBoundingBox(worldBox, GREEN);

        Vector3 center = Vector3Scale(Vector3Add(worldBox.min, worldBox.max), 0.5f);
        DrawLine3D(center, Vector3Add(center, (Vector3){0, 20, 0}), GREEN);

      }
    }
  }
}

void InitFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
    // 1. Reset ECS
    InitEntityManager(&systems->entityManager);

    // 2. Configura Câmera (Necessário passar para o Loader)
    data->camera.position = (Vector3){ 0.0f, 2.5f, 0.0f };
    data->camera.target = (Vector3){ 0.0f, 2.5f, 1.0f };
    data->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    data->camera.fovy = 60.0f;
    data->camera.projection = CAMERA_PERSPECTIVE;

    // 3. Prepara Contexto e Carrega o Mapa
    MapContext context;
    context.mainCamera = &data->camera;
    
    // Carrega tudo (Player, Armas, Inimigos, Base) do arquivo de texto
    LoadMapFromText(&systems->entityManager, &systems->resourceManager, "resources/maps/level1.map", context);

    // 4. Input
    DisableCursor(); 
}

void UpdateFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  systems->delta_time = GetFrameTime(); 

  // Run Gameplay Systems
  PlayerControlSystem(systems);
  AIControlSystem(systems);
  LifetimeSystem(systems); 
  MovementSystem(systems);  
  AttachmentSystem(systems);
  WeaponSystem(systems); 
  CollisionSystem(systems);
  PlayerAudioSystem(systems);

  if (IsKeyPressed(systems->configManager.KeyMap.KeyPause)) 
  {
    EnableCursor();
    RequestScreenChange(systems, SCREEN_MAIN_MENU);
  }

  ProcessGameEvents(systems);
}

void DrawFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  ClearBackground(SKYBLUE);
  BeginMode3D(data->camera);
  DrawLevel(systems);           
  RenderSystem(systems);  
  DrawTargetDebug(systems);
  EffectSystem(systems, &data->camera);
  EndMode3D();


  DrawFPS(10, 10);
}

void DestroyFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{

}

static void DrawLevel(struct Systems* systems) // <--- Recebendo systems
{
    // 1. Desenha o Terreno Infinito (Visual)
    Model* terrain = GetModel(&systems->resourceManager, MODEL_ID_TERRAIN);
    
    if (terrain) {
        DrawModel(*terrain, (Vector3){0, -0.1f, 0}, 1.0f, WHITE);
    }
    
    // 2. Ambiente (Sol)
    DrawSphere((Vector3){ 300.0f, 300.0f, 0.0f }, 100.0f, (Color){ 255, 200, 50, 255 });
}