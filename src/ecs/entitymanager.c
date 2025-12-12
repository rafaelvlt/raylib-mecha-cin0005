#include <raylib.h>
#include <raymath.h>
#include "ecs/entitymanager.h"
#include "ecs/components.h"
#include "ecs/types.h"
#include "resource_manager.h"
#include <string.h>
#include "ecs/systems.h"


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

void AddAnimationComponent(EntityManager* entityManager, Entity entity, AssetModelID modelId, int startAnim, float playbackSpeed, bool loop) {
  AnimationComponent* anim = &entityManager->animationComponents[entity];

  anim->modelId = modelId;
  anim->currentAnim = startAnim;
  anim->currentTime = 0.0f;
  anim->playbackSpeed = playbackSpeed;
  anim->loop = loop;

  entityManager->componentMasks[entity] |= COMPONENT_ANIMATION;
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
  player->maxSpeed = 25.0f;
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
  player->wasZooming = false;
  player->lockTargetRequested = false;

  entityManager->componentMasks[entity] |= COMPONENT_PLAYER_CONTROL;
}

void AddHealthComponent(EntityManager* entityManager, Entity entity, float health) {
  HealthComponent* hp = &entityManager->healthComponents[entity];

  hp->currentHealth = health;
  hp->maxHealth = health;
  hp->hasTakenDamage = false;
  hp->lastDamageDirection = (Vector3){0, 0, 0};
  hp->damageReactionTimer = 0.0f;

  entityManager->componentMasks[entity] |= COMPONENT_HEALTH;
}

void AddWeaponComponent(EntityManager* em, Entity entity, WeaponType type, 
                        float fireRate, float projSpeed, float damage, float range, float heat, 
                        int burstTotal, float burstRate)
{

  WeaponComponent* w = &em->weaponComponents[entity];

  // base Stats
  w->type = type;
  w->firingRate = fireRate;
  w->projectileSpeed = projSpeed;
  w->projectileDamage = damage;
  w->range = range;
  w->heatGenerated = heat;

  // Burst stats
  w->burstTotal = burstTotal;
  w->burstRate = burstRate;


  // Zero-init
  w->cooldownTimer = 0.0f;
  w->burstCount = 0;     
  w->burstTimer = 0.0f;

  em->componentMasks[entity] |= COMPONENT_WEAPON;
}

void AddLifetimeComponent (EntityManager* entityManager, Entity entity, float lifetime){
  LifetimeComponent* lt = &entityManager->lifetimeComponents[entity];

  lt->lifetime = lifetime;
  lt->currentTime = lifetime;

  entityManager->componentMasks[entity] |= COMPONENT_LIFETIME;
}

void AddProjectileComponent(EntityManager* entityManager, Entity entity, Entity owner, float damage, bool destroyOnHit, float blastRadius,  WeaponType type){
  ProjectileComponent* projectile = &entityManager->projectileComponents[entity];

  projectile->owner = owner;
  projectile->damage = damage;
  projectile->destroyOnHit = destroyOnHit;
  projectile->blastRadius = blastRadius;
  projectile->type = type;

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
  ai->state = AI_STATE_PATROL;
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

static EffectComponent* AddBaseEffect(EntityManager* em, Entity entity, float startSize, float endSize, Color color, float duration, bool looping) {
    EffectComponent* fx = &em->effectComponents[entity];
    fx->startSize = startSize;
    fx->endSize = endSize;
    fx->color = color;
    fx->rotation = 0.0f;
    fx->loop = looping;
    em->componentMasks[entity] |= COMPONENT_EFFECT;
    
    AddLifetimeComponent(em, entity, duration);
    
    return fx;
}

void AddEffectSheet(EntityManager* em, Entity entity, float startSize, float endSize, Color color, float duration, 
                     AssetTextureID texID, int cols, int rows, bool looping) {
    
    EffectComponent* fx = AddBaseEffect(em, entity, startSize, endSize, color, duration, looping);
    
    fx->type = FX_TYPE_SPRITESHEET;
    fx->totalFrames = cols * rows;
    
    // Preenche a Union
    fx->data.sheet.id = texID;
    fx->data.sheet.columns = cols;
    fx->data.sheet.rows = rows;
}

void AddEffectArray(EntityManager* em, Entity entity, float startSize, float endSize, Color color, float duration, 
                     AssetTextureID id, int count, bool looping) {
                         
    EffectComponent* fx = AddBaseEffect(em, entity, startSize, endSize, color, duration, looping);
    
    fx->type = FX_TYPE_ARRAY;
    fx->totalFrames = count;
    fx->data.arr.frameIDstart = id;
}

void AddHomingComponent(EntityManager* em, Entity entity, Entity target, float turnSpeed, float speed, float armingTime){
  HomingComponent* hm = &em->homingComponents[entity];

  hm->target = target;
  hm->turnSpeed = turnSpeed;
  hm->speed = speed;
  hm->armingTime = armingTime;
  hm->timer = 0.0f;

  em->componentMasks[entity] |= COMPONENT_HOMING;
}
