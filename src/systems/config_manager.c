#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include "config_manager.h"
#include "systems.h"

#define CONFIG_FILE "config.ini"

static void SetDefaultConfig(struct Systems* systems);
static void LoadConfigFile(struct Systems* systems);
static void SaveConfigFile(struct Systems* systems);

void InitConfigManager(struct Systems* systems) {
  LoadConfigFile(systems);
}


void UpdateconfigManagerManager(struct Systems* systems) {
  //update configManager from user input (expand as needed)
  // o codigo vai ficar aqui

}

void ShutdownConfigManager() {
  // No dynamic memory to free for now
}

float GetAudioVolume(ConfigManager* configManager) {
  return configManager->audioVolume;
}

Vector2 GetScreenResolution(ConfigManager* configManager) {
  return configManager->screenResolution;
}

bool GetFullscreen(ConfigManager* configManager) {
  return configManager->fullscreen;
}

int GetLanguage(ConfigManager* configManager) {
  return configManager->language;
}

static void SetDefaultConfig(struct Systems* systems){
  // [VIDEO]
  systems->configManager.screenResolution.x = 1280;
  systems->configManager.screenResolution.y = 720;
  systems->configManager.fullscreen = false; // 0

  // [AUDIO]
  systems->configManager.audioVolume = 0.50f;

  // [PLAYER INPUT]
  systems->configManager.KeyMap.KeyMoveForward  = KEY_W; // 87
  systems->configManager.KeyMap.KeyMoveBackward = KEY_S; // 83
  systems->configManager.KeyMap.KeyTurnLeft     = KEY_A; // 65
  systems->configManager.KeyMap.KeyTurnRight    = KEY_D; // 68
  systems->configManager.KeyMap.KeyCenterTorsoToLegs = KEY_C; // 67
  systems->configManager.KeyMap.KeyCenterLegsToTorso = KEY_F; // 70
  systems->configManager.KeyMap.KeyShoot = MOUSE_BUTTON_LEFT;  // 0
  systems->configManager.KeyMap.KeyZoom  = MOUSE_BUTTON_RIGHT; // 1
  systems->configManager.KeyMap.KeyLockTarget = KEY_TAB;         // 82
  systems->configManager.KeyMap.KeyWeaponGroups[0] = KEY_ONE;   // 49
  systems->configManager.KeyMap.KeyWeaponGroups[1] = KEY_TWO;   // 50
  systems->configManager.KeyMap.KeyWeaponGroups[2] = KEY_THREE; // 51
  systems->configManager.KeyMap.KeyWeaponGroups[3] = KEY_FOUR;  // 52
  systems->configManager.KeyMap.KeyWeaponGroups[4] = KEY_FIVE;  // 53
  systems->configManager.KeyMap.KeyShowMap = KEY_M; // 77
  systems->configManager.KeyMap.KeyPause   = KEY_ENTER;
  SaveConfigFile(systems);
}

static void SaveConfigFile(struct Systems* systems){
  FILE* file = fopen(CONFIG_FILE, "w"); 

  if (file == NULL) {
    TraceLog(LOG_WARNING, "Configuration file was not saved");
    return;
  }

  fprintf(file, "[VIDEO]\n");
  fprintf(file, "WIDTH=%d\n", (int)systems->configManager.screenResolution.x);
  fprintf(file, "HEIGHT=%d\n", (int)systems->configManager.screenResolution.y);
  fprintf(file, "FULLSCREEN=%d\n", systems->configManager.fullscreen ? 1 : 0);

  fprintf(file, "\n[AUDIO]\n");
  fprintf(file, "VOLUME=%.2f\n", systems->configManager.audioVolume);

  fprintf(file, "\n[PLAYER INPUT]\n");
  fprintf(file, "KEY_MOVE_FORWARD=%d\n", systems->configManager.KeyMap.KeyMoveForward);
  fprintf(file, "KEY_MOVE_BACKWARD=%d\n", systems->configManager.KeyMap.KeyMoveBackward);
  fprintf(file, "KEY_TURN_LEFT=%d\n", systems->configManager.KeyMap.KeyTurnLeft);
  fprintf(file, "KEY_TURN_RIGHT=%d\n", systems->configManager.KeyMap.KeyTurnRight);

  fprintf(file, "KEY_CENTER_TORSO=%d\n", systems->configManager.KeyMap.KeyCenterTorsoToLegs);
  fprintf(file, "KEY_CENTER_LEGS=%d\n", systems->configManager.KeyMap.KeyCenterLegsToTorso);

  fprintf(file, "BTN_SHOOT=%d\n", systems->configManager.KeyMap.KeyShoot);
  fprintf(file, "BTN_ZOOM=%d\n", systems->configManager.KeyMap.KeyZoom);
  fprintf(file, "KEY_LOCK_TARGET=%d\n", systems->configManager.KeyMap.KeyLockTarget);

  for(int i = 0; i < 6; i++) {
    fprintf(file, "KEY_WEAPON_%d=%d\n", i + 1, systems->configManager.KeyMap.KeyWeaponGroups[i]);
  }

  fprintf(file, "KEY_SHOW_MAP=%d\n", systems->configManager.KeyMap.KeyShowMap);
  fprintf(file, "KEY_PAUSE=%d\n", systems->configManager.KeyMap.KeyPause);

  fclose(file);
  TraceLog(LOG_INFO, "Configuration file saved succesfuly!");
}

static void LoadConfigFile(struct Systems* systems){
  FILE* configFile = fopen(CONFIG_FILE, "r"); 
  if (configFile == NULL){
    TraceLog(LOG_WARNING, "Configuration file not found. Created a default one"); 
    SetDefaultConfig(systems);
    return ;
  }

  TraceLog(LOG_INFO, "Loading configuration file...");

  char buff[256];
  char key[128];
  char value[128];

  while(fgets(buff, sizeof(buff), configFile)){
    if(buff[0] == '[' || buff[0] == '#' || buff[0] == ' '){
      ; // Do nothing
    }
    else{
      if(sscanf(buff, "%[^=]=%s", key, value) == 2){
        if (strcmp(key, "WIDTH") == 0) systems->configManager.screenResolution.x = atoi(value);
        else if(strcmp(key, "HEIGHT") == 0) systems->configManager.screenResolution.y = atoi(value);
        else if(strcmp(key, "VOLUME") == 0) systems->configManager.audioVolume = atof(value);
        else if(strcmp(key, "FULLSCREEN") == 0) systems->configManager.fullscreen = atoi(value);

          //Input
        else if (strcmp(key, "KEY_MOVE_FORWARD") == 0)  systems->configManager.KeyMap.KeyMoveForward = atoi(value);
        else if (strcmp(key, "KEY_MOVE_BACKWARD") == 0) systems->configManager.KeyMap.KeyMoveBackward = atoi(value);
        else if (strcmp(key, "KEY_TURN_LEFT") == 0)     systems->configManager.KeyMap.KeyTurnLeft = atoi(value);
        else if (strcmp(key, "KEY_TURN_RIGHT") == 0)    systems->configManager.KeyMap.KeyTurnRight = atoi(value);

        else if (strcmp(key, "KEY_CENTER_TORSO") == 0) systems->configManager.KeyMap.KeyCenterTorsoToLegs = atoi(value);
        else if (strcmp(key, "KEY_CENTER_LEGS") == 0)  systems->configManager.KeyMap.KeyCenterLegsToTorso = atoi(value);

        else if (strcmp(key, "BTN_SHOOT") == 0) systems->configManager.KeyMap.KeyShoot = atoi(value);
        else if (strcmp(key, "BTN_ZOOM") == 0)  systems->configManager.KeyMap.KeyZoom = atoi(value);
        else if (strcmp(key, "KEY_LOCK_TARGET") == 0) systems->configManager.KeyMap.KeyLockTarget = atoi(value);

        else if (strcmp(key, "KEY_WEAPON_1") == 0) systems->configManager.KeyMap.KeyWeaponGroups[0] = atoi(value);
        else if (strcmp(key, "KEY_WEAPON_2") == 0) systems->configManager.KeyMap.KeyWeaponGroups[1] = atoi(value);
        else if (strcmp(key, "KEY_WEAPON_3") == 0) systems->configManager.KeyMap.KeyWeaponGroups[2] = atoi(value);
        else if (strcmp(key, "KEY_WEAPON_4") == 0) systems->configManager.KeyMap.KeyWeaponGroups[3] = atoi(value);
        else if (strcmp(key, "KEY_WEAPON_5") == 0) systems->configManager.KeyMap.KeyWeaponGroups[4] = atoi(value);

        else if (strcmp(key, "KEY_SHOW_MAP") == 0) systems->configManager.KeyMap.KeyShowMap = atoi(value);
        else if (strcmp(key, "KEY_PAUSE") == 0)    systems->configManager.KeyMap.KeyPause = atoi(value);
      }
    }
  }
}
