#include <raylib.h>
#include <raymath.h>
#include "ecs/entitymanager.h"
#include "ecs/components.h"
#include "ecs/types.h"
#include "resource_manager.h"
#include <string.h>


// Take the address of the entity manager and fill all of it with zero's to takeout junk out of the database
void InitEntityManager(EntityManager* entityManager) {
  memset(entityManager, 0, sizeof(EntityManager));
}

// Gets a new Entity ID from the manager with blank components if there's room available. else gets a invalid ID
Entity CreateEntity(EntityManager* em) {

  for (int i = 0; i < MAX_ENTITIES; i++) {

    // Find the first available spot by checking for a none bitmask
    if (em->componentMasks[i] == COMPONENT_NONE) {
      // increases numEntities if the new ID is higher than the largest being used
      if (i >= em->numEntities) {
        em->numEntities = i + 1;
      }

      return (Entity)i;
    }
  }

  TraceLog(LOG_WARNING,"ECS: Max Entity Limit reached. New Entity was not created"); 
  return MAX_ENTITIES; 
}

// Remove an Entity by zeroing out the bitmask to get removed,
void DestroyEntity(EntityManager* em, Entity entity) {
  if (entity < MAX_ENTITIES) {
    em->componentMasks[entity] = COMPONENT_NONE;
    // Reduces numEntities if the one being removed is the highest ID
    if (entity == em->numEntities - 1) em->numEntities--;

  }
}

// Same as init, just to Document Better
void ClearEntityManager(EntityManager* entityManager) {
  memset(entityManager, 0, sizeof(EntityManager));
}

/* ===========================
  Add Components Functions
  Parameters - EntityManager, Entity, Component Data...
============================== */

void AddTransformComponent(EntityManager* entityManager, Entity entity, Vector3 position) {
  TransformComponent* transform = &entityManager->transformComponents[entity];

  transform->position = position;
  transform->orientation = QuaternionIdentity();

  entityManager->componentMasks[entity] |= COMPONENT_TRANSFORM;
}

void AddPhysicsComponent(EntityManager* entityManager, Entity entity, Vector3 velocity, float drag) {
  PhysicsComponent* physics = &entityManager->physicsComponents[entity];

  physics->velocity = velocity;
  physics->acceleration = (Vector3){0, 0, 0};
  physics->drag = drag; 

  entityManager->componentMasks[entity] |= COMPONENT_PHYSICS;
}

void AddCollisionComponent(EntityManager* entityManager, Entity entity, BoundingBox hitbox, bool isStatic, bool isTrigger) {
  CollisionComponent* collision = &entityManager->collisionComponents[entity];

  collision->hitbox = hitbox;
  collision->isStatic = isStatic;
  collision->isTrigger = isTrigger; 
  entityManager->componentMasks[entity] |= COMPONENT_COLLISION;
}

void AddRenderComponent(EntityManager* entityManager, Entity entity, Model* model, Color tint) {
  RenderComponent* render = &entityManager->renderComponents[entity];

  render->model = model;
  render->tint = tint;
  render->isVisible = true;

  entityManager->componentMasks[entity] |= COMPONENT_RENDER;
}

void AddAttachmentComponent(EntityManager* entityManager, Entity entity, Entity parent, Vector3 offsetPos, Quaternion offsetRot) {
  AttachmentComponent* attachment = &entityManager->attachmentComponents[entity];

  attachment->parent = parent;
  attachment->offsetPosition = offsetPos;
  attachment->offsetRotation = offsetRot;

  entityManager->componentMasks[entity] |= COMPONENT_ATTACHMENT;
}

void AddPlayerControlComponent(EntityManager* entityManager, Entity entity, Camera *camera) {
  PlayerControlComponent* player = &entityManager->playerControlComponents[entity];
  // Camera configuration 
  player->camera = camera;


  player->mouseSensitivity = 0.001f;
  player->maxSpeed = 15.0f;
  player->turnSpeed = 1.5f;

  // Zero-init the rest
  player->legAngle = 0.0f;
  player->throttle = 0.0f;
  player->turnState = 0.0f;

  player->torsoYaw = 0.0f;
  player->torsoPitch = 0.0f;

  player->headTimer = 0.0f;
  player->walkLerp = 0.0f;
  player->headLerp = 0.0f;
  player->lean = (Vector2){0};
  
  player->centeringLegstoTorso = false;
  player->centeringTorsotoLegs = false;
  player->isMoving = false;
  player->isRotating = false;
  player->isZooming = false;
  player->lockTargetRequested = false;

  entityManager->componentMasks[entity] |= COMPONENT_PLAYER_CONTROL;
}

void AddHealthComponent(EntityManager* entityManager, Entity entity, float health) {
  HealthComponent* hp = &entityManager->healthComponents[entity];

  hp->currentHealth = health;
  hp->maxHealth = health;

  entityManager->componentMasks[entity] |= COMPONENT_HEALTH;
}

void AddWeaponComponent
(EntityManager* entityManager, Entity entity, WeaponType type,
 float firingRate, float projectileSpeed,
 float projectileDamage,  float range,  float heatGenerated,
 AssetSoundID launchSoundID,  AssetModelID projectileModelID
 ) {
  WeaponComponent* weapon = &entityManager->weaponComponents[entity];

  weapon->type = type;
  weapon->firingRate = firingRate;
  weapon->projectileSpeed = projectileSpeed;
  weapon->projectileDamage = projectileDamage;
  weapon->range = range;
  weapon->heatGenerated = heatGenerated;
  weapon->launchSoundID = launchSoundID;
  weapon->projectileModelID = projectileModelID;


  weapon->cooldownTimer = 0.0f;

  entityManager->componentMasks[entity] |= COMPONENT_WEAPON;
}

