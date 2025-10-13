#include <raylib.h>
#include <raymath.h>
#include "utility.h"
#include "ecs_systems.h"       
#include "ecs_entitymanager.h" 
#include "states.h"

int main(void) {
    // --- Window Initialization ---
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);

    // Camera Initialization (Same as Raylib Examples)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f }; 
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // --- MENU -------
    // State Machine Definition
    GameScreen gameScreen = TITLE;
    MenuButton menuButtonSelected = NONE;
    // TITLE/MENU Variables
    int framesCounter = 0;
    
    // --- ECS Initialization ---
    EntityManager entityManager;
    InitEntityManager(&entityManager);

    DisableCursor();
    SetTargetFPS(60);

    // Test variables for the Menu Orbit Target
    // An Asset Manager will be made for this in the future.
    Model playerModel = LoadModel("resources\\models\\player\\mechafullmenu.obj");
    //--------- Split Screen(MENU) -----------
    RenderTexture splitScreen[2]; 
    splitScreen[0] = LoadRenderTexture(SCREEN_WIDTH/2, SCREEN_HEIGHT);
    splitScreen[1] = LoadRenderTexture(SCREEN_WIDTH/2, SCREEN_HEIGHT);
    // --- Game Loop ---
    while (!WindowShouldClose()) {
        // Delta Time (Attribute to no give a warning to the compiler about not being used)
        [[maybe_unused]] float deltaTime = GetFrameTime();

        // Update Phase
         switch (gameScreen)
        {
            case TITLE:
            {
                framesCounter++;    // Count frames

                // Wait for 4 seconds (240 frames) before jumping to MENU screen
                if (framesCounter > 240)
                {
                    gameScreen = MENU;
                    InitMenuScreen(&camera, &entityManager, &playerModel);
                }
            } break;
            case MENU:
            {
                UpdateMenuScreen(&gameScreen, &camera, &menuButtonSelected);
            } break;
            case FIRST_LEVEL:
            {
                // TODO
                if (IsKeyPressed(KEY_ENTER))
                {
                    gameScreen = MENU;
                }
            } break;
            case CREDITS_SCREEN:
            {
                // TODO

                // Press enter to return to MENU screen
                if (IsKeyPressed(KEY_ENTER))
                {
                    gameScreen = MENU;
                }
            } break;
            default: break;
        }

        // Drawing Phase
        BeginDrawing();
            ClearBackground(BLACK);
            switch (gameScreen)
            {
                case TITLE:
                {
                    DrawTitleScreen(framesCounter);
                } break;
                case MENU:
                {
                    DrawMenuScreen(&camera, &entityManager, &menuButtonSelected, splitScreen, framesCounter);
                } break;
                case FIRST_LEVEL:
                {
                    //TODO

                    // Dummy text for testing
                    DrawText("FIRST LEVEL", SCREEN_WIDTH/2 - MeasureText(GAME_TITLE, 100)/2, SCREEN_HEIGHT/2, 100, WHITE);
                    DrawText("Press Enter to go to Menu", SCREEN_WIDTH/2 - MeasureText("Press Enter to go to Menu", 30)/2, SCREEN_HEIGHT/2 + 100, 30, WHITE);

                } break;
                case CREDITS_SCREEN:
                {
                    DrawCreditsScreen();
                } break;
                default: break;
            }
        EndDrawing();
    }

    // ---  Cleaning Memory ---
    UnloadModel(playerModel);
    UnloadRenderTexture(splitScreen[0]);
    UnloadRenderTexture(splitScreen[1]);
    CloseWindow();
    return 0;
}

