#include <raylib.h>
#include "utility.h"
#include "config_manager.h"

/************************************
TO-DO:                              *
-Add a option to change resolution  *
-Add a audio slider                 *
-Maybe add an background, 3d Pref   *
*************************************/

void InitOptionsScreen(struct Systems* systems, OptionsData* data)
{
    data->creditsFont = GetFont(&(systems->resourceManager), FONT_ID_CAPTURE_IT);
    data->selectedOption[] = {0,0};
    data->rangeOptions[] = {2,3,2,100,1}; //number of options in each line
}

void optionSelect(struct Systems* systems, int selectedOption[]){
    switch (selectedOption[0]){
        case OPTION_LANGUAGE:
            //change language
            systems->configManager.language = selectedOption[1]; //toggle between 0 and 1
            break;
        case OPTION_SCREEN_RESOLUTION:
            //change resolution
            systems->configManager.screenResolution = (Vector2){1280 + (selectedOption[1]*320), 720 + (selectedOption[1]*180)};
            break;
        case OPTION_FULLSCREEN:
            //fullscreen toggle
            systems->configManager.fullscreen = selectedOption[1] == 1;
            break;
        case OPTION_AUDIO_VOLUME:
            //change audio volume
            systems->configManager.audioVolume = selectedOption[1]/100.0f; //0 to 1
            break;
        case OPTION_BACK_TO_MENU:
            RequestScreenChange(systems, SCREEN_MAIN_MENU);
            break;
        default:
            break;
        }
}

void UpdateOptionsScreen(struct Systems* systems, OptionsData* data)
{
    if (IsKeyPressed(KEY_UP))
    {
        if (data->selectedOption[0] > 0)
            data->selectedOption[0]--;
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        if (data->selectedOption[0] < OPTION_LINES - 1)
            data->selectedOption[0]++;
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        if (data->selectedOption[1] > 0)
            data->selectedOption[1]--;
    }
    if (IsKeyPressed(KEY_RIGHT))
    {
        if (data->selectedOption[1] < data->rangeOptions[data->selectedOption[0]] - 1)
            data->selectedOption[1]++;
    }
    if (IsKeyPressed(KEY_ENTER)){
        optionSelect(systems, data->selectedOption);
    }
}

void DrawOptionsScreen(struct Systems* systems, OptionsData* data)
{
    DrawText("Options", SCREEN_WIDTH/2 - MeasureText("Options", 100)/2, SCREEN_HEIGHT/2, 100, WHITE);
    DrawText("language: EN / PT", SCREEN_WIDTH/2 - MeasureText("language: EN / PT", 30)/2, SCREEN_HEIGHT/2 + 100, 30, WHITE);
    DrawText("resolution: 1280x720 / 1600x900 / 1920x1080", SCREEN_WIDTH/2 - MeasureText("resolution: 1280x720 / 1600x900 / 1920x1080", 30)/2, SCREEN_HEIGHT/2 + 130, 30, WHITE);
    DrawText("fullscreen: ON / OFF", SCREEN_WIDTH/2 - MeasureText("fullscreen: ON / OFF", 30)/2, SCREEN_HEIGHT/2 + 160, 30, WHITE);
    DrawText("audio volume: %d", (int)(systems->configManager.audioVolume * 100), SCREEN_WIDTH/2 - MeasureText("audio volume: 000",  30)/2, SCREEN_HEIGHT/2 + 190, 30, WHITE);
    DrawText("back to Menu", SCREEN_WIDTH/2 - MeasureText("back to Menu", 30)/2, SCREEN_HEIGHT/2 + 220, 30, WHITE);
    //highlight selected option
    DrawRectangleLines(SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 + 100 + (data->selectedOption[0] * 30), 400, 30, RED);
}

void DestroyOptionsScreen(struct Systems* systems, OptionsData* data)
{
    // Unload fonts
    UnloadFont(*data->creditsFont);
}
