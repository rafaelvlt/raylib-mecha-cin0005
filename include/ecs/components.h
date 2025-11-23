#ifndef ECS_COMPONENTS_H
#define ECS_COMPONENTS_H
#include <raylib.h>
#include <stdint.h>
#include "resource_manager.h"
#include "types.h"

// Definition for maximum array size, and typedef for UID
#define MAX_ENTITIES 1024
#define MAX_WEAPONS_EQUIP 8
#define MAX_WEAPONS_GROUPS 5

typedef uint32_t Entity;


/* =========================== Components definitions =============================*/

// Used for position drawn in the world
typedef struct  {
  Vector3 position;
  Quaternion orientation;
} TransformComponent;

// Used for movement systems
typedef struct  {
  Vector3 velocity;
  Vector3 acceleration;
  float drag;
} PhysicsComponent;

// For Collision system suport
typedef struct {
  BoundingBox hitbox;
  bool isStatic;
  bool isTrigger;
} CollisionComponent;

// Used for model drawing in the world
typedef struct  {
  Model* model;
  Color tint;
  bool isVisible;
} RenderComponent;

// Used by multi-model objects to know where to attach themselves (i.e mech with multiple parts)
typedef struct {
  Entity parent;
  Vector3 offsetPosition;
  Quaternion offsetRotation;
} AttachmentComponent;

// Used in player control for camera movement
typedef struct {
  Camera *camera;          
  float mouseSensitivity;
  // Movement
  float maxSpeed;
  float legAngle;
  float turnSpeed;
  float throttle;       
  float turnState;      
  float torsoYaw;       
  float torsoPitch;     

  //Audio
  float hydraulicVolume;

  //flags
  bool isMoving;
  bool isRotating; 
  bool isZooming;
  bool centeringTorsotoLegs;
  bool centeringLegstoTorso;
  bool lockTargetRequested;

  // Camera Animation 
  float headTimer;
  float lastHeadTimer;
  float walkLerp;
  float headLerp;
  Vector2 lean;
} PlayerControlComponent;

// Used in damage calculations and destroyed parts(maybe)
typedef struct  {
  float currentHealth;
  float maxHealth;
}  HealthComponent;

// Used for stats about a weapon
typedef struct  {
  WeaponType type;

  // State
  float cooldownTimer;
  
  // Attributes
  float firingRate;
  float projectileSpeed;
  float projectileDamage;
  float range;

  // TBD HEAT
  float heatGenerated;
  
  // Animation
  AssetSoundID launchSoundID;
  AssetModelID projectileModelID;
}  WeaponComponent;

// Used to prevent own death
typedef struct  {
  Entity owner;
  float damage; // copied from WeaponComponent
  bool destroyOnHit;

  //Animation
  float blastRadius;
  WeaponType type;
  Effect hitEffectID;
} ProjectileComponent;

typedef struct{
  float lifetime;
  float currentTime;
} LifetimeComponent;

// Detect which weapon to use and fire with for both player and A.I
typedef struct {
  // Control Proprieties 
  bool triggerPulled;
  Vector3 aimDirection;
  AimMode aimMode;
  Entity lockedTarget;
  
  //Weapon Group
  Entity weaponsSlots[MAX_WEAPONS_EQUIP];
  int weaponsGroupMap[MAX_WEAPONS_EQUIP];
  int activeGroup[MAX_WEAPONS_GROUPS];
} WeaponControlComponent;


// Used by the enemies
typedef struct  {
  Entity target;
  float sightRadius;
  float attackRange;
  float timeSinceLastAction;
  int state; // probably change to a enum later, 0 = patrol 1 = attack;
} AIControlComponent;

// Dumb container for HUD numbers
typedef struct  {
  float currentHeat;
  float maxHeat;
  float heatPerShot;
  float cooldownRate;
} CockpitHUDComponent;

typedef struct {
  float startSize; 
  float endSize;   
  Color color;
  AssetTextureID textureID; // If textureID = 0, procedural 
  int columns;
  int rows;
  int totalFrames;
} EffectComponent;

/* =======================================
  Bitmask for querying by the systems
==========================================*/ 

typedef enum {
  COMPONENT_NONE = 0,
  COMPONENT_TRANSFORM = 1 << 0,
  COMPONENT_PHYSICS = 1 << 1,
  COMPONENT_RENDER = 1 << 2,
  COMPONENT_ATTACHMENT = 1 << 3,
  COMPONENT_PLAYER_CONTROL = 1 << 4,
  COMPONENT_HEALTH = 1 << 5,
  COMPONENT_WEAPON = 1 << 6,
  COMPONENT_LIFETIME = 1 << 7,
  COMPONENT_PROJECTILE = 1 << 8,
  COMPONENT_WEAPON_CONTROL = 1 << 9,
  COMPONENT_AI_CONTROL = 1 << 10,
  COMPONENT_COCKPIT_HUD = 1 << 11,
  COMPONENT_COLLISION = 1 << 12,
  COMPONENT_EFFECT = 1 << 13,
} ComponentMask;

#endif //components.h
