#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#define OPTION_LINES 5

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

// Struct to hold configuration values
typedef struct {
    int language; //0 = EN, 1 = PT-BR
    float audioVolume;
    Vector2 screenResolution;
    bool fullscreen;
} ConfigManager;

typedef struct OptionsData {
    Font* creditsFont;
    int selectedOption[2]; // [line, option]
    int rangeOptions[OPTION_LINES]; //number of options in each line
} OptionsData;

typedef enum options {
    OPTION_LANGUAGE = 0,
    OPTION_SCREEN_RESOLUTION,
    OPTION_FULLSCREEN,
    OPTION_AUDIO_VOLUME,
    OPTION_BACK_TO_MENU
} Option;

typedef enum Language {
    LANGUAGE_EN = 0,
    LANGUAGE_PT_BR
} Language;

typedef enum ScreenResolution {
    RESOLUTION_1280x720 = 0,
    RESOLUTION_1600x900,
    RESOLUTION_1920x1080
} ScreenResolution;

void InitConfigManager(struct Systems* systems);
void UpdateConfigManager(struct Systems* systems);
void ShutdownConfigManager();

float GetAudioVolume(ConfigManager* configManager);
Vector2 GetScreenResolution(ConfigManager* configManager);
bool GetFullscreen(ConfigManager* configManager);

#endif