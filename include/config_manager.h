#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <raylib.h>

struct Systems;

/*******************
 WORK IN PROGRESS  * 
 *******************/

// Enum for configuration IDs (add more as needed)
typedef enum {
  CONFIG_ID_AUDIO_VOLUME = 0,
  CONFIG_ID_SCREEN_RESOLUTION,
  CONFIG_ID_FULLSCREEN,
  CONFIG_ID_COUNT
} ConfigID;


typedef enum Language {
  LANGUAGE_EN = 0,
  LANGUAGE_PT_BR
} Language;

typedef enum ScreenResolution {
  RESOLUTION_1280x720 = 0,
  RESOLUTION_1600x900,
  RESOLUTION_1920x1080
} ScreenResolution;

// Key Mappings, maybe will be set a option to configure all of them
typedef struct{
  // Movement
  KeyboardKey KeyMoveForward;
  KeyboardKey KeyMoveBackward;
  KeyboardKey KeyTurnLeft;
  KeyboardKey KeyTurnRight;

  // Movement Helper Buttons
  KeyboardKey KeyCenterTorsoToLegs;
  KeyboardKey KeyCenterLegsToTorso;

  // Weapon Groups
  KeyboardKey KeyWeaponGroups[5];

  // Firing
  MouseButton KeyShoot;
  MouseButton KeyZoom;
  KeyboardKey KeyLockTarget;

  // System
  KeyboardKey KeyShowMap;
  KeyboardKey KeyPause;
} InputSystem;


// Struct to hold configuration values
typedef struct {
  Language language; //0 = EN, 1 = PT-BR
  float audioVolume;

  InputSystem KeyMap;
  //Screen
  Vector2 screenResolution;
  bool fullscreen;
} ConfigManager;

void InitConfigManager(struct Systems* systems);
void UpdateConfigManager(struct Systems* systems);
void ShutdownConfigManager();

float GetAudioVolume(ConfigManager* configManager);

// Input System;

//Fullscreen
Vector2 GetScreenResolution(ConfigManager* configManager);
bool GetFullscreen(ConfigManager* configManager);

#endif
