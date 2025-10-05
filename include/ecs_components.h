#ifndef ECS_COMPONENTS_H
#define ECS_COMPONENTS_H
#include <raylib.h>
#include <stdint.h>

// Definition for maximum arary size, and typedef for UID
#define MAX_ENTITIES 1024
typedef uint32_t Entity;

/* ===========================
  Components definitions
=============================*/

// Used for position drawn in the world
typedef struct  {
    Vector3 position;
    Quaternion orientation;
} TransformComponent;

// Used for movement systems
typedef struct  {
    Vector3 velocity;
    Vector3 acceleration;
} PhysicsComponent;

// Used for model drawing in the world
typedef struct  {
    Model model;
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
    float cameraYaw;        
    float cameraPitch;      
    float mouseSensitivity; 
} PlayerControlComponent;

// Used in damage calculations and destroyed parts(maybe)
typedef struct  {
  float currentHealth;
  float maxHealth;
}  HealthComponent;

// Used for stats about a weapon
typedef struct  {
  float fireRate;
  float cooldownTimer;
  float projectileSpeed;
  float projectileDamage;
  bool isFiring;
}  WeaponComponent;

// Used to prevent own death
typedef struct  {
  Entity owner;
}  ProjectileComponent;

// Detect which weapon to use and fire with for both player and A.I
typedef struct {
  Entity primaryWeapon;
  Entity secondaryWeapon;
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
  COMPONENT_PROJECTILE = 1 << 7,
  COMPONENT_WEAPON_CONTROL = 1 << 8,
  COMPONENT_AI_CONTROL = 1 << 9,
  COMPONENT_COCKPIT_HUD = 1 << 10
} ComponentMask;

#endif //ECS_COMPONENTS_H
