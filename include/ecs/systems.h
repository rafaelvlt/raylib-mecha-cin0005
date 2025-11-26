#ifndef ECS_SYSTEMS_H
#define ECS_SYSTEMS_H
#include "screens/screen_first_level.h"

struct Systems;

#include <raylib.h>

// Updates every position applying physics
void MovementSystem(struct Systems* systems);

// Draw every visible Entity
void RenderSystem(struct Systems* systems);
void EffectSystem(struct Systems* systems, Camera* camera);

//Draw HUD
void DrawHUDSystem(struct Systems* systems);
void DrawCrosshair(struct Systems* systems);
void DrawMinimapSystem(struct Systems* systems, FirstLevelData* data);
void Hud3DSystem(struct Systems* systems);
// Effects helper, should be in updates
void TrailSystem(struct Systems* systems);

// Deals with all types of collisions
void CollisionSystem(struct Systems* systems);

// For Mecha parts other than torso
void AttachmentSystem(struct Systems* systems);

// For deleting temporary entities, like bullets or other things
void LifetimeSystem(struct Systems* systems);

// Generalist Combat Systems: Works both for the player and for the AI
void WeaponSystem(struct Systems* systems);
void ProjectileSystem(struct Systems* systems);
void MissileSystem(struct Systems* systems);
void HealthSystem(struct Systems* systems);

//Player Systems
void PlayerControlSystem(struct Systems* systems);
void PlayerAudioSystem(struct Systems* systems);

//Enemy AI Systems
void AIControlSystem(struct Systems* systems);
void Hudsystem(struct Systems* systems);

#endif // ECS_SYSTEMS_H
