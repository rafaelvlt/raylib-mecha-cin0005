#include <raylib.h>
#include "systems.h"

void InitSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{

}

void UpdateSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
  if (IsKeyPressed(KEY_ENTER))
  {
    RequestScreenChange(systems, SCREEN_MAIN_MENU);
  }
}

void DrawSecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{
  Vector2 resolution = GetScreenResolution(&systems->configManager);
  DrawText("Second Level", (int)(resolution.x/2) - MeasureText("Second Level", 100)/2, (int)(resolution.y/2), 100, WHITE);
  DrawText("Press Enter to go to Menu", (int)(resolution.x/2) - MeasureText("Press Enter to go to Menu", 30)/2, (int)(resolution.y/2) + 100, 30, WHITE);

}

void DestroySecondLevelScreen(struct Systems* systems, SecondLevelData* data)
{

}
