#include <raylib.h>
#include <raymath.h>
#include "ecs/ecs_entitymanager.h"
#include <string.h>


// Take the address of the entity manager and fill all of it with zero's to takeout junk out of the database
void InitEntityManager(EntityManager* entityManager) {
    memset(entityManager, 0, sizeof(EntityManager));
}

// Gets a new Entity ID from the manager with blank components if there's room available. else gets a invalid ID
Entity CreateEntity(EntityManager* entityManager) {
    if (entityManager->numEntities < MAX_ENTITIES) {
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

void AddPlayerControlComponent(EntityManager* entityManager, Entity entity, Camera *camera, float sensitivity, float maxSpeed, float turnSpeed) {
    PlayerControlComponent* player = &entityManager->playerControlComponents[entity];
    
    // Configurações
    player->camera = camera;
    player->mouseSensitivity = sensitivity;
    player->maxSpeed = maxSpeed;
    player->turnSpeed = turnSpeed;

    // Zero-init the rest
    player->throttle = 0.0f;
    player->turnState = 0.0f;

    player->torsoYaw = 0.0f;
    player->torsoPitch = 0.0f;

    player->headTimer = 0.0f;
    player->walkLerp = 0.0f;
    player->headLerp = 0.0f;
    player->lean = (Vector2){0};

    player->isShooting = false;
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

void AddWeaponComponent(EntityManager* entityManager, Entity entity, float fireRate, float projSpeed, float projDamage) {
    WeaponComponent* weapon = &entityManager->weaponComponents[entity];
    
    weapon->fireRate = fireRate;
    weapon->cooldownTimer = 0.0f;
    weapon->projectileSpeed = projSpeed;
    weapon->projectileDamage = projDamage;
    weapon->isFiring = false;
    
    entityManager->componentMasks[entity] |= COMPONENT_WEAPON;
}

// Caso não tivesse a função Projectile implementada no seu código anterior:
void AddProjectileComponent(EntityManager* entityManager, Entity entity, Entity owner) {
    ProjectileComponent* projectile = &entityManager->projectileComponents[entity];
    
    projectile->owner = owner;
    
    entityManager->componentMasks[entity] |= COMPONENT_PROJECTILE;
}

void AddWeaponControlComponent(EntityManager* entityManager, Entity entity, Entity primary, Entity secondary) {
    WeaponControlComponent* wControl = &entityManager->weaponControlComponents[entity];
    
    wControl->primaryWeapon = primary;
    wControl->secondaryWeapon = secondary;
    
    entityManager->componentMasks[entity] |= COMPONENT_WEAPON_CONTROL;
}

void AddAIControlComponent(EntityManager* entityManager, Entity entity, float sight, float range) {
    AIControlComponent* ai = &entityManager->aiControlComponents[entity];
    
    ai->target = MAX_ENTITIES;
    ai->sightRadius = sight;
    ai->attackRange = range;
    ai->timeSinceLastAction = 0.0f;
    ai->state = 0;
    
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

void AddCollisionComponent(EntityManager* entityManager, Entity entity, BoundingBox bounds, bool isStatic) {
    CollisionComponent* collision = &entityManager->collisionComponents[entity];
    
    collision->bounds = bounds;
    collision->isStatic = isStatic;
    
    entityManager->componentMasks[entity] |= COMPONENT_COLLISION;
}
