#ifndef ECS_ENTITYMANAGER_H
#define ECS_ENTITYMANAGER_H
#include "ecs/ecs_components.h"

typedef struct  {
  // Array for every component
  TransformComponent      transformComponents[MAX_ENTITIES];
  PhysicsComponent        physicsComponents[MAX_ENTITIES];
  RenderComponent         renderComponents[MAX_ENTITIES];
  AttachmentComponent     attachmentComponents[MAX_ENTITIES];
  PlayerControlComponent  playerControlComponents[MAX_ENTITIES];
  HealthComponent         healthComponents[MAX_ENTITIES];
  WeaponComponent         weaponComponents[MAX_ENTITIES];
  ProjectileComponent     projectileComponents[MAX_ENTITIES];
  WeaponControlComponent  weaponControlComponents[MAX_ENTITIES];
  AIControlComponent      aiControlComponents[MAX_ENTITIES];
  CockpitHUDComponent     cockpitHUDComponents[MAX_ENTITIES];
  
  // Bitmask for every Entity
  uint32_t                componentMasks[MAX_ENTITIES];
  // Number of entities active/created
  uint32_t                numEntities;
} EntityManager;

/* ==========================
  Entity Manager Functions
============================= */

// Prepares for Entity Manager, zeroing out the memory addresses for it, basically it's a zero init for everything.
void InitEntityManager(EntityManager* entityManager);

// Gets a new ID from the manager
Entity CreateEntity(EntityManager* entityManager);

// Entity ID gets on the "discard pile". Can be used by the manager to create new entities
void DestroyEntity(EntityManager* entityManager, Entity entity);

// Same as Init, but better name
void ClearEntityManager(EntityManager* entityManager);

// Functions to add components to entities.
void AddTransformComponent(EntityManager* entityManager, Entity entity, Vector3 position);
void AddPhysicsComponent(EntityManager* entityManager, Entity entity, Vector3 velocity);
void AddRenderComponent(EntityManager* entityManager, Entity entity, Model* model, Color tint);
void AddAttachmentComponent(EntityManager* entityManager, Entity entity, Entity parent, Vector3 offsetPos, Quaternion offsetRot);
void AddPlayerControlComponent(EntityManager* entityManager, Entity entity, Camera *camera, float sensitivity);
void AddHealthComponent(EntityManager* entityManager, Entity entity, float health);
void AddWeaponComponent(EntityManager* entityManager, Entity entity, float fireRate, float projSpeed, float projDamage);
void AddWeaponControlComponent(EntityManager* entityManager, Entity entity, Entity primary, Entity secondary);
void AddAIControlComponent(EntityManager* entityManager, Entity entity, float sight, float range);
void AddCockpitHUDComponent(EntityManager* entityManager, Entity entity, float maxHeat, float heatPerShot, float cooldown);

#endif // ECS_ENTITYMANAGER_H
