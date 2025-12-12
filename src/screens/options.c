#include <raylib.h>
#include "systems.h"


/************************************
TO-DO:                              *
-Add a option to change resolution  *
-Add a audio slider                 *
-Maybe add an background, 3d Pref   *
*************************************/

void InitOptionsScreen(struct Systems* systems, OptionsData* data)
{
    data->optionsFont = GetFont(&(systems->resourceManager), FONT_ID_CAPTURE_IT);
    data->selectedOption = 0; // line
    data->selectedOptions[OPTION_LANGUAGE] = systems->configManager.language; //language
    data->selectedOptions[OPTION_SCREEN_RESOLUTION] = systems->configManager.screenResolutionIndex; //resolution
    data->selectedOptions[OPTION_FULLSCREEN] = systems->configManager.fullscreen; //fullscreen
    data->selectedOptions[OPTION_MUSIC_VOLUME] = (int)(systems->configManager.musicVolume * 100); //music volume
    data->selectedOptions[OPTION_SOUND_VOLUME] = (int)(systems->configManager.soundVolume * 100); //sound volume

    
    //number of options in each line
    data->rangeOptions[OPTION_LANGUAGE] = 2; // EN, PT-BR
    data->rangeOptions[OPTION_SCREEN_RESOLUTION] = 3; // 1280x720, 1600x900, 1920x1080
    data->rangeOptions[OPTION_FULLSCREEN] = 2; // OFF, ON
    data->rangeOptions[OPTION_MUSIC_VOLUME] = 100; // 0 to 100
    data->rangeOptions[OPTION_SOUND_VOLUME] = 100; // 0 to 100
    data->rangeOptions[OPTION_BACK_TO_MENU] = 1; // Back to menu
}

void optionSelect(struct Systems* systems, OptionsData* data){
    systems->configManager.language = data->selectedOptions[OPTION_LANGUAGE];
    systems->configManager.screenResolutionIndex = data->selectedOptions[OPTION_SCREEN_RESOLUTION];
    Vector2 resolutions[] = {
        {1280, 720},
        {1600, 900},
        {1920, 1080}
    };
    systems->configManager.screenResolution = resolutions[data->selectedOptions[OPTION_SCREEN_RESOLUTION]];
    systems->configManager.fullscreen = data->selectedOptions[OPTION_FULLSCREEN];
    systems->configManager.musicVolume = data->selectedOptions[OPTION_MUSIC_VOLUME] / 100.0f;
    systems->configManager.soundVolume = data->selectedOptions[OPTION_SOUND_VOLUME] / 100.0f;

    if (data->selectedOption == OPTION_BACK_TO_MENU) RequestScreenChange(systems, SCREEN_MAIN_MENU);
    
}

