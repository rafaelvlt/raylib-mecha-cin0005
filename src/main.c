#include "raylib.h"
#include "raymath.h"
#include "ecs_systems.h"       
#include "ecs_entitymanager.h" 
#include "states.h"

int main(void) {
    // --- Window Initialization ---
    const int screenWidth = 1280;
    const int screenHeight = 720;
    InitWindow(screenWidth, screenHeight, "Mecha Game");

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

    // TITLE/MENU Variables
    int framesCounter = 0;
    
    // --- ECS Initialization ---
    EntityManager entityManager;
    InitEntityManager(&entityManager);

    DisableCursor();
    SetTargetFPS(60);

    // Test variables for the Menu Orbit Target
    // An Asset Manager will be made for this in the future.
    Mesh playerMesh = GenMeshCube(4.0f, 6.0f, 4.0f);
    Model playerModel = LoadModelFromMesh(playerMesh);

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
                UpdateMenuScreen(&gameScreen, &camera);
            } break;
            case FIRST_LEVEL:
            {
                // TODO
                if (IsKeyPressed(KEY_ENTER))
                {
                    gameScreen = CREDITS_SCREEN;
                }
            } break;
            case CREDITS_SCREEN:
            {
                // TODO

                // Press enter to return to TITLE screen
                if (IsKeyPressed(KEY_ENTER))
                {
                    gameScreen = MENU;
                    InitTitleScreen(&framesCounter);
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
                    DrawMenuScreen(&camera, &entityManager, framesCounter);
                } break;
                case FIRST_LEVEL:
                {
                    // TODO
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
    
    CloseWindow();
    return 0;
}

