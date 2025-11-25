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
    ClearBackground(BLACK);
    DrawText("Second Level", SCREEN_WIDTH/2 - MeasureText("Second Level", 100)/2, SCREEN_HEIGHT/2, 100, WHITE);
    DrawText("Press Enter to go to Menu", SCREEN_WIDTH/2 - MeasureText("Press Enter to go to Menu", 30)/2, SCREEN_HEIGHT/2 + 100, 30, WHITE);

}

void DestroySecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{

}
