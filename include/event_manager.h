#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <raylib.h>
#include "ecs/ecs_components.h"
#include "ecs/ecs_entitymanager.h"
#include "ecs/ecs_systems.h"
#include "ecs/ecs_types.h"
#include "state_manager.h"

#define MAX_EVENTS_QUANTITY 256

struct Systems;

typedef enum {
  EVENT_NONE = 0,
  EVENT_WEAPON_FIRED,
  EVENT_COLISSION,
  EVENT_ENTITY_DEATH,
  EVENT_SCREEN_CHANGE,
} EventType;

typedef struct{
  Entity owner;
  WeaponType weapon;
  Vector3 direction;
} WeaponFiredData;

typedef struct {
    Entity attacker;   
    Entity victim;    
    float damageAmount;
    Vector3 impactPoint;
} ColissionData;

typedef struct{
  EntityType type;
  Entity owner;
} EntityDeathData;

typedef struct {
  GameScreen nextScreen;
} ScreenChangeData;

typedef union {
  WeaponFiredData weaponFired;
  ColissionData colissionDetected;
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
void ClearEventManager(EventManager* em);

#endif
