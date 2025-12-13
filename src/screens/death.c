#include <raylib.h>
#include "systems.h"
#include "state_manager.h"
#include "screens/screen_death.h"
#include <stdio.h>

// Screen constants
#define FADE_SPEED 2.0f
#define GAME_OVER_TEXT_SIZE 80
#define RESTART_TEXT_SIZE 40
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 70

// --------------------------------------------------------------------------------------------------
// Initialization
// --------------------------------------------------------------------------------------------------
void DeathInit(struct Systems *systems, DeathData *data) {
    (void)systems;
    data->gameOverText = "GAME OVER";
    data->restartText = "RESTART";
    data->fontSize = GAME_OVER_TEXT_SIZE;

    data->isButtonHovered = false;
    data->alpha = 0.0f;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    data->restartButtonBounds = (Rectangle){
        (float)screenW / 2 - BUTTON_WIDTH / 2,
        (float)screenH / 2 + 50,
        BUTTON_WIDTH,
        BUTTON_HEIGHT
    };

    EnableCursor();
    TraceLog(LOG_INFO, "SCREEN_DEATH: Initialized");
}

// --------------------------------------------------------------------------------------------------
// Update
// --------------------------------------------------------------------------------------------------
void UpdateDeathScreen(struct Systems *systems, DeathData *data) {
    // Fade-in effect
    if (data->alpha < 1.0f) {
        data->alpha += GetFrameTime() * FADE_SPEED;
        if (data->alpha > 1.0f) data->alpha = 1.0f;
    }

    // Keep button centered if resolution changes
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    data->restartButtonBounds.x = (float)screenW / 2 - BUTTON_WIDTH / 2;
    data->restartButtonBounds.y = (float)screenH / 2 + 50;

    // Button logic
    if (data->alpha >= 1.0f) {
        // Check hover
        data->isButtonHovered = CheckCollisionPointRec(GetMousePosition(), data->restartButtonBounds);

        if (data->isButtonHovered) {
            // Check click
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Restart on click
                TraceLog(LOG_INFO, "SCREEN_DEATH: Restart clicked.");
                RequestScreenChange(systems, SCREEN_SECOND_LEVEL);
            }
        }
        
        // Exit death screen with ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            TraceLog(LOG_INFO, "SCREEN_DEATH: ESC pressed. Returning to main menu.");
            RequestScreenChange(systems, SCREEN_MAIN_MENU);
        }
    }
    
}

// --------------------------------------------------------------------------------------------------
// Draw
// --------------------------------------------------------------------------------------------------
void DrawDeathScreen(struct Systems *systems, DeathData *data) {
    (void)systems;
    // Draw background
    ClearBackground(BLACK);

    Color primaryColor = Fade(RED, data->alpha);
    Color secondaryColor = Fade(YELLOW, data->alpha);
    Color buttonColor = Fade(data->isButtonHovered ? GREEN : DARKGREEN, data->alpha);
    
    int screenW = GetScreenWidth();
    
    // Draw game over text
    int textWidth = MeasureText(data->gameOverText, GAME_OVER_TEXT_SIZE);
    DrawText(data->gameOverText, 
             screenW / 2 - textWidth / 2, 
             GetScreenHeight() / 2 - GAME_OVER_TEXT_SIZE, 
             GAME_OVER_TEXT_SIZE, 
             primaryColor);
             
    // Draw restart button
    Rectangle button = data->restartButtonBounds;
    
    // Draw button background
    DrawRectangleRounded(button, 0.5f, 8, buttonColor);
    DrawRectangleRoundedLines(button, 0.5f, 8, Fade(LIME, data->alpha));

    // Draw button text
    int restartTextWidth = MeasureText(data->restartText, RESTART_TEXT_SIZE);
    DrawText(data->restartText, 
             (int)(button.x + button.width / 2 - restartTextWidth / 2), 
             (int)(button.y + button.height / 2 - RESTART_TEXT_SIZE / 2), 
             RESTART_TEXT_SIZE, 
             Fade(WHITE, data->alpha));

    // Draw secondary hint
    const char *hintText = "Press ESC to leave.";
    int hintWidth = MeasureText(hintText, 20);
    DrawText(hintText, 
             screenW / 2 - hintWidth / 2, 
             GetScreenHeight() - 50, 
             20, 
             secondaryColor);
}

// --------------------------------------------------------------------------------------------------
// Unload
// --------------------------------------------------------------------------------------------------
void DeathUnload(struct Systems *systems, DeathData *data) {
    (void)systems;
    (void)data;
    TraceLog(LOG_INFO, "SCREEN_DEATH: Unloaded.");
}
