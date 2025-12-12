#include <raylib.h>
#include <raymath.h>
#include "ecs/types.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "state_manager.h"
#include "systems.h"

<<<<<<< HEAD
// Helper Functions
=======
// Forward declarations
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
static AssetSoundID GetGunfightSoundByType(EventType et, WeaponType wt);
static void PlaySpatialGunfightSound(struct Systems* systems, Event event);
static void PlayEnemyDeath(struct Systems* systems);
static void PlayTargetDestroyed(struct Systems* systems);
<<<<<<< HEAD
=======
static void FindListenerPositionAndOrientation(struct Systems* systems, Vector3* pos, Vector3* right);
static void CalculateSpatialAudioParams(Vector3 listenerPos, Vector3 listenerRight, Vector3 soundPos, float baseVolume, float* finalVolume, float* finalPan);
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee

void InitAudioManager(struct Systems* systems){

  InitAudioDevice();
  systems->audioManager.playingNow = GetMusic(&systems->resourceManager, MUSIC_ID_MENU);
  systems->audioManager.playingNow->looping = true;

}

void AudioManagerOnEvent(struct Systems* systems, Event event) {
  if (event.type == EVENT_WEAPON_FIRED || event.type == EVENT_PROJECTILE_COLLISION) {
    PlaySpatialGunfightSound(systems, event);
  }
  if (event.type == EVENT_ENTITY_DEATH){
    if (event.data.deathEvent.type == ENTITY_TURRET_STRUCTURE) PlayTargetDestroyed(systems);
    else PlayEnemyDeath(systems);
  }
}

void UpdateAudioManager(struct Systems* systems){
<<<<<<< HEAD
  // If the current screen is one of the menu screen, stay playing the music
  Music* targetMusic = NULL; 
  Music* currentMusic = systems->audioManager.playingNow;

  if (systems->stateManager.currentScreen < SCREEN_FIRST_LEVEL) {
=======
  // Determine target music based on current screen
  Music* targetMusic = NULL;
  if (systems->stateManager.currentScreen < SCREEN_DEBRIEFING) {
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
    targetMusic = GetMusic(&systems->resourceManager, MUSIC_ID_MENU);
  }
  else if (systems->stateManager.currentScreen == SCREEN_FIRST_LEVEL) {
    targetMusic = GetMusic(&systems->resourceManager, MUSIC_ID_FIRST_LEVEL);
  }
<<<<<<< HEAD
  else {
    targetMusic = NULL;
  }

=======

  Music* currentMusic = systems->audioManager.playingNow;

  // Switch music if needed
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  if (targetMusic != currentMusic){
    if (currentMusic != NULL) StopMusicStream(*currentMusic);
    if (targetMusic != NULL) PlayMusicStream(*targetMusic);
    systems->audioManager.playingNow = targetMusic;
  }

<<<<<<< HEAD
  if (systems->audioManager.playingNow != NULL){
    float volume = systems->configManager.audioVolume;
=======
  // Update current music stream
  if (systems->audioManager.playingNow != NULL){
    float volume = GetAudioVolume(&systems->configManager);
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
    SetMusicVolume(*systems->audioManager.playingNow, volume);
    UpdateMusicStream(*systems->audioManager.playingNow);
  }
}

void ShutdownAudioManager(){
<<<<<<< HEAD
    CloseAudioDevice();     // Close audio device (music streaming is automatically stopped)
=======
  CloseAudioDevice();     // Close audio device (music streaming is automatically stopped)
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
}


static AssetSoundID GetGunfightSoundByType(EventType et, WeaponType wt){
  if (et == EVENT_WEAPON_FIRED){
    switch (wt) {
      case WEAPON_PULSE_LASER:   return SOUND_ID_PULSE_LASER_FIRING;
      case WEAPON_MISSILE_LAUNCHER: return SOUND_ID_MISSILE_LAUNCHER_FIRING;
      default: return SOUND_ID_COUNT;
    }
  }
  else{
    switch (wt) {
      case WEAPON_PULSE_LASER:  return SOUND_ID_PULSE_LASER_IMPACT;
      case WEAPON_MISSILE_LAUNCHER: return SOUND_ID_MISSILE_LAUNCHER_IMPACT;
      default: return SOUND_ID_COUNT;
    }
  }
}

static void PlaySpatialGunfightSound(struct Systems* systems, Event event) {
<<<<<<< HEAD
  EntityManager* ecs = &systems->entityManager;

  WeaponType wt;
  Vector3 soundPos;
  
  // Gets sound and pos based on event type
  if (event.type == EVENT_PROJECTILE_COLLISION) {
    wt = event.data.projectileCollisionDetected.type;
    soundPos = event.data.projectileCollisionDetected.impactPoint; 
  } else {
=======
  // Extract weapon type and sound position from event
  WeaponType wt;
  Vector3 soundPos;

  if (event.type == EVENT_PROJECTILE_COLLISION) {
    wt = event.data.projectileCollisionDetected.type;
    soundPos = event.data.projectileCollisionDetected.impactPoint; 
  }
  else {
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
    wt = event.data.weaponFired.weapon;
    soundPos = event.data.weaponFired.position;
  }

<<<<<<< HEAD
=======
  // Get sound asset
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  AssetSoundID sID = GetGunfightSoundByType(event.type, wt);
  if (sID >= SOUND_ID_COUNT) return; 

  Sound* sfxPtr = GetSound(&systems->resourceManager, sID);
  if (sfxPtr == NULL) return;
  Sound sfx = *sfxPtr;

<<<<<<< HEAD
  // Sfx editing depends on distance, listener, etc
  float finalVolume = systems->configManager.audioVolume;
  float finalPan = 0.5f;
  const float HEARING_DISTANCE = 200.0f;
  
  // Gets the camera pos for distance calculations
  Vector3 listenerPos = {0};
  Vector3 listenerRight = {1, 0, 0};
  bool hasListener = false;

  for (int i = 0; i < ecs->numEntities || !hasListener; i++) {
    if (ecs->componentMasks[i] & COMPONENT_PLAYER_CONTROL) {
      Camera* cam = ecs->playerControlComponents[i].camera;
      if (cam) {
        listenerPos = cam->position;
        Vector3 forward = Vector3Normalize(Vector3Subtract(cam->target, cam->position));
        listenerRight = Vector3Normalize(Vector3CrossProduct(forward, (Vector3){0,1,0}));
        hasListener = true;
      }
    }
  }
  
  // Distance
  if (hasListener) {
    float dist = Vector3Distance(listenerPos, soundPos);
    if (dist > 0.1f) {
      float attenuation = 1.0f - (dist / HEARING_DISTANCE);
      if (attenuation < 0.0f) attenuation = 0.0f;
      finalVolume *= attenuation;
    }

    // Panning
    Vector3 dirToSound = Vector3Normalize(Vector3Subtract(soundPos, listenerPos));
    float dotRight = Vector3DotProduct(dirToSound, listenerRight);
    finalPan = (dotRight + 1.0f) / 2.0f;
  }

  // Applies everything and play
=======
  // Find listener position and orientation (player camera)
  Vector3 listenerPos = {0};
  Vector3 listenerRight = {1, 0, 0};
  FindListenerPositionAndOrientation(systems, &listenerPos, &listenerRight);

  // Calculate spatial audio parameters (volume attenuation and panning)
  float finalVolume = GetAudioVolume(&systems->configManager);
  float finalPan = 0.5f;
  CalculateSpatialAudioParams(listenerPos, listenerRight, soundPos, finalVolume, &finalVolume, &finalPan);

  // Apply effects and play sound
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  SetSoundVolume(sfx, finalVolume);
  SetSoundPan(sfx, finalPan);          
  float pitch = 0.95f + ((float)GetRandomValue(-5, 5) / 100.0f);
  SetSoundPitch(sfx, pitch);

  if (finalVolume >= 0.01f) PlaySound(sfx);
}




static void PlayEnemyDeath(struct Systems* systems){
  Sound* deathSfx = GetSound(&systems->resourceManager, SOUND_ID_ENEMY_MECH_DESTROYED);
<<<<<<< HEAD
  SetSoundVolume(*deathSfx, systems->configManager.audioVolume);
=======
  SetSoundVolume(*deathSfx, GetAudioVolume(&systems->configManager));
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  PlaySound(*deathSfx);
}


static void PlayTargetDestroyed(struct Systems* systems){
  Sound* destroyedSfx = GetSound(&systems->resourceManager, SOUND_ID_ENEMY_TARGET_DESTROYED);
<<<<<<< HEAD
  SetSoundVolume(*destroyedSfx, systems->configManager.audioVolume);
  PlaySound(*destroyedSfx);
}
=======
  SetSoundVolume(*destroyedSfx, GetAudioVolume(&systems->configManager));
  PlaySound(*destroyedSfx);
}

// Finds the player camera position and right vector for spatial audio calculations
static void FindListenerPositionAndOrientation(struct Systems* systems, Vector3* pos, Vector3* right) {
  EntityManager* ecs = &systems->entityManager;

  for (int i = 0; i < ecs->numEntities; i++) {
    if (ecs->componentMasks[i] & COMPONENT_PLAYER_CONTROL) {
      Camera* cam = ecs->playerControlComponents[i].camera;
      if (cam) {
        *pos = cam->position;
        Vector3 forward = Vector3Normalize(Vector3Subtract(cam->target, cam->position));
        *right = Vector3Normalize(Vector3CrossProduct(forward, (Vector3){0,1,0}));
        return;
      }
    }
  }
}

// Calculates volume attenuation and panning based on distance and direction to sound
static void CalculateSpatialAudioParams(Vector3 listenerPos, Vector3 listenerRight, Vector3 soundPos, float baseVolume, float* finalVolume, float* finalPan) {
  const float HEARING_DISTANCE = 200.0f;

  // Calculate volume attenuation based on distance
  float dist = Vector3Distance(listenerPos, soundPos);
  if (dist > 0.1f) {
    float attenuation = 1.0f - (dist / HEARING_DISTANCE);
    if (attenuation < 0.0f) attenuation = 0.0f;
    *finalVolume = baseVolume * attenuation;
  }
  else {
    *finalVolume = baseVolume;
  }

  // Calculate panning based on direction to sound
  Vector3 dirToSound = Vector3Normalize(Vector3Subtract(soundPos, listenerPos));
  float dotRight = Vector3DotProduct(dirToSound, listenerRight);
  *finalPan = (dotRight + 1.0f) / 2.0f;
}
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
