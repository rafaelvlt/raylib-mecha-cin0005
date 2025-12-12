#ifndef SCREEN_OPTIONS_H
#define SCREEN_OPTIONS_H
#include <raylib.h>
#include "config_manager.h"
struct Systems;

typedef enum options {
    OPTION_LANGUAGE = 0,
    OPTION_SCREEN_RESOLUTION,
    OPTION_FULLSCREEN,
    OPTION_MUSIC_VOLUME,
    OPTION_SOUND_VOLUME,
    OPTION_BACK_TO_MENU,
    OPTION_LINE_COUNT
} Option;

typedef struct OptionsData {
    int selectedOption; // line
    int selectedOptions[OPTION_LINE_COUNT-1]; // options index
    int rangeOptions[OPTION_LINE_COUNT-1]; //number of options in each line
    int timer[5]; //contador para verificar se está pressionado
    Font* optionsFont;
} OptionsData;


void InitOptionsScreen(struct Systems* systems, OptionsData* data);
void UpdateOptionsScreen(struct Systems* systems, OptionsData* data);
void DrawOptionsScreen(struct Systems* systems, OptionsData* data);
void DestroyOptionsScreen(struct Systems* systems, OptionsData* data);

#endif
