#include <raylib.h>
#include <raymath.h>
#include "ecs/entitymanager.h"
#include "ecs/systems.h"
#include "ecs/types.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"

// -------------------------------------------
// TEMPORARY FUNCTION WHILE THE MAP ISN'T READY 
// -------------------------------------------
static void DrawLevel(void);
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
  // 1. Reset ECS state
  InitEntityManager(&systems->entityManager);

  // ---------------------------------------------------------
  // Player Entity Setup
  // ---------------------------------------------------------
  Entity player = CreateEntity(&systems->entityManager);

  AddTransformComponent(&systems->entityManager, player, (Vector3){ 0.0f, 0.0f, 0.0f });
  AddPhysicsComponent(&systems->entityManager, player, (Vector3){ 0.0f, 0.0f, 0.0f }, 0.90f);

  // Connects camera to player control. Sensitivity: 0.003, MaxSpeed: 15.0, TurnSpeed: 2.0
  AddPlayerControlComponent(&systems->entityManager, player, &data->camera);

  BoundingBox playerBounds = (BoundingBox){(Vector3){-1,-1,-1}, (Vector3){1,2,1}};
  AddCollisionComponent(&systems->entityManager, player, playerBounds, false, false);

  AddHealthComponent(&systems->entityManager, player, 100.0f); // Vida máxima 100.0
  // Max Heat: 100.0, Heat/Shot: 2.0, Cooldown Rate: 10.0
  AddCockpitHUDComponent(&systems->entityManager, player, 100.0f, 2.0f, 10.0f); 

  // ---------------------------------------------------------
  // Weapon Setup 
  // ---------------------------------------------------------

  // --- Left Laser Weapon ---
  Entity laserLeft = CreateEntity(&systems->entityManager);
  Vector3 offsetL = { -1.70f, 5.50f, 1.50f };

  AddTransformComponent(&systems->entityManager, laserLeft, Vector3Zero());
  AddAttachmentComponent(&systems->entityManager, laserLeft, player, offsetL, QuaternionIdentity());
  AddWeaponComponent(
      &systems->entityManager, laserLeft, 
      WEAPON_PULSE_LASER, 
      0.75f,   // fireRate 
      150.0f, // Speed
      5.0f,   // Damage
      500.0f, // Range
      2.0f,   // Heat
      2,      // burstTotal
      0.1f    // burstRate 
  );

  // --- Right Laser Weapon ---
  Entity laserRight = CreateEntity(&systems->entityManager);
  Vector3 offsetR = { 1.70f, 5.50f, 1.50f }; // Inverted X

  AddTransformComponent(&systems->entityManager, laserRight, Vector3Zero());
  AddAttachmentComponent(&systems->entityManager, laserRight, player, offsetR, QuaternionIdentity());
  AddWeaponComponent(
      &systems->entityManager, laserRight, 
      WEAPON_PULSE_LASER, 
      0.75f,   // fireRate 
      150.0f, // Speed
      5.0f,   // Damage
      500.0f, // Range
      2.0f,   // Heat
      2,      // burstTotal
      0.1f    // burstRate 
  );


  // --- Left Missile Weapon ---
  
  Entity missileLeft = CreateEntity(&systems->entityManager);
  Vector3 offsetLM = { -1.70f, 7.00f, 1.50f };

  AddTransformComponent(&systems->entityManager, missileLeft, Vector3Zero());
  AddAttachmentComponent(&systems->entityManager, missileLeft, player, offsetLM, QuaternionIdentity());
  AddWeaponComponent(
    &systems->entityManager, missileLeft, 
    WEAPON_MISSILE_LAUNCHER, 
    5.0f,   // fireRate 
    60.0f,  // Speed 
    5.0f,  // Damage
    1000.0f,// Range
    5.0f,   // Heat
    20,     // burstTotal 
    0.115f   // burstRate 
  );
  // --- Right Missile Weapon ---
  //
  Entity missileRight = CreateEntity(&systems->entityManager);
  Vector3 offsetRM = { 1.70f, 7.00f, 1.50f };
  AddTransformComponent(&systems->entityManager, missileRight, Vector3Zero());
  AddAttachmentComponent(&systems->entityManager, missileRight, player, offsetRM, QuaternionIdentity());
  AddWeaponComponent(
    &systems->entityManager, missileRight, 
    WEAPON_MISSILE_LAUNCHER, 
    5.0f,   // fireRate 
    60.0f,  // Speed 
    5.0f,  // Damage
    1000.0f,// Range
    5.0f,   // Heat
    20,     // burstTotal 
    0.115f   // burstRate 
  );
  // ---------------------------------------------------------
  // Weapon Control System (Loadout)
  // ---------------------------------------------------------
  AddWeaponControlComponent(&systems->entityManager, player, AIM_MODE_CAMERA);

  WeaponControlComponent* ctrl = &systems->entityManager.weaponControlComponents[player];

  // Link entities to slots and groups
  ctrl->weaponsSlots[0] = laserLeft;
  ctrl->weaponsGroupMap[0] = 0; // Group 1

  ctrl->weaponsSlots[1] = laserRight;
  ctrl->weaponsGroupMap[1] = 0; // Group 1

  ctrl->weaponsSlots[2] = missileLeft;
  ctrl->weaponsGroupMap[2] = 1; // Group 2
  
  ctrl->weaponsSlots[3] = missileRight;
  ctrl->weaponsGroupMap[3] = 1; // Group 2
  
  ctrl->activeGroup[0] = true;

  // ---------------------------------------------------------
  // Enemy Entity Setup
  // ---------------------------------------------------------

  Model* enemyModel = GetModel(&systems->resourceManager, MODEL_ID_ENEMY_SCOUT);

  if (enemyModel != NULL) {
    // Apply scale fix to the shared model
    enemyModel->transform = MatrixScale(0.5f, 0.5f, 0.5f);

    Entity enemy = CreateEntity(&systems->entityManager);

    AddTransformComponent(&systems->entityManager, enemy, (Vector3){ 0.0f, 3.0f, -50.0f });
    AddPhysicsComponent(&systems->entityManager, enemy, (Vector3){0,0,0}, 0.90f);

    BoundingBox enemyBox = {
      (Vector3){ -3.0f, 0.0f, -6.0f }, // Largura 4m, 
      (Vector3){  3.0f, 4.0f,  12.5f }  // Altura 9m
    };
    AddCollisionComponent(&systems->entityManager, enemy, enemyBox, false, false);

    AddHealthComponent(&systems->entityManager, enemy, 100.0f);
    AddAIControlComponent(&systems->entityManager, enemy, 50.0f, 10.0f);

    AddRenderComponent(&systems->entityManager, enemy, enemyModel, WHITE);
  }

  if (enemyModel != NULL) {
    // Apply scale fix to the shared model
    enemyModel->transform = MatrixScale(0.5f, 0.5f, 0.5f);

    Entity enemy = CreateEntity(&systems->entityManager);

    AddTransformComponent(&systems->entityManager, enemy, (Vector3){ 10.0f, 3.0f, -50.0f });
    AddPhysicsComponent(&systems->entityManager, enemy, (Vector3){0,0,0}, 0.90f);

    BoundingBox enemyBox = {
      (Vector3){ -3.0f, 0.0f, -6.0f }, // Largura 4m, 
      (Vector3){  3.0f, 4.0f,  12.5f }  // Altura 9m
    };
    AddCollisionComponent(&systems->entityManager, enemy, enemyBox, false, false);

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
  AIControlSystem(systems);
  LifetimeSystem(systems); 
  MissileSystem(systems);
  TrailSystem(systems);
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
  ClearBackground(RAYWHITE);

  BeginMode3D(data->camera);
  DrawLevel();         
  RenderSystem(systems);  
  DrawTargetDebug(systems);
  EffectSystem(systems, &data->camera);
  EndMode3D();

  //Desenha o HUD e minimapa
  DrawHUDSystem(systems);
  DrawMinimapSystem(systems, data);




  Hudsystem(systems);
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
