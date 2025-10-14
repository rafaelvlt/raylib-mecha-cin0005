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
    DrawText("Options", SCREEN_WIDTH/2 - MeasureText("Options", 100)/2, SCREEN_HEIGHT/2, 100, WHITE);
    DrawText("Press Enter to go to Menu", SCREEN_WIDTH/2 - MeasureText("Press Enter to go to Menu", 30)/2, SCREEN_HEIGHT/2 + 100, 30, WHITE);

}

void DestroyOptionsScreen(struct Systems* systems, OptionsData* data)
{

}
