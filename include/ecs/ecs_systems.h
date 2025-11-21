#ifndef ECS_SYSTEMS_H
#define ECS_SYSTEMS_H

struct Systems;



// Movement System: Update every position applying physics
void MovementSystem(struct Systems* systems);

// Drawing System: Draw every visible Entity
void RenderSystem(struct Systems* systems);

// For Mecha parts other than torso
void AttachmentSystem(struct Systems* systems);


// Generalist Shooting Systems: Works both for the player and for the AI
void WeaponSystem(struct Systems* systems);
void ProjectileSystem(struct Systems* systems);

//Player Systems
void PlayerControlSystem(struct Systems* systems);
void PlayerAudioSystem(struct Systems* systems);

//Enemy AI Systems
void AIControlSystem(struct Systems* systems);

#endif // ECS_SYSTEMS_H
