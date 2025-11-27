#ifndef ECS_COMPONENTS_H
#define ECS_COMPONENTS_H
#include <raylib.h>
#include <stdint.h>
#include "resource_manager.h"
#include "types.h"

// Definition for maximum array size, and typedef for UID
#define MAX_ENTITIES 2048
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
  bool wasZooming;
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
  bool hasTakenDamage;
}  HealthComponent;

// Used for stats about a weapon
typedef struct  {
  WeaponType type;

  // State
  float cooldownTimer;
  int burstCount;
  float burstTimer;


  // Attributes
  float firingRate;
  float projectileSpeed;
  float projectileDamage;
  float range;
  float burstTotal;
  float burstRate;

  // TBD HEAT
  float heatGenerated;
}  WeaponComponent;

// Used to prevent own death
typedef struct  {
  Entity owner;
  float damage; // copied from WeaponComponent
  bool destroyOnHit;

  //Animation
  float blastRadius;
  WeaponType type;
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
  float sightAngle;
  float attackRange;
  float timeSinceLastAction;
  int state; // probably change to a enum later, 0 = patrol 1 = chase 2 = attack;
  Vector3* patrolPoints; // null for fighters
  int numPatrolPoints; // 0 for fighters
  int currentPatrolIndex;
} AIControlComponent;

// Dumb container for HUD numbers
typedef struct  {
  float maxSpeed;
  float currentSpeed;
  float maxHeat;
  float currentHeat;
  float heatPerShot;
  float cooldownRate;
  float LRMammocount;
  float MGammocount;
} CockpitHUDComponent;

typedef struct {
  EffectRenderType type;
  Color color;
  float startSize;
  float endSize;
  float rotation;
  bool loop;

  int totalFrames;

  union {
    struct {
      AssetTextureID id;
      int columns;
      int rows;
    } sheet;

    struct {
      AssetTextureID frameIDstart; 
    } arr;

  } data;
} EffectComponent;

typedef struct {
  Entity target; 
  float turnSpeed;
  float speed;   
  float armingTime;
  float timer;    
} HomingComponent;

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
  COMPONENT_HOMING = 1 << 14,
} ComponentMask;

#endif //components.h
