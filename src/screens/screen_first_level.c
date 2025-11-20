#include <raylib.h>
#include <raymath.h>
#include "ecs/ecs_systems.h"
#include "systems.h"

static void DrawLevel(void);

void InitFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  // Reset ECS state
  InitEntityManager(&systems->entityManager);

  // ---------------------------------------------------------
  // Player Entity Setup
  // ---------------------------------------------------------
  Entity player = CreateEntity(&systems->entityManager);

  AddTransformComponent(&systems->entityManager, player, (Vector3){ 0.0f, 0.0f, 0.0f });
  AddPhysicsComponent(&systems->entityManager, player, (Vector3){ 0.0f, 0.0f, 0.0f }, 0.90f);

  // Connects camera to player control. Sensitivity: 0.003, MaxSpeed: 15.0, TurnSpeed: 2.0
  AddPlayerControlComponent(&systems->entityManager, player, &data->camera, 0.003f, 15.0f, 2.0f);

  BoundingBox playerBounds = (BoundingBox){(Vector3){-1,-1,-1}, (Vector3){1,2,1}};
  AddCollisionComponent(&systems->entityManager, player, playerBounds, false);


  // ---------------------------------------------------------
  // Enemy Entity Setup
  // ---------------------------------------------------------
  Model* enemyModel = GetModel(&systems->resourceManager, MODEL_ID_ENEMY_SCOUT);

  if (enemyModel != NULL) {
    // Apply scale fix to the shared model
    enemyModel->transform = MatrixScale(0.5f, 0.5f, 0.5f);

    Entity enemy = CreateEntity(&systems->entityManager);

    AddTransformComponent(&systems->entityManager, enemy, (Vector3){ 0.0f, 0.0f, 15.0f });
    AddPhysicsComponent(&systems->entityManager, enemy, (Vector3){0,0,0}, 0.90f);

    BoundingBox enemyBox = { (Vector3){ -1.0f, 0.0f, -1.0f }, (Vector3){ 1.0f, 3.0f, 1.0f } };
    AddCollisionComponent(&systems->entityManager, enemy, enemyBox, false);

    // AI and Stats
    AddHealthComponent(&systems->entityManager, enemy, 100.0f);
    AddAIControlComponent(&systems->entityManager, enemy, 50.0f, 10.0f);

    AddRenderComponent(&systems->entityManager, enemy, enemyModel, WHITE);
  }

  // ---------------------------------------------------------
  // Camera & Input Setup
  // ---------------------------------------------------------
  data->camera.position = (Vector3){ 0.0f, 2.5f, 0.0f };
  data->camera.target = (Vector3){ 0.0f, 2.5f, 1.0f };
  data->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  data->camera.fovy = 60.0f;
  data->camera.projection = CAMERA_PERSPECTIVE;

  DisableCursor(); 
}

void UpdateFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  systems->delta_time = GetFrameTime(); 

  // Run Gameplay Systems
  PlayerControlSystem(systems);
  PlayerAudioSystem(systems);
  AIControlSystem(systems);
  MovementSystem(systems);


  if (IsKeyPressed(systems->configManager.KeyMap.KeyPause)) 
  {
    EnableCursor();
    RequestScreenChange(systems, SCREEN_MAIN_MENU);
  }
}

void DrawFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
  ClearBackground(RAYWHITE);

  BeginMode3D(data->camera);
  DrawLevel();            
  RenderSystem(systems);  
  EndMode3D();

  DrawFPS(10, 10);
}

void DestroyFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{

}

static void DrawLevel(void)
{
  const int floorExtent = 25;
  const float tileSize = 5.0f;
  const Color tileColor1 = (Color){ 150, 200, 200, 255 };

  // Floor tiles
  for (int y = -floorExtent; y < floorExtent; y++)
  {
    for (int x = -floorExtent; x < floorExtent; x++)
    {
      if ((y & 1) && (x & 1))
      {
        DrawPlane((Vector3){ x*tileSize, 0.0f, y*tileSize}, (Vector2){ tileSize, tileSize }, tileColor1);
      }
      else if (!(y & 1) && !(x & 1))
      {
        DrawPlane((Vector3){ x*tileSize, 0.0f, y*tileSize}, (Vector2){ tileSize, tileSize }, LIGHTGRAY);
      }
    }
  }

  const Vector3 towerSize = (Vector3){ 16.0f, 32.0f, 16.0f };
  const Color towerColor = (Color){ 150, 200, 200, 255 };

  Vector3 towerPos = (Vector3){ 16.0f, 16.0f, 16.0f };
  DrawCubeV(towerPos, towerSize, towerColor);
  DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

  towerPos.x *= -1;
  DrawCubeV(towerPos, towerSize, towerColor);
  DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

  towerPos.z *= -1;
  DrawCubeV(towerPos, towerSize, towerColor);
  DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

  towerPos.x *= -1;
  DrawCubeV(towerPos, towerSize, towerColor);
  DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

  // Red sun
  DrawSphere((Vector3){ 300.0f, 300.0f, 0.0f }, 100.0f, (Color){ 255, 0, 0, 255 });
} 
