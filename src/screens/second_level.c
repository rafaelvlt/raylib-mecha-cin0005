#include <raylib.h>
#include "state_manager.h"
#include "systems.h"
#include "utility.h"

void InitSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
    
}

void UpdateSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
    if (IsKeyPressed(KEY_ENTER))
    {
         RequestScreenChange(systems, SCREEN_SECOND_LEVEL);
    }
}

void DrawSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
    bool lang = systems->configManager.language;
    const char* secondLevelText = lang?"Segundo Nível":"Second Level";
    const char* goMenuText = lang?"Pressione ENTER para ir ao Menu":"Press Enter to go to Menu";
    ClearBackground(BLACK);
    DrawText(secondLevelText, systems->configManager.screenResolution.x/2 - MeasureText(secondLevelText, 100)/2, systems->configManager.screenResolution.y/2, 100, WHITE);
    DrawText(goMenuText, systems->configManager.screenResolution.x/2 - MeasureText(goMenuText, 30)/2, systems->configManager.screenResolution.y/2 + 100, 30, WHITE);

}

void DestroySecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{

}
