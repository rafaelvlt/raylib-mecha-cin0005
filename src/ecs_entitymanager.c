#include "ecs_entitymanager.h"
#include <raymath.h>
#include <string.h>


// Take the address of the entity manager and fill all of it with zero's to takeout junk out of the database
void InitEntityManager(EntityManager* entityManager) {
  memset(entityManager, 0, sizeof(EntityManager));
}

// Gets a new Entity ID from the manager with blank components if there's room available. else gets a invalid ID
Entity CreateEntity(EntityManager* entityManager)   {
  if (entityManager->numEntities < MAX_ENTITIES)  {
    Entity newEntity = entityManager->numEntities;
    entityManager->componentMasks[newEntity] = COMPONENT_NONE;
    entityManager->numEntities++;
    return newEntity;
  }
  else return MAX_ENTITIES;
}

// Remove an Entity by zeroing out the bitmask to get removed
void DestroyEntity(EntityManager* entityManager, Entity entity) {
  if (entity < MAX_ENTITIES) {
    entityManager->componentMasks[entity] = COMPONENT_NONE;
  }
}

/* ===========================
  Add Components Functions
  Parameters - EntityManager, Entity, Component Data...
============================== */

void AddTransformComponent(EntityManager* entityManager, Entity entity, Vector3 position) {
    entityManager->transformComponents[entity].position = position;
    entityManager->transformComponents[entity].orientation = QuaternionIdentity();
    entityManager->componentMasks[entity] |= COMPONENT_TRANSFORM;
}

void AddPhysicsComponent(EntityManager* entityManager, Entity entity, Vector3 velocity) {
    entityManager->physicsComponents[entity].velocity = velocity;
    entityManager->physicsComponents[entity].acceleration = (Vector3){0, 0, 0};
    entityManager->componentMasks[entity] |= COMPONENT_PHYSICS;
}

void AddRenderComponent(EntityManager* entityManager, Entity entity, Model model, Color tint) {
    entityManager->renderComponents[entity].model = model;
    entityManager->renderComponents[entity].tint = tint;
    entityManager->renderComponents[entity].isVisible = true;
    entityManager->componentMasks[entity] |= COMPONENT_RENDER;
}

void AddAttachmentComponent(EntityManager* entityManager, Entity entity, Entity parent, Vector3 offsetPos, Quaternion offsetRot) {
    entityManager->attachmentComponents[entity].parent = parent;
    entityManager->attachmentComponents[entity].offsetPosition = offsetPos;
    entityManager->attachmentComponents[entity].offsetRotation = offsetRot;
    entityManager->componentMasks[entity] |= COMPONENT_ATTACHMENT;
}

void AddPlayerControlComponent(EntityManager* entityManager, Entity entity, Camera *camera, float sensitivity) {
    entityManager->playerControlComponents[entity].camera = camera;
    entityManager->playerControlComponents[entity].cameraYaw = 0.0f;
    entityManager->playerControlComponents[entity].cameraPitch = 0.0f;
    entityManager->playerControlComponents[entity].mouseSensitivity = sensitivity;
    entityManager->componentMasks[entity] |= COMPONENT_PLAYER_CONTROL;
}

void AddHealthComponent(EntityManager* entityManager, Entity entity, float health) {
    entityManager->healthComponents[entity].currentHealth = health;
    entityManager->healthComponents[entity].maxHealth = health;
    entityManager->componentMasks[entity] |= COMPONENT_HEALTH;
}

void AddWeaponComponent(EntityManager* entityManager, Entity entity, float fireRate, float projSpeed, float projDamage) {
    entityManager->weaponComponents[entity].fireRate = fireRate;
    entityManager->weaponComponents[entity].cooldownTimer = 0.0f;
    entityManager->weaponComponents[entity].projectileSpeed = projSpeed;
    entityManager->weaponComponents[entity].projectileDamage = projDamage;
    entityManager->weaponComponents[entity].isFiring = false;
    entityManager->componentMasks[entity] |= COMPONENT_WEAPON;
}

void AddWeaponControlComponent(EntityManager* entityManager, Entity entity, Entity primary, Entity secondary) {
    entityManager->weaponControlComponents[entity].primaryWeapon = primary;
    entityManager->weaponControlComponents[entity].secondaryWeapon = secondary;
    entityManager->componentMasks[entity] |= COMPONENT_WEAPON_CONTROL;
}

void AddAIControlComponent(EntityManager* entityManager, Entity entity, float sight, float range) {
    entityManager->aiControlComponents[entity].target = MAX_ENTITIES;
    entityManager->aiControlComponents[entity].sightRadius = sight;
    entityManager->aiControlComponents[entity].attackRange = range;
    entityManager->aiControlComponents[entity].timeSinceLastAction = 0.0f;
    entityManager->aiControlComponents[entity].state = 0;
    entityManager->componentMasks[entity] |= COMPONENT_AI_CONTROL;
}

void AddCockpitHUDComponent(EntityManager* entityManager, Entity entity, float maxHeat, float heatPerShot, float cooldown) {
    entityManager->cockpitHUDComponents[entity].maxHeat = maxHeat;
    entityManager->cockpitHUDComponents[entity].currentHeat = 0.0f;
    entityManager->cockpitHUDComponents[entity].heatPerShot = heatPerShot;
    entityManager->cockpitHUDComponents[entity].cooldownRate = cooldown;
    entityManager->componentMasks[entity] |= COMPONENT_COCKPIT_HUD;
}
