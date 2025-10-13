#include <raylib.h>
#include "utility.h"

void InitCreditsScreen(struct Systems* systems, CreditsData* data)
{
    
}

void UpdateCreditsScreen(struct Systems* systems, CreditsData* data)
{
    if (IsKeyPressed(KEY_ENTER))
    {
         RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
}

void DrawCreditsScreen(struct Systems* systems, CreditsData* data)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    DrawText("Credits Screen", GetScreenWidth()/2 - MeasureText("Credits Screen", 40)/2, GetScreenHeight()/4, 40, WHITE);
    DrawText("Press ENTER to go back to the SCREEN_MAIN_MENU", GetScreenWidth()/2 - MeasureText("Press ENTER to go back to the SCREEN_MAIN_MENU", 20)/2, GetScreenHeight()/2, 20, GRAY);
    DrawText("Press ESC to QUIT", GetScreenWidth()/2 - MeasureText("Press ESC to QUIT", 20)/2, GetScreenHeight()/2 + 30, 20, GRAY);
}

void DestroyCreditsScreen(struct Systems* systems, CreditsData* data)
{

}
