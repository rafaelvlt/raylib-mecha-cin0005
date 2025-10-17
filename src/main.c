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

    // Systems initialization(MUST BE IN THIS ORDER)
    struct Systems systems = {0};
    InitConfigManager(&systems);
    InitAudioManager(&systems);
    InitResourceManager(&systems.resourceManager);
    InitStateManager(&systems, SCREEN_TITLE);

    SetTargetFPS(60);

    // --- Game Loop ---
    systems.shouldExit = false;
    while (!systems.shouldExit) {
        systems.shouldExit = WindowShouldClose();
        // Update Phase
        UpdateStateManager(&systems);
        UpdateAudioManager(&systems);
        // Drawing Phase
        BeginDrawing();
            ClearBackground(BLACK);
            DrawStateManager(&systems);
        EndDrawing();
    }

    //Shutdown struct Systems
    ShutdownAudioManager();
    ShutdownStateManager(&systems);
    ClearEntityManager(&systems.entityManager);
    ShutdownResourceManager(&systems.resourceManager);

    CloseWindow();
    return 0;
}
