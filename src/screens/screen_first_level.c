#include <raylib.h>
#include "utility.h"

void InitFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
    
}

void UpdateFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
    if (IsKeyPressed(KEY_ENTER))
    {
         RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
}

void DrawFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{
    DrawText("FIRST LEVEL", SCREEN_WIDTH/2 - MeasureText(GAME_TITLE, 100)/2, SCREEN_HEIGHT/2, 100, WHITE);
    DrawText("Press Enter to go to Menu", SCREEN_WIDTH/2 - MeasureText("Press Enter to go to Menu", 30)/2, SCREEN_HEIGHT/2 + 100, 30, WHITE);

}

void DestroyFirstLevelScreen(struct Systems* systems, FirstLevelData* data)
{

}
