#ifndef ECS_TYPES_H
#define ECS_TYPES_H

typedef enum{
  ENTITY_PLAYER = 0,
  ENTITY_ENEMY_SCOUT,
  ENTITY_ENEMY_FIGHTER,
  ENTITY_TURRET_STRUCTURE,
  ENTITY_COUNT
} EntityType;

typedef enum{
  WEAPON_TYPE_NONE = 0,
  WEAPON_PULSE_LASER,
  WEAPON_LASER_BEAM,
  WEAPON_MISSILE_LAUNCHER,
  WEAPON_MACHINE_GUN,
  WEAPON_COUNTER
} WeaponType;

typedef enum{
  AIM_MODE_PHYSICAL = 0,
  AIM_MODE_CAMERA,
} AimMode;

typedef enum{
  FX_TYPE_SPRITESHEET = 0,
  FX_TYPE_ARRAY,
  FX_TYPE_STATIC,
} EffectRenderType;

typedef enum {
  AI_STATE_PATROL = 0,
  AI_STATE_CHASE = 1,
  AI_STATE_ATTACK = 2
} AIState;

// Animation indices for enemy scout model
typedef enum {
  SCOUT_ANIM_IDLE = 0,            
  SCOUT_ANIM_DAMAGE_RIGHT = 1,   
  SCOUT_ANIM_DAMAGE_FRONT_RIGHT = 2,
  SCOUT_ANIM_DAMAGE_LEFT = 3,      
  SCOUT_ANIM_DAMAGE_FRONT_LEFT = 4,
  SCOUT_ANIM_WALK = 5             
} ScoutAnimationID;

#endif
