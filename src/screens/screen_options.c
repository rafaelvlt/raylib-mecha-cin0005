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
    
}

void UpdateOptionsScreen(struct Systems* systems, OptionsData* data)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
}

void DrawOptionsScreen(struct Systems* systems, OptionsData* data)
{
    //Variables for volume
    static Vector2 volumeButtonCenter = {(float)SCREEN_WIDTH / 3.8, (float)SCREEN_HEIGHT / 10 + 300};
    static float volumeButtonRadius = 15.0f;
    static float volumeLevel = 0.5f; 
    //Draw text and buttons
    DrawText("Options", SCREEN_WIDTH/2 - MeasureText("Options", 100)/2, SCREEN_HEIGHT/10, 100, WHITE);
    DrawText("Press Enter to go to Menu", SCREEN_WIDTH/2 - MeasureText("Press Enter to return to Menu", 30)/2, SCREEN_HEIGHT/10 + 100, 30, WHITE);
    //VOLUME
    DrawText("Volume principal", SCREEN_WIDTH/4 - MeasureText("Volume principal", 40)/2, SCREEN_HEIGHT/10 + 250, 30, WHITE);
    DrawLineEx((Vector2){volumeButtonCenter.x - 150, volumeButtonCenter.y}, (Vector2){volumeButtonCenter.x + 50, volumeButtonCenter.y}, 5, DARKGRAY);
    DrawCircleV((Vector2){(volumeLevel, 0.0f, 1.0f, volumeButtonCenter.x - 50, volumeButtonCenter.x + 50), volumeButtonCenter.y}, volumeButtonRadius, WHITE);
    
    DrawText("Tamanho da tela", SCREEN_WIDTH/1.25 - MeasureText("Tamanho da tela", 40)/2, SCREEN_HEIGHT/10 + 250, 30, WHITE);
    DrawText("Idioma", SCREEN_WIDTH/4 - MeasureText("Idioma", 40)/2, SCREEN_HEIGHT/10 + 400, 30, WHITE);

}

void DestroyOptionsScreen(struct Systems* systems, OptionsData* data)
{

}
