#include <raylib.h>
#include "systems.h"
#include "state_manager.h"
#include "screens/screen_end_game.h"

static const float DEFAULT_DURATION = 7.0f;

void InitEndGameScreen(struct Systems* systems, EndGameData* data) {
    *data = (EndGameData){0};
    data->duration = DEFAULT_DURATION;
    data->message = "MISSION COMPLETE";
    DisableCursor();
    PlaySound(systems->resourceManager.sounds[SOUND_ID_END_GAME]);
}

void UpdateEndGameScreen(struct Systems* systems, EndGameData* data) {
    data->timer += systems->delta_time;
    if (data->timer >= data->duration) {
        RequestScreenChange(systems, SCREEN_MAIN_MENU);
    }
}

void DrawEndGameScreen(struct Systems* systems, EndGameData* data) {
    ClearBackground(BLACK);

    const int fontSize = 80;
    int textWidth = MeasureText(data->message, fontSize);
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    int x = (screenW - textWidth) / 2;
    int y = (screenH - fontSize) / 2;
    DrawText(data->message, x, y, fontSize, WHITE);

    const char* hint = "Returning to main menu...";
    int hintSize = 20;
    int hintWidth = MeasureText(hint, hintSize);
    DrawText(hint, (screenW - hintWidth) / 2, y + fontSize + 20, hintSize, GRAY);
}

void DestroyEndGameScreen(struct Systems* systems, EndGameData* data) {
}
