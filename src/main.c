#include <raylib.h>
#include <raymath.h>
#include "utility.h"
#include "state_manager.h"
#include "resource_manager.h"
#include "ecs/ecs_components.h"
#include "ecs/ecs_entitymanager.h"
#include "ecs/ecs_systems.h"


int main(void) {
    // --- Window Initialization ---
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    InitAudioDevice();
    
    struct Systems systems = {0};
    InitResourceManager(&systems.resourceManager);
    InitStateManager(&systems, SCREEN_TITLE);

    SetTargetFPS(60);

    //Title + Menu Music
    Music* MainMenuMusicPtr = GetMusic(&(systems.resourceManager), MUSIC_ID_MENU);
    MainMenuMusicPtr->looping = true;
    // --- Game Loop ---
    systems.shouldExit = false;
    while (!systems.shouldExit) {
        systems.shouldExit = WindowShouldClose();
        // Update Phase
        UpdateMusicStream(*MainMenuMusicPtr);      // Update music buffer with new stream data
        UpdateStateManager(&systems);

        // Drawing Phase
        BeginDrawing();
            ClearBackground(BLACK);
            DrawStateManager(&systems);
        EndDrawing();
    }

    //Shutdown struct Systems
    ShutdownStateManager(&systems);
    ClearEntityManager(&systems.entityManager);
    ShutdownResourceManager(&systems.resourceManager);

    CloseAudioDevice();     // Close audio device (music streaming is automatically stopped)
    CloseWindow();
    return 0;
}