void AddLifetimeComponent (EntityManager* entityManager, Entity entity, float lifetime){
  LifetimeComponent* lt = &entityManager->lifetimeComponents[entity];

  lt->lifetime = lifetime;
  lt->currentTime = lifetime;

  entityManager->componentMasks[entity] |= COMPONENT_LIFETIME;
}

void AddProjectileComponent(EntityManager* entityManager, Entity entity, Entity owner, float damage, bool destroyOnHit, float blastRadius, Effect hitEffectID, WeaponType type){
  ProjectileComponent* projectile = &entityManager->projectileComponents[entity];

  projectile->owner = owner;
  projectile->damage = damage;
  projectile->destroyOnHit = destroyOnHit;
  projectile->blastRadius = blastRadius;
  projectile->hitEffectID = hitEffectID;

  entityManager->componentMasks[entity] |= COMPONENT_PROJECTILE;
}

void AddWeaponControlComponent(EntityManager* entityManager, Entity entity, AimMode aimMode) {
  WeaponControlComponent* wControl = &entityManager->weaponControlComponents[entity];

  wControl->triggerPulled = false;
  wControl->aimDirection = Vector3Zero();
  wControl->aimMode = aimMode;
  wControl->lockedTarget = MAX_ENTITIES;

  for (int i = 0; i < MAX_WEAPONS_EQUIP; i++){
    wControl->weaponsSlots[i] = MAX_ENTITIES;
    wControl->weaponsGroupMap[i] = 0; // First group by default
  } 

  for (int i = 0; i < MAX_WEAPONS_GROUPS; i++){
    wControl->activeGroup[i] = false;
  }

  // wGroup 1 active by default
  wControl->activeGroup[0] = true;

  entityManager->componentMasks[entity] |= COMPONENT_WEAPON_CONTROL;
}

void AddAIControlComponent(EntityManager* entityManager, Entity entity, float sight, float range, Vector3* patrolPoints, int numPatrolPoints) {
  AIControlComponent* ai = &entityManager->aiControlComponents[entity];

  ai->target = MAX_ENTITIES;
  ai->sightRadius = sight;
  ai->attackRange = range;
  ai->timeSinceLastAction = 0.0f;
  ai->state = 0;
  ai->patrolPoints = patrolPoints;
  ai->numPatrolPoints = numPatrolPoints;
  ai->currentPatrolIndex = 0;

  entityManager->componentMasks[entity] |= COMPONENT_AI_CONTROL;
}

void AddCockpitHUDComponent(EntityManager* entityManager, Entity entity, float maxHeat, float heatPerShot, float cooldown) {
  CockpitHUDComponent* hud = &entityManager->cockpitHUDComponents[entity];

  hud->maxHeat = maxHeat;
  hud->currentHeat = 0.0f;
  hud->heatPerShot = heatPerShot;
  hud->cooldownRate = cooldown;

  entityManager->componentMasks[entity] |= COMPONENT_COCKPIT_HUD;
}

void AddEffectComponent(EntityManager* em, Entity entity, float startSize, float endSize, Color color, AssetTextureID texID, int cols, int rows) {
  EffectComponent* fx = &em->effectComponents[entity];

  fx->startSize = startSize;
  fx->endSize = endSize;
  fx->color = color;

  fx->textureID = texID;
  fx->columns = cols;
  fx->rows = rows;

  if (texID != TEXTURE_ID_COUNT && cols > 0 && rows > 0) {
    fx->totalFrames = cols * rows;
  } else {
    fx->totalFrames = 0;
  }

  em->componentMasks[entity] |= COMPONENT_EFFECT;
}

void createEnemyScout(ResourceManager* resourceManager,EntityManager* entityManager, Vector3 position, Vector3* scoutPoints, int numPoints){

  Model* enemyModel = GetModel(resourceManager, MODEL_ID_ENEMY_SCOUT);

    if (enemyModel != NULL) {
        // Apply scale fix to the shared model
        enemyModel->transform = MatrixScale(0.5f, 0.5f, 0.5f);

        Entity scout = CreateEntity(entityManager);

        AddTransformComponent(entityManager, scout, position);
        AddPhysicsComponent(entityManager, scout, (Vector3){0,0,0}, 0.90f);

        BoundingBox enemyBox = GetModelBoundingBox(*enemyModel); 
        AddCollisionComponent(entityManager, scout, enemyBox, false, false);

        AddHealthComponent(entityManager, scout, 100.0f);
        AddAIControlComponent(entityManager, scout, 50.0f, 10.0f, scoutPoints, numPoints);

        AddRenderComponent(entityManager, scout, enemyModel, WHITE);
    }
}

void createEnemyCombatent(ResourceManager* resourceManager, EntityManager* entityManager, Vector3 position){

  Model* enemyModel = GetModel(resourceManager, MODEL_ID_ENEMY_SCOUT); // Using same model for placeholder

    if (enemyModel != NULL) {
        // Apply scale fix to the shared model
        enemyModel->transform = MatrixScale(1.0f, 1.0f, 1.0f);

        Entity combatent = CreateEntity(entityManager);

        AddTransformComponent(entityManager, combatent, position);
        AddPhysicsComponent(entityManager, combatent, (Vector3){0,0,0}, 0.90f);

        BoundingBox enemyBox = GetModelBoundingBox(*enemyModel); 
        AddCollisionComponent(entityManager, combatent, enemyBox, false, false);

        AddHealthComponent(entityManager, combatent, 150.0f);
        AddAIControlComponent(entityManager, combatent, 60.0f, 15.0f, NULL, 0); // No patrol points for combatent

        AddRenderComponent(entityManager, combatent, enemyModel, WHITE);
    }
}