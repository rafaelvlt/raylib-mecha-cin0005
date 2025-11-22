#include <raylib.h>
#include <raymath.h>
#include "ecs/types.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "state_manager.h"
#include "systems.h"

static AssetSoundID GetSoundForWeapon(WeaponType weaponType);


void InitAudioManager(struct Systems* systems){

    InitAudioDevice();
    systems->audioManager.playingNow = GetMusic(&systems->resourceManager, MUSIC_ID_MENU);
    systems->audioManager.playingNow->looping = true;

}
void UpdateAudioManager(struct Systems* systems){
  EventManager* es = &systems->eventManager;

  // If the current screen is one of the menu screen, stay playing the music
  if (systems->stateManager.currentScreen < SCREEN_FIRST_LEVEL){
    systems->audioManager.playingNow = GetMusic(&systems->resourceManager, MUSIC_ID_MENU);
    UpdateMusicStream(*systems->audioManager.playingNow);
  }
  else{
    StopMusicStream(*GetMusic(&systems->resourceManager, MUSIC_ID_MENU));
  }

  if (systems->stateManager.currentScreen == SCREEN_FIRST_LEVEL || systems->stateManager.currentScreen == SCREEN_SECOND_LEVEL){
    EntityManager* ecs = &systems->entityManager;
    
    // Listener for spatial audio
    Vector3 listenerPos = {0};
    Vector3 listenerRight = {1, 0, 0};
    bool hasListener = false;

    for (int i = 0; i < ecs->numEntities; i++) {
        if (ecs->componentMasks[i] & COMPONENT_PLAYER_CONTROL) {
            Camera* cam = ecs->playerControlComponents[i].camera;
            listenerPos = cam->position;
             
            Vector3 forward = Vector3Normalize(Vector3Subtract(cam->target, cam->position));
            listenerRight = Vector3Normalize(Vector3CrossProduct(forward, (Vector3){0,1,0}));
            
            hasListener = true;
            break;
        }
    }

    for (int i = 0; i < es->eventCounter; i++){
      Event event = es->eventQueue[i]; 

      if (event.type == EVENT_WEAPON_FIRED){

        WeaponType type = event.data.weaponFired.weapon;
        AssetSoundID sID = GetSoundForWeapon(type);

        if (sID < SOUND_ID_COUNT){

          Sound sfx = *(GetSound(&systems->resourceManager, sID));
          Vector3 soundPos = event.data.weaponFired.position;

          float finalVolume = systems->configManager.audioVolume;
          float finalPan = 0.5f;
          
          // Spatial Audio
          if (hasListener){
            float dist = Vector3Distance(listenerPos, soundPos);
            float hearingDistance = 200.0f;

            if (dist > 0.1f){
              // Linear formula for distance
              float attenuation = 1.0f - (dist/hearingDistance);
              if (attenuation < 0.0f) attenuation = 0;
              finalVolume *= attenuation;
            }
            // G.A for stereo sound
            Vector3 dirToSound = Vector3Normalize(Vector3Subtract(soundPos, listenerPos));
            float dotRight = Vector3DotProduct(dirToSound, listenerRight);
            
            // Conversion between us(-1 for left and 1 for right, and raylib: 0 for left 1 to right);
            finalPan = (dotRight + 1.0f) / 2.0f;
          }
          
          // Make all alterations for the sound
          SetSoundVolume(sfx, finalVolume);
          SetSoundPan(sfx, finalPan);         

          float pitch = 0.95f + ((float)GetRandomValue(-5, 5)/100.0f);
          SetSoundPitch(sfx, pitch);

          SetSoundVolume(sfx, systems->configManager.audioVolume);

          if (finalVolume >= 0.1f) PlaySound(sfx);

        }
      }

    }
  }
}

void ShutdownAudioManager(){
    CloseAudioDevice();     // Close audio device (music streaming is automatically stopped)
}


static AssetSoundID GetSoundForWeapon(WeaponType weaponType) {
  switch (weaponType) {
    case WEAPON_PULSE_LASER:   return SOUND_ID_PULSE_LASER_FIRING;
    default: return SOUND_ID_COUNT;
  }
}



