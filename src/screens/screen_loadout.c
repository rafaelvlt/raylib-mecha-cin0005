#include <raylib.h>
#include "utility.h"

void InitLoadoutScreen(struct Systems* systems, LoadoutData* data)
{
    
}

void UpdateLoadoutScreen(struct Systems* systems, LoadoutData* data)
{
    if (IsKeyPressed(KEY_ENTER))
    {
         RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
}

void DrawLoadoutScreen(struct Systems* systems, LoadoutData* data)
{
    DrawText("Loadout", SCREEN_WIDTH/2 - MeasureText("Loadout", 100)/2, SCREEN_HEIGHT/2, 100, WHITE);
    DrawText("Press Enter to go to Menu", SCREEN_WIDTH/2 - MeasureText("Press Enter to go to Menu", 30)/2, SCREEN_HEIGHT/2 + 100, 30, WHITE);

}

void DestroyLoadoutScreen(struct Systems* systems, LoadoutData* data)
{

}
