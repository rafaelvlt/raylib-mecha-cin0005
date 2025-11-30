#include <raylib.h>
#include "systems.h"
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
    DrawText("Loadout", systems->configManager.screenResolution.x/2 - MeasureText("Loadout", 100)/2, systems->configManager.screenResolution.y/2, 100, WHITE);
    DrawText("Press Enter to go to Menu", systems->configManager.screenResolution.x/2 - MeasureText("Press Enter to go to Menu", 30)/2, systems->configManager.screenResolution.y/2 + 100, 30, WHITE);

}

void DestroyLoadoutScreen(struct Systems* systems, LoadoutData* data)
{

}