void UpdateOptionsScreen(struct Systems* systems, OptionsData* data)
{
    if (IsKeyPressed(KEY_UP)) if (data->selectedOption > 0) data->selectedOption--;
        
    if (IsKeyPressed(KEY_DOWN))  if (data->selectedOption < OPTION_LINE_COUNT -1) data->selectedOption++;
    
    if (IsKeyPressed(KEY_LEFT)) if (data->selectedOptions[data->selectedOption] > 0) data->selectedOptions[data->selectedOption]--;
    
    if (IsKeyPressed(KEY_RIGHT)) if (data->selectedOptions[data->selectedOption] < data->rangeOptions[data->selectedOption] - 1) data->selectedOptions[data->selectedOption]++;
    
    if (IsKeyPressed(KEY_ENTER)) optionSelect(systems, data);
        
    
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

    //Highlight
    const float highlightWidth = screenWidth * 0.6f;
    const float highlightX = (screenWidth - highlightWidth) / 2.0f;

    // Options Strings
    const bool lang = systems->configManager.language;
    const char* langTitleText = lang?"Idioma":"Language";
    const char* langOptionsText[2] = {"EN", "PT"};
    const char* resTitleText = lang?"Resolução:":"Resolution:";
    const char* resOptionsText[3] = {"1280x720", "1600x900", "1920x1080"};
    const char* fullTitleText = lang?"Tela Cheia:":"Fullscreen:";
    const char* fullOptionsText[2] = {lang?"DESLIGADO":"OFF", lang?"LIGADO":"ON"};
    const char* musicVolumeText = lang?"Volume Musica ":"Music Volume ";
    const char* audioVolumeText = lang?"Volume Audio ":"Audio Volume ";
    const char* backText = lang?"Voltar ao Menu":"Back to Menu";

    const char* langText = TextFormat("%s: %s", langTitleText, langOptionsText[data->selectedOptions[OPTION_LANGUAGE]]);
    const char* resText = TextFormat("%s: %s", resTitleText, resOptionsText[data->selectedOptions[OPTION_SCREEN_RESOLUTION]]);
    const char* fullText = TextFormat("%s: %s", fullTitleText, fullOptionsText[data->selectedOptions[OPTION_FULLSCREEN]]);

    // Sliders
    const float sliderWidth = highlightWidth * 0.5f;
    const float musicSliderY = optionsStartY + (lineHeight * 3) + optionFontSize/2;
    const float soundSliderY = optionsStartY + (lineHeight * 4) + optionFontSize/2;
    //const Vector2 sliderStart = { (screenWidth - sliderWidth + (MeasureText(musicVolumeText, (int)optionFontSize))) / 2.0f, musicSliderY };
    //const Vector2 sliderEnd = { sliderStart.x + sliderWidth, musicSliderY };
    const float sliderStartX = (screenWidth - sliderWidth + (MeasureText(musicVolumeText, (int)optionFontSize))) / 2.0f;
    const float sliderEndX = sliderStartX + sliderWidth;

    // ---------------- Draw -------------------

    // Screen Title
    DrawText("Options", screenWidth/2 - MeasureText("Options", (int)titleFontSize)/2, (int)titleY, (int)titleFontSize, WHITE);


    // Options Text
    DrawText(langText, screenWidth/2 - MeasureText(langText, (int)optionFontSize)/2, (int)(optionsStartY + (lineHeight * 0)), (int)optionFontSize, WHITE);
    DrawText(resText, screenWidth/2 - MeasureText(resText, (int)optionFontSize)/2, (int)(optionsStartY + (lineHeight * 1)), (int)optionFontSize, WHITE);
    DrawText(fullText, screenWidth/2 - MeasureText(fullText, (int)optionFontSize)/2, (int)(optionsStartY + (lineHeight * 2)), (int)optionFontSize, WHITE);
    DrawText(musicVolumeText, screenWidth/2 - (MeasureText(musicVolumeText, (int)optionFontSize) + sliderWidth)/2, (int)(optionsStartY + (lineHeight * 3)), (int)optionFontSize, WHITE);
    DrawText(audioVolumeText, screenWidth/2 - (MeasureText(audioVolumeText, (int)optionFontSize) + sliderWidth)/2, (int)(optionsStartY + (lineHeight * 4)), (int)optionFontSize, WHITE);
    DrawText(backText, screenWidth/2 - MeasureText(backText, (int)optionFontSize)/2, (int)(optionsStartY + (lineHeight * 5)), (int)optionFontSize, WHITE);

    // Volume Sliders
    DrawLineEx((Vector2){sliderStartX, musicSliderY}, (Vector2) {sliderEndX, musicSliderY}, 5, DARKGRAY);
    DrawCircleV((Vector2){sliderStartX + (data->selectedOptions[OPTION_MUSIC_VOLUME] * sliderWidth)/100.0f, musicSliderY}, 15.0f, WHITE);
    DrawLineEx((Vector2){sliderStartX, soundSliderY}, (Vector2) {sliderEndX, soundSliderY}, 5, DARKGRAY);
    DrawCircleV((Vector2){sliderStartX + (data->selectedOptions[OPTION_SOUND_VOLUME] * sliderWidth)/100.0f, soundSliderY}, 15.0f, WHITE);


    // Selected option highlight
    DrawRectangleLines(
        (int)highlightX, 
        (int)(optionsStartY + (data->selectedOption * lineHeight) - (optionFontSize * 0.3f)), 
        (int)highlightWidth, 
        (int)(optionFontSize * 1.3f),
        RED);
}

void DestroyOptionsScreen(struct Systems* systems, OptionsData* data)
{

}
