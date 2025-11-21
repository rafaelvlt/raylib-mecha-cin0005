#include <raylib.h>
#include "ecs/ecs_components.h"
#include "ecs/ecs_entitymanager.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "systems.h"
#include "ecs/ecs_systems.h"
#include "raymath.h" 

// Helper functions
void SpawnProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats);

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

      // Zero out the acceleration vector
      physics->acceleration = Vector3Zero();
    }
  }
}

void RenderSystem(struct Systems* systems) {
  const uint32_t mask = COMPONENT_TRANSFORM | COMPONENT_RENDER;
  EntityManager* em = &(systems->entityManager);
  for (Entity i = 0; i < em->numEntities; i++) {
    if ((em->componentMasks[i] & mask) == mask) {

      RenderComponent* render = &em->renderComponents[i];

      // If the attachment is destroyed, doesn't draw it
      if (!render->isVisible) continue;

      TransformComponent* transform = &em->transformComponents[i];

      Vector3 axis;
      float angle;

      // Converts the quartenion to angle and axis, make it possible to work with DrawModelEx
      QuaternionToAxisAngle(transform->orientation, &axis, &angle);

      // Angle needs to be converted from Rad to Degree, the Vector3 is the scale component
      DrawModelEx(*(render->model), transform->position, axis, angle * RAD2DEG, (Vector3){ 1.0f, 1.0f, 1.0f }, render->tint);
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


void WeaponSystem(struct Systems* systems){
  const uint32_t mask = COMPONENT_WEAPON_CONTROL;
  EntityManager* em = &systems->entityManager;

  float dt = systems->delta_time;
  for (Entity mecha = 0; mecha < em->numEntities; mecha++){
    if((em->componentMasks[mecha] & mask) == mask){

      WeaponControlComponent* wc = &em->weaponControlComponents[mecha];

      for (int idx = 0; idx < MAX_WEAPONS_EQUIP; idx++){
        Entity WeaponID = wc->weaponsSlots[idx];

        // Empty Slot or not a Weapon
        uint32_t weapon_mask = (COMPONENT_WEAPON | COMPONENT_ATTACHMENT);
        if (WeaponID == MAX_ENTITIES || (em->componentMasks[WeaponID] & (weapon_mask)) != weapon_mask);
        else{
          WeaponComponent* weapon = &em->weaponComponents[WeaponID];
          TransformComponent* weaponTrans = &em->transformComponents[WeaponID];

          if (weapon->cooldownTimer > 0) weapon->cooldownTimer -= dt;

          int group = wc->weaponsGroupMap[idx];

          if (wc->triggerPulled && wc->activeGroup[group] && weapon->cooldownTimer <= 0){
            weapon->cooldownTimer = weapon->firingRate;

            SpawnProjectile(systems, weaponTrans->position, wc->aimDirection, mecha, weapon);

            // Event logic
            EventData data;
            data.weaponFired.owner = mecha;
            data.weaponFired.position = weaponTrans->position; 
            data.weaponFired.direction = wc->aimDirection;
            data.weaponFired.weapon = weapon->type;
            PushEvent(systems, EVENT_WEAPON_FIRED, data);
          }

        }
      }
    }
  }
}

void SpawnProjectile(struct Systems* systems, Vector3 position, Vector3 direction, Entity owner, WeaponComponent* stats) {
  EntityManager* em = &systems->entityManager;
  ResourceManager* rm = &systems->resourceManager;

  Entity bullet = CreateEntity(em);
  if (bullet >= MAX_ENTITIES) return;
  
  AddTransformComponent(em, bullet, position);

  Vector3 velocity = Vector3Scale(direction, stats->projectileSpeed);

  if (em->componentMasks[owner] & COMPONENT_PHYSICS) {
    Vector3 ownerVel = em->physicsComponents[owner].velocity;
    velocity = Vector3Add(velocity, ownerVel);
  }
  AddPhysicsComponent(em, bullet, velocity, 0.0f); // Drag 0

  BoundingBox box = (BoundingBox){(Vector3){-0.2f, -0.2f, -0.2f}, (Vector3){0.2f, 0.2f, 0.2f}};
  AddCollisionComponent(em, bullet, box, false, true);

  AddProjectileComponent(em, bullet, owner, stats->projectileDamage, true, 0.0f, (Effect)0);

  Model* bulletModel = GetModel(rm, stats->projectileModelID);

  if (bulletModel == NULL) {
    bulletModel = GetModel(rm, MODEL_ID_PROJECTILE);
  }

  if (bulletModel != NULL) {
    AddRenderComponent(em, bullet, bulletModel, YELLOW);
  }

  // AddLifetimeComponent(em, bullet, 3.0f); 


}

