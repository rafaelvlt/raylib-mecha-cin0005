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
  Vector2 resolution = GetScreenResolution(&systems->configManager);
  DrawText("Loadout", (int)(resolution.x/2) - MeasureText("Loadout", 100)/2, (int)(resolution.y/2), 100, WHITE);
  DrawText("Press Enter to go to Menu", (int)(resolution.x/2) - MeasureText("Press Enter to go to Menu", 30)/2, (int)(resolution.y/2) + 100, 30, WHITE);

}

void DestroyLoadoutScreen(struct Systems* systems, LoadoutData* data)
{

}
