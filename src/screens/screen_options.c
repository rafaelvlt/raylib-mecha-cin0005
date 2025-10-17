#include <raylib.h>
#include "utility.h"


/************************************
TO-DO:                              *
-Add a option to change resolution  *
-Add a audio slider                 *
-Maybe add an background, 3d Pref   *
*************************************/

void InitOptionsScreen(struct Systems* systems, OptionsData* data)
{
    data->optionsFont = GetFont(&(systems->resourceManager), FONT_ID_CAPTURE_IT);
    data->selectedOption[0] = 0, data->selectedOption[1] = 0;
    data->rangeOptions[0] = 2;
    data->rangeOptions[1] = 3;
    data->rangeOptions[2] = 2;
    data->rangeOptions[3] = 100;
    data->rangeOptions[4] = 1; //number of options in each line
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
        if (data->selectedOption[0] > 0){
            if (data->selectedOption[0] == OPTION_AUDIO_VOLUME+1) {
                data->selectedOption[0] -= 2;
            }
            else {data->selectedOption[0]--;}
        }
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        if (data->selectedOption[0] < OPTION_LINE_COUNT){
            if (data->selectedOption[0] == OPTION_AUDIO_VOLUME-1) {
                data->selectedOption[0] += 2;
            }
            else {data->selectedOption[0]++;}
        }
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
        RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
}

void DrawOptionsScreen(struct Systems* systems, OptionsData* data)
{
    // ------------------- Parameters -------------------- (Should be moved to another function)
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Font Size
    const float titleFontSize = screenHeight * 0.12f; 
    const float optionFontSize = screenHeight * 0.05f; 

    // Vertical Spacing
    const float titleY = screenHeight * 0.1f;        
    const float optionsStartY = screenHeight * 0.35f;   
    const float lineHeight = optionFontSize * 1.5f;  
    const float volumeTextY = optionsStartY + (lineHeight * 3);

    //Highlight
    const float highlightWidth = screenWidth * 0.6f;   
    const float highlightX = (screenWidth - highlightWidth) / 2.0f;

    // Options Strings
    const char* langText = "Language: EN / PT";
    const char* resText = "Resolution: 1280x720 / 1600x900 / 1920x1080";
    const char* fullText = "Fullscreen: ON / OFF";
    const char* volumeText = "Master Volume";
    const char* backText = "Back to Menu";

    // ---------------- Draw -------------------

    // Screen Title
    DrawText("Options", screenWidth/2 - MeasureText("Options", (int)titleFontSize)/2, (int)titleY, (int)titleFontSize, WHITE);


    // Options Text
    DrawText(langText, screenWidth/2 - MeasureText(langText, (int)optionFontSize)/2, (int)(optionsStartY + (lineHeight * 0)), (int)optionFontSize, WHITE);
    DrawText(resText, screenWidth/2 - MeasureText(resText, (int)optionFontSize)/2, (int)(optionsStartY + (lineHeight * 1)), (int)optionFontSize, WHITE);
    DrawText(fullText, screenWidth/2 - MeasureText(fullText, (int)optionFontSize)/2, (int)(optionsStartY + (lineHeight * 2)), (int)optionFontSize, WHITE);
    DrawText(volumeText, screenWidth/2 - MeasureText(volumeText, (int)optionFontSize)/2, (int)volumeTextY, (int)optionFontSize, WHITE);
    DrawText(backText, screenWidth/2 - MeasureText(backText, (int)optionFontSize)/2, (int)(optionsStartY + (lineHeight * 5)), (int)optionFontSize, WHITE);

    // Slider
    const float sliderWidth = highlightWidth * 0.5f;
    const float sliderY = volumeTextY * 1.05 + optionFontSize; 
    const Vector2 sliderStart = { (screenWidth - sliderWidth) / 2.0f, sliderY };
    const Vector2 sliderEnd = { sliderStart.x + sliderWidth, sliderY };

    DrawLineEx(sliderStart, sliderEnd, 5, DARKGRAY);
    DrawCircleV((Vector2){sliderStart.x + (systems->configManager.audioVolume * sliderWidth), sliderStart.y}, 15.0f, WHITE);


    // Selected option highlight
    DrawRectangleLines(
        (int)highlightX, 
        (int)(optionsStartY + (data->selectedOption[0] * lineHeight) - (optionFontSize * 0.2f)), 
        (int)highlightWidth, 
        (int)(optionFontSize * 1.2f),
        RED);
}

void DestroyOptionsScreen(struct Systems* systems, OptionsData* data)
{

}
