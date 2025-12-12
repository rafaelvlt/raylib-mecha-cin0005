#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <raylib.h>
#include "ecs/components.h"
#include "ecs/types.h"
#include "state_manager.h"

#define MAX_EVENTS_QUANTITY 256

struct Systems;

typedef enum {
  EVENT_NONE = 0,
  EVENT_WEAPON_FIRED,
  EVENT_PROJECTILE_COLLISION,
  EVENT_ENTITY_DEATH,
  EVENT_SCREEN_CHANGE,
} EventType;

typedef struct{
  Entity owner;
  WeaponType weapon;
  Vector3 direction;
  Vector3 position;
  Entity projectileEntity;
} WeaponFiredData;

typedef struct {
  Entity attacker;   
  Entity victim;    
  float damageAmount;
  Vector3 impactPoint;
  WeaponType type;
} ProjectileCollisionData;

typedef struct{
  EntityType type;
  Entity owner;
  Entity killer;
  Vector3 pos;
} EntityDeathData;

typedef struct {
  GameScreen nextScreen;
} ScreenChangeData;

typedef union {
  WeaponFiredData weaponFired;
  ProjectileCollisionData projectileCollisionDetected;
  EntityDeathData deathEvent;
  ScreenChangeData screenChange;
} EventData;

typedef struct{
  EventType type;
  EventData data;
} Event;

typedef struct{
  Event eventQueue[MAX_EVENTS_QUANTITY];
  int eventCounter;
} EventManager;

void InitEventManager(struct Systems* systems);
void PushEvent(struct Systems* systems, EventType type, EventData data);
void ProcessGameEvents(struct Systems* systems);
void ClearEventManager(EventManager* em);

// Integration with other systems
void EffectSystemOnEvent(struct Systems* systems, Event event);
#endif
