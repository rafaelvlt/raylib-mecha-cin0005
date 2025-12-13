#include <raylib.h>
#include <raymath.h>
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"
#include "ecs/types.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"
#include "utility.h"

// Missile Explosion
#define MISSILE_EXPLOSION_START_SIZE  2.0f  
#define MISSILE_EXPLOSION_END_SIZE    2.0f
#define MISSILE_EXPLOSION_DURATION    1.5f  
#define MISSILE_EXPLOSION_COLS        4    
#define MISSILE_EXPLOSION_ROWS        4

// Missile Explosion
#define DEATH_EXPLOSION_START_SIZE  15.0f  
#define DEATH_EXPLOSION_END_SIZE    20.0f
#define DEATH_EXPLOSION_DURATION    2.5f  
#define DEATH_EXPLOSION_COLS        4    
#define DEATH_EXPLOSION_ROWS        4

// Objective Explosion
#define    OBJECTIVE_EXPLOSION_START_SIZE 30.00f
#define    OBJECTIVE_EXPLOSION_END_SIZE 50.00f
#define   OBJECTIVE_EXPLOSION_DURATION 5.0

// Laser Explosion
#define LASER_HIT_START_SIZE  2.0f
#define LASER_HIT_END_SIZE    2.0f
#define LASER_HIT_DURATION    1.0f
#define LASER_HIT_COLUMNS     6
#define LASER_HIT_ROWS        6

// Missile Smoke Trail 
#define TRAIL_SPAWN_CHANCE    15    

// Helper Functions

static void SpawnLaserPulseExplosion(struct Systems* systems, Vector3 position) {
  EntityManager* em = &systems->entityManager;

  Entity effect = CreateEntity(em);
  if (effect == MAX_ENTITIES) return;
  AddTransformComponent(em, effect, position);

  AddEffectSheet(
    em, 
    effect,
    LASER_HIT_START_SIZE, 
    LASER_HIT_END_SIZE,  
    WHITE,              
    LASER_HIT_DURATION,
    TEXTURE_ID_LASER_EXPLOSION_SPRITESHEET, 
    LASER_HIT_COLUMNS, 
    LASER_HIT_ROWS,
    false
  );
}


static void SpawnMissileLauncherExplosion(struct Systems* systems, Vector3 position) {
  EntityManager* em = &systems->entityManager;

  Entity effect = CreateEntity(em);
  if (effect == MAX_ENTITIES) return;

  AddTransformComponent(em, effect, position);

  AddEffectSheet(
    em, 
    effect,
    MISSILE_EXPLOSION_START_SIZE,
    MISSILE_EXPLOSION_END_SIZE,
    WHITE,              
    MISSILE_EXPLOSION_DURATION,
    TEXTURE_ID_MISSILE_EXPLOSION_SPRITESHEET, 
    MISSILE_EXPLOSION_COLS,
    MISSILE_EXPLOSION_ROWS,
    false
  );
}

static void SpawnDeathExplosion(struct Systems* systems, Vector3 position){
  EntityManager* em = &systems->entityManager;

  Entity effect = CreateEntity(em);
  if (effect == MAX_ENTITIES) return;

  Vector3 explosionPos = position;
  explosionPos.y += 8.0f;

  AddTransformComponent(em, effect, explosionPos);

  AddEffectSheet(
    em, 
    effect,
    DEATH_EXPLOSION_START_SIZE,
    DEATH_EXPLOSION_END_SIZE,
    WHITE,              
    DEATH_EXPLOSION_DURATION,
    TEXTURE_ID_MISSILE_EXPLOSION_SPRITESHEET, 
    DEATH_EXPLOSION_COLS,
    DEATH_EXPLOSION_ROWS,
    false
  );
}

static void SpawnWinExplosion(struct Systems* systems, Vector3 position){
  EntityManager* em = &systems->entityManager;

  Entity effect = CreateEntity(em);
  if (effect == MAX_ENTITIES) return;

  Vector3 explosionPos = position;
  if (systems->stateManager.currentScreen == SCREEN_FIRST_LEVEL) explosionPos.y += 100.0f;
  else explosionPos.y += 20.0f;

  AddTransformComponent(em, effect, explosionPos);

  AddEffectSheet(
    em, 
    effect,
    OBJECTIVE_EXPLOSION_START_SIZE,
    OBJECTIVE_EXPLOSION_END_SIZE,
    WHITE,              
    OBJECTIVE_EXPLOSION_DURATION,
    TEXTURE_ID_MISSILE_EXPLOSION_SPRITESHEET, 
    MISSILE_EXPLOSION_COLS,
    MISSILE_EXPLOSION_ROWS,
    false
  );
}


