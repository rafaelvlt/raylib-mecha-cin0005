#include <raylib.h>
#include "utility.h"

void InitTitleScreen(struct Systems* systems, TitleData* data)
{
    DisableCursor();
    data->framesCounter = 0;
    data->titleFontPtr = GetFont(&(systems->resourceManager), FONT_ID_CAPTURE_IT);

     
    if (data->titleFontPtr == NULL)
    {
        TraceLog(LOG_FATAL, "Failed to load title font");
        exit(1);
    }
    data->titleMusicPtr = GetMusic(&(systems->resourceManager), MUSIC_ID_MENU);
    if (data->titleMusicPtr == NULL)
    {
        TraceLog(LOG_FATAL, "Failed to load title music");
        exit(1);
    }
    PlayMusicStream(*data->titleMusicPtr);
}

void UpdateTitleScreen(struct Systems* systems, TitleData* data)
{
    data->framesCounter++;

    if (data->framesCounter > 240){
        RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
}

void DrawTitleScreen(struct Systems* systems, TitleData* data)
{
    const float maxAlpha = 1.0f;
    const int twoSeconds = 120;
    float alphaValue;

    if (data->framesCounter  <= twoSeconds) alphaValue = 0.01 * data->framesCounter;
    else if (data->framesCounter > twoSeconds) alphaValue = maxAlpha - (0.01*(data->framesCounter - twoSeconds));

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);

    Vector2 textSize = MeasureTextEx(*data->titleFontPtr, GAME_TITLE, 150.0f, 20.0f);
    Vector2 titlePosition;
    titlePosition.x = (GetScreenWidth() / 2.0f) - (textSize.x / 2.0f);
    titlePosition.y = (GetScreenHeight() / 2.0f) - (textSize.y / 2.0f);
    DrawTextEx(*data->titleFontPtr, GAME_TITLE, titlePosition, 150.0f, 20.0f, Fade(WHITE, alphaValue));
}

void DestroyTitleScreen(struct Systems* systems, TitleData* data)
{

}
