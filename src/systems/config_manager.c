#include <raylib.h>
#include "config_manager.h"
#include "utility.h"

// Default configuration values
#define DEFAULT_AUDIO_VOLUME 0.5f
#define DEFAULT_SCREEN_RESOLUTION (Vector2){1280, 720}
#define DEFAULT_FULLSCREEN false

void InitConfigManager(struct Systems* systems) {
    systems->configManager.audioVolume = DEFAULT_AUDIO_VOLUME;
    systems->configManager.screenResolution = DEFAULT_SCREEN_RESOLUTION;
    systems->configManager.fullscreen = DEFAULT_FULLSCREEN;
}

void UpdateConfigManager(struct Systems* systems) {
    //update config from user input (expand as needed)
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
