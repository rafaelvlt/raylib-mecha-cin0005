#include <raylib.h>
#include <raymath.h>
#include "event_manager.h"
#include "systems.h"
#include "utility.h"
#include "state_manager.h"
#include "resource_manager.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"

int main(void) {
    // --- Window Initialization ---

    // Systems initialization(MUST BE IN THIS ORDER)
    struct Systems systems = {0};
    InitConfigManager(&systems);

    InitWindow(systems.configManager.screenResolution.x, systems.configManager.screenResolution.y, GAME_TITLE);

    InitAudioManager(&systems);
    InitResourceManager(&systems);
    InitEventManager(&systems);
    InitStateManager(&systems, SCREEN_TITLE);

    SetTargetFPS(60);

    // --- Game Loop ---
    systems.shouldExit = false;
    while (!systems.shouldExit) {
        systems.delta_time = GetFrameTime(); 
        systems.shouldExit = WindowShouldClose();
        // Update Phase
        UpdateStateManager(&systems);
        UpdateAudioManager(&systems);
        // Drawing Phase
        BeginDrawing();
            ClearBackground(BLACK);
            DrawStateManager(&systems);
        EndDrawing();
        
        ClearEventManager(&systems.eventManager);
    }

    //Shutdown struct Systems
    ShutdownAudioManager();
    ShutdownStateManager(&systems);
    ClearEntityManager(&systems.entityManager);
    ShutdownResourceManager(&systems.resourceManager);

    CloseWindow();
    return 0;
}
