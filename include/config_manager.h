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

// Struct to hold configuration values
typedef struct {
    Language language; //0 = EN, 1 = PT-BR
    float audioVolume;
    Vector2 screenResolution;
    bool fullscreen;
} ConfigManager;

void InitConfigManager(struct Systems* systems);
void UpdateConfigManager(struct Systems* systems);
void ShutdownConfigManager();

float GetAudioVolume(ConfigManager* configManager);
Vector2 GetScreenResolution(ConfigManager* configManager);
bool GetFullscreen(ConfigManager* configManager);

#endif
