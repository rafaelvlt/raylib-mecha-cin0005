#ifndef SCREEN_OPTIONS_H
#define SCREEN_OPTIONS_H
#include <raylib.h>
#include "config_manager.h"
struct Systems;

typedef struct OptionsData {
    Font* creditsFont;
    int selectedOption[2]; // [line, option]
    int rangeOptions[OPTION_LINES-1]; //number of options in each line
} OptionsData;

void InitOptionsScreen(struct Systems* systems, OptionsData* data);
void UpdateOptionsScreen(struct Systems* systems, OptionsData* data);
void DrawOptionsScreen(struct Systems* systems, OptionsData* data);
void DestroyOptionsScreen(struct Systems* systems, OptionsData* data);

#endif
