#ifndef ECS_ENTITYMANAGER_H
#define ECS_ENTITYMANAGER_H
#include "components.h"
#include "ecs/types.h"
#include "raylib.h"

typedef struct  {
  // Array for every component
  TransformComponent      transformComponents[MAX_ENTITIES];
  PhysicsComponent        physicsComponents[MAX_ENTITIES];
  RenderComponent         renderComponents[MAX_ENTITIES];
  AnimationComponent      animationComponents[MAX_ENTITIES];
  AttachmentComponent     attachmentComponents[MAX_ENTITIES];
  PlayerControlComponent  playerControlComponents[MAX_ENTITIES];
  HealthComponent         healthComponents[MAX_ENTITIES];
  WeaponComponent         weaponComponents[MAX_ENTITIES];
  LifetimeComponent       lifetimeComponents[MAX_ENTITIES];
  ProjectileComponent     projectileComponents[MAX_ENTITIES];
  WeaponControlComponent  weaponControlComponents[MAX_ENTITIES];
  AIControlComponent      aiControlComponents[MAX_ENTITIES];
  CockpitHUDComponent     cockpitHUDComponents[MAX_ENTITIES];
  CollisionComponent      collisionComponents[MAX_ENTITIES];
  EffectComponent         effectComponents[MAX_ENTITIES];
  HomingComponent         homingComponents[MAX_ENTITIES];
  HeatComponent           heatComponents[MAX_ENTITIES];
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

void AddPhysicsComponent(EntityManager* entityManager, Entity entity, Vector3 velocity, float drag);

void AddCollisionComponent(EntityManager* entityManager, Entity entity, BoundingBox hitbox, bool isStatic, bool isTrigger);

void AddRenderComponent(EntityManager* entityManager, Entity entity, Model* model, Color tint);

void AddAnimationComponent(EntityManager* entityManager, Entity entity, AssetModelID modelId, int startAnim, float playbackSpeed, bool loop);

void AddAttachmentComponent(EntityManager* entityManager, Entity entity, Entity parent, Vector3 offsetPos, Quaternion offsetRot);

void AddPlayerControlComponent(EntityManager* entityManager, Entity entity, Camera *camera);

void AddHealthComponent(EntityManager* entityManager, Entity entity, float health);

void AddHeatComponent(EntityManager* entityManager, Entity entity, float maxHeat, float dissipationRate, float penaltyDuration);

void AddWeaponComponent(EntityManager* em, Entity entity, WeaponType type, 
                        float fireRate, float projSpeed, float damage, float range, float heat, 
                        int burstTotal, float burstRate);

void AddLifetimeComponent (EntityManager* entityManager, Entity entity, float lifetime);

void AddProjectileComponent(EntityManager* entityManager, Entity entity, Entity owner, float damage, bool destroyOnHit, float blastRadius, WeaponType type);

void AddWeaponControlComponent(EntityManager* entityManager, Entity entity, AimMode aimMode);

void AddAIControlComponent(EntityManager* entityManager, Entity entity, float sight, float range, Vector3* patrolPoints, int numPatrolPoints);

void AddCockpitHUDComponent(EntityManager* entityManager, Entity entity, float maxHeat, float heatPerShot, float cooldown);

void AddEffectSheet(EntityManager* em, Entity entity, float startSize, float endSize, Color color, float duration, 
                    AssetTextureID texID, int cols, int rows, bool looping);

void AddEffectArray(EntityManager* em, Entity entity, float startSize, float endSize, Color color, float duration, 
                    AssetTextureID id, int count, bool looping);

void AddHomingComponent(EntityManager* em, Entity entity, Entity target, float turnSpeed, float speed, float armingTime);

void createEnemyScout(ResourceManager* resourceManager, EntityManager* entityManager, Vector3 position, Vector3* scoutPoints, int numPoints);

void createEnemyCombatent(ResourceManager* resourceManager, EntityManager* entityManager, Vector3 position);

#endif // ECS_ENTITYMANAGER_H
