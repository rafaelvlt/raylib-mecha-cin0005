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
    bool lang = systems->configManager.language;
    const char* loadoutText = lang?"Loadout":"Equipamento";
    const char* goMenuText = lang?"Press Enter to go to Menu":"Pressione ENTER para ir ao Menu";

    DrawText(loadoutText, systems->configManager.screenResolution.x/2 - MeasureText(loadoutText, 100)/2, systems->configManager.screenResolution.y/2, 100, WHITE);
    DrawText(goMenuText, systems->configManager.screenResolution.x/2 - MeasureText(goMenuText, 30)/2, systems->configManager.screenResolution.y/2 + 100, 30, WHITE);

}

void DestroyLoadoutScreen(struct Systems* systems, LoadoutData* data)
{

}
