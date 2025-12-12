#include <raylib.h>
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"
#include "ecs/systems.h"
#include "raymath.h" 

// Query all components with Transform and Physics Components, make them move based on their data
void MovementSystem(struct Systems* systems) {
  const uint32_t mask = COMPONENT_TRANSFORM | COMPONENT_PHYSICS;
  EntityManager* em = &(systems->entityManager);
  for (Entity i = 0; i < em->numEntities; i++) {
    // Bitmask check
    if ((em->componentMasks[i] & mask) == mask) {
      float deltaTime = systems->delta_time;

      // Gets the a pointer to the components from the Manager 
      TransformComponent* transform = &(em->transformComponents[i]);
      PhysicsComponent* physics = &(em->physicsComponents[i]);
      // Vf = Vi + a*dt
      physics->velocity = Vector3Add(physics->velocity, Vector3Scale(physics->acceleration, deltaTime));

      // Pf = Pi + v*dt
      transform->position = Vector3Add(transform->position, Vector3Scale(physics->velocity, deltaTime));

      // Apply a little friction
      // TBD -> Make friction definite and a macro, just a dummy value
      physics->velocity = Vector3Scale(physics->velocity, 0.98f);
      physics->acceleration = Vector3Zero();
    }
  }
}

// This system advances the animation timeline based on playback speed
void AnimationSystem(struct Systems* systems) {
  const uint32_t mask = COMPONENT_ANIMATION;
  EntityManager* em = &(systems->entityManager);
  float dt = systems->delta_time;

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {
      AnimationComponent* animComp = &em->animationComponents[i];

      // playbackSpeed > 1.0 = faster, < 1.0 = slower, 1.0 = normal speed
      animComp->currentTime += dt * animComp->playbackSpeed;
    }
  }
}

// Applies the current animation frame to a model based on animation component state
static void ApplyEntityAnimation(EntityManager* em, ResourceManager* rm, Entity entity, RenderComponent* render, AnimationComponent* animComp) {
  // Validating
  if (!render->model) return;
  
  AssetModelID modelId = animComp->modelId;
  if (modelId < 0 || modelId >= MODEL_ID_COUNT) return;
  
  // Get animation array and count for this model
  ModelAnimation* animations = rm->modelAnimations[modelId];
  int animCount = rm->modelAnimCounts[modelId];

  // Validate animation data exists and current animation index is valid
  if (!animations || animCount <= 0 || animComp->currentAnim < 0 || animComp->currentAnim >= animCount) return;

  // Get the specific animation clip to play
  ModelAnimation anim = animations[animComp->currentAnim];

  // Only apply if animation has frames and is valid for this model
  if (anim.frameCount > 0 && IsModelAnimationValid(*render->model, anim)) {
    float totalFrames = (float)anim.frameCount;

    // fmodf handles float mod and positive wrapping, manual fix for negative values
    float wrapped = fmodf(animComp->currentTime, totalFrames);
    if (wrapped < 0.0f) wrapped += totalFrames;

    // Convert time to frame index (truncate to integer)
    int frame = (int)wrapped;

    // Clamp frame to valid range
    if (frame < 0) frame = 0;
    if (frame >= anim.frameCount) frame = anim.frameCount - 1;

    // Apply this frame to the model (updates bone transforms)
    UpdateModelAnimation(*render->model, anim, frame);
  }
}

// Renders all visible entities with transform and render components
void RenderSystem(struct Systems* systems) {
  const uint32_t mask = COMPONENT_TRANSFORM | COMPONENT_RENDER;
  EntityManager* em = &(systems->entityManager);
  ResourceManager* rm = &systems->resourceManager;

  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {
      RenderComponent* render = &em->renderComponents[i];

      // Only render if entity is marked as visible
      if (render->isVisible) {
        TransformComponent* transform = &em->transformComponents[i];

        // Apply animation if entity has animation component
        if ((em->componentMasks[i] & COMPONENT_ANIMATION) == COMPONENT_ANIMATION) {
          AnimationComponent* animComp = &em->animationComponents[i];
          ApplyEntityAnimation(em, rm, i, render, animComp);
        }

        // Convert quaternion to axis-angle (required by RL)
        Vector3 axis;
        float angle;
        QuaternionToAxisAngle(transform->orientation, &axis, &angle);

        DrawModelEx(*(render->model), transform->position, axis, angle * RAD2DEG, (Vector3){ 1.0f, 1.0f, 1.0f }, render->tint);
      }
    }
  } 
}

void AttachmentSystem(struct Systems* systems){
  const uint32_t mask = COMPONENT_ATTACHMENT | COMPONENT_TRANSFORM;
  EntityManager* em = &systems->entityManager;

  for (Entity i = 0; i < em->numEntities; i++){
    if ((em->componentMasks[i] & mask) == mask){
      AttachmentComponent* child = &em->attachmentComponents[i];
      TransformComponent* childTrans = &em->transformComponents[i];      

      if (em->componentMasks[child->parent] == COMPONENT_NONE){
        DestroyEntity(em, i);
        return;
      }

      TransformComponent* parentTrans = &em->transformComponents[child->parent];      

      // Defines the transform component based on the parent      
      childTrans->orientation = QuaternionMultiply(parentTrans->orientation, child->offsetRotation);

      Vector3 rotatedOffset = Vector3RotateByQuaternion(child->offsetPosition, parentTrans->orientation);

      childTrans->position = Vector3Add(parentTrans->position, rotatedOffset); 
    }
  }
}




void LifetimeSystem(struct Systems* systems){
  uint32_t mask = COMPONENT_LIFETIME;
  EntityManager* em = &systems->entityManager;

  float dt = systems->delta_time;
  for (Entity entity = 0; entity < em->numEntities; entity++){
    if ((em->componentMasks[entity] & mask) == mask){
      LifetimeComponent* lt = &em->lifetimeComponents[entity];

      lt->currentTime -= dt;

      if (lt->currentTime <= 0){
        DestroyEntity(em, entity);
      }
    }
  }
}