void EffectSystemOnEvent(struct Systems* systems, Event event){

  if (event.type == EVENT_PROJECTILE_COLLISION) {
    if(event.data.projectileCollisionDetected.type == WEAPON_PULSE_LASER){
      SpawnLaserPulseExplosion(systems, event.data.projectileCollisionDetected.impactPoint);
    } 
    else if(event.data.projectileCollisionDetected.type == WEAPON_MISSILE_LAUNCHER){
      SpawnMissileLauncherExplosion(systems, event.data.projectileCollisionDetected.impactPoint);
    }
  }
  if (event.type == EVENT_ENTITY_DEATH){
    if (event.data.deathEvent.type == ENTITY_OBJECTIVE) SpawnWinExplosion(systems, event.data.deathEvent.pos);
    else SpawnDeathExplosion(systems, event.data.deathEvent.pos);
  }
}

void EffectSystem(struct Systems* systems, Camera* camera){
  EntityManager* em = &systems->entityManager;
  ResourceManager* rm = &systems->resourceManager;
  BeginBlendMode(BLEND_ALPHA);
  for (Entity i = 0; i < em->numEntities; i++) {
    uint32_t mask = COMPONENT_EFFECT | COMPONENT_TRANSFORM | COMPONENT_LIFETIME;

    if ((em->componentMasks[i] & mask) == mask) {

      EffectComponent* fx = &em->effectComponents[i];
      LifetimeComponent* life = &em->lifetimeComponents[i];
      TransformComponent* trans = &em->transformComponents[i];
      int currentFrame = 0;
      float timeAlive = life->lifetime - life->currentTime;
      if (fx->loop){
        float progress = timeAlive / life->lifetime; 
        int totalFrames = fx->totalFrames;
        currentFrame = (int)(progress * totalFrames) % totalFrames;
      }
      else{
        // Remaining time (0.0 to 1.0)
        float t = 1.0f - timeAlive; 
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        currentFrame = (int)(t * fx->totalFrames);
        if (currentFrame >= fx->totalFrames) currentFrame = fx->totalFrames - 1;
      }
      // Size scaling
      float currentSize = Lerp(fx->startSize, fx->endSize, timeAlive);
      // Switch case for each type of resource sprite(spritesheet, multiple file, static)
      switch (fx->type) {

        case FX_TYPE_SPRITESHEET: {
          Texture2D tex = *GetTexture(rm, fx->data.sheet.id);

          // Change draw rect for the next frame
          float cellW = (float)tex.width / fx->data.sheet.columns;
          float cellH = (float)tex.height / fx->data.sheet.rows;

          int col = currentFrame % fx->data.sheet.columns;
          int row = currentFrame / fx->data.sheet.columns;

          Rectangle sourceRec = { 
            (float)col * cellW,  // X position in texture
            (float)row * cellH,  // Y position in texture
            cellW,               // Width of ONE frame
            cellH                // Height of ONE frame
          };

          DrawBillboardRec(*camera, tex, sourceRec, trans->position, (Vector2){currentSize, currentSize}, fx->color);
          break;
        }

        case FX_TYPE_ARRAY: { 
          AssetTextureID currentTexID = fx->data.sheet.id + currentFrame;

          Texture2D* texPtr = GetTexture(rm, currentTexID);

          if (texPtr != NULL) {
            Rectangle source = {0.0f, 0.0f, (float)texPtr->width, (float)texPtr->height};

            DrawBillboardRec(*camera, *texPtr, source, trans->position, (Vector2){currentSize, currentSize}, fx->color);
          }
          EndBlendMode();
          break;
        }

        case FX_TYPE_STATIC: {
          Texture2D tex = *GetTexture(rm, fx->data.sheet.id);
          Rectangle sourceRec = {0, 0, tex.width, tex.height};
          DrawBillboardRec(*camera, tex, sourceRec, trans->position, (Vector2){currentSize, currentSize}, fx->color);
          break;
        }
      }
      EndBlendMode();
    }
  }
}

void TrailSystem(struct Systems* systems) {
  EntityManager* em = &systems->entityManager;
  uint32_t mask = (COMPONENT_PROJECTILE | COMPONENT_TRANSFORM);

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {
      ProjectileComponent* proj = &em->projectileComponents[i];

      if (proj->type == WEAPON_MISSILE_LAUNCHER) {
        TransformComponent* trans = &em->transformComponents[i];

        Vector3 forward = Vector3RotateByQuaternion((Vector3){0,0,-1}, trans->orientation);
        Vector3 tailPos = Vector3Subtract(trans->position, Vector3Scale(forward, 1.0f)); 
        // Chance of trail spawning
        if (GetRandomValue(0, 100) < TRAIL_SPAWN_CHANCE) {
          Entity smoke = CreateEntity(em);
          if (smoke < MAX_ENTITIES) {
            // Jitter, changes here they spawn
            Vector3 jitter = { 
              (float)GetRandomValue(-2,2)/20.0f, 
              (float)GetRandomValue(-2,2)/20.0f, 
              (float)GetRandomValue(-2,2)/20.0f 
            };
            AddTransformComponent(em, smoke, Vector3Add(tailPos, jitter));

            float life = 1.5f;
            AddEffectArray(em, smoke, 0.75f, 2.5f, Fade(WHITE, 0.6f), life, TEXTURE_ID_SMOKE, 10, false);
          }
        }
      }
    }
  }
}



