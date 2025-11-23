#include <raylib.h>
#include <raymath.h>
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"
#include "ecs/types.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"

// Explosion numbers
#define EXPLOSION_START_SIZE 1.5f
#define EXPLOSION_END_SIZE 1.5f
#define EXPLOSION_DURATION 0.5f
#define EXPLOSION_SPRITESHEET_COLS 6
#define EXPLOSION_SPRITESHEET_ROWS 6

// Helper Functions
static void SpawnExplosion(struct Systems* systems, Vector3 position) {
  EntityManager* em = &systems->entityManager;

  Entity effect = CreateEntity(em);
  if (effect == MAX_ENTITIES) return;

  AddTransformComponent(em, effect, position);
  AddLifetimeComponent(em, effect, EXPLOSION_DURATION);
  AddEffectComponent(em, effect,
                     EXPLOSION_START_SIZE, 
                     EXPLOSION_END_SIZE, 
                     WHITE, TEXTURE_ID_EXPLOSION_SPRITESHEET, 
                     EXPLOSION_SPRITESHEET_COLS, 
                     EXPLOSION_SPRITESHEET_ROWS);
}

void EffectSystemOnEvent(struct Systems* systems, Event event){

    if (event.type == EVENT_PROJECTILE_COLLISION) {
        SpawnExplosion(systems, event.data.projectileCollisionDetected.impactPoint);
    }
}

void EffectSystem(struct Systems* systems, Camera* camera){
  EntityManager* em = &systems->entityManager;
  ResourceManager* rm = &systems->resourceManager;

  for (Entity i = 0; i < em->numEntities; i++) {
    uint32_t mask = COMPONENT_EFFECT | COMPONENT_TRANSFORM | COMPONENT_LIFETIME;

    if ((em->componentMasks[i] & mask) == mask) {

      EffectComponent* fx = &em->effectComponents[i];
      LifetimeComponent* life = &em->lifetimeComponents[i];
      TransformComponent* trans = &em->transformComponents[i];

      // Remaining time (0.0 to 1.0)
      float t = 1.0f - (life->currentTime / life->lifetime);
      if (t < 0.0f) t = 0.0f;
      if (t > 1.0f) t = 1.0f;

      // Size scaling
      float currentSize = Lerp(fx->startSize, fx->endSize, t);

      // Spritesheet Logic
      if (fx->textureID != TEXTURE_ID_COUNT) { 
        
        int currentFrame = (int)(t * fx->totalFrames);
        if (currentFrame >= fx->totalFrames) currentFrame = fx->totalFrames - 1;

        Texture2D tex = *GetTexture(rm, fx->textureID);

        // Change draw rect for the next frame
        float cellW = (float)tex.width / fx->columns;
        float cellH = (float)tex.height / fx->rows;

        int col = currentFrame % fx->columns;
        int row = currentFrame / fx->columns;

        Rectangle sourceRec = { 
            (float)col * cellW,  // X position in texture
            (float)row * cellH,  // Y position in texture
            cellW,               // Width of ONE frame
            cellH                // Height of ONE frame
        };
        
        DrawBillboardRec(*camera, tex, sourceRec, trans->position, (Vector2){currentSize, currentSize}, fx->color);
      } 
    }
  }
}
