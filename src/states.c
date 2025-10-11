#include "raylib.h"
#include "states.h"



// Funções de título(Inicialização)
void InitTitleScreen(int* framesCounter)
{
    *framesCounter = 0;
}

void UpdateTitleScreen(GameScreen* Screen)
{
    
}

void DrawTitleScreen(int framesCounter)
{
    const float maxAlpha = 1.0f;
    const int twoSeconds = 120;

    float alphaValue;
    if (framesCounter  <= twoSeconds) alphaValue = 0.01 * framesCounter;
    else if (framesCounter > twoSeconds) alphaValue = maxAlpha - (0.01*(framesCounter - twoSeconds));

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    DrawText("MECHA GAME", GetScreenWidth()/2 - MeasureText("MECHA GAME", 100)/2, GetScreenHeight()/2 - 100, 100, Fade(WHITE, alphaValue));
}

void UnloadTitleScreen(void)
{

}

// Menu Functions
void InitMenuScreen(Camera3D* camera, EntityManager* entityManager, Model* playerModel)
{
    InitEntityManager(entityManager);

    Entity player = CreateEntity(entityManager);
    AddTransformComponent(entityManager, player, (Vector3){ 0.0f, 2.0f, 0.0f });
    AddRenderComponent(entityManager, player, *playerModel, DARKGREEN);

    camera->position = (Vector3){ 10.0f, 3.0f, 10.0f };
    camera->target = (Vector3){ 0.0f, 2.0f, 0.0f }; // Camera Targeting Mecha Position
    camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera->fovy = 60.0f;
    camera->projection = CAMERA_PERSPECTIVE;
    

}

void UpdateMenuScreen(GameScreen* Screen, Camera3D* camera)
{

    UpdateCamera(camera, CAMERA_ORBITAL);
    // Changing State Logic
    if (IsKeyPressed(KEY_ENTER))
    {
        *Screen = FIRST_LEVEL;
    }
    if (IsKeyPressed(KEY_E))
    {
        *Screen = CREDITS_SCREEN;
    }
    if (IsKeyPressed(KEY_ESCAPE))
    {
        *Screen = CREDITS_SCREEN;
    }
}

void DrawMenuScreen(Camera3D* camera, EntityManager* entityManager, int framesCounter)
{
    BeginMode3D(*camera);
        RenderSystem_Draw(entityManager);
    EndMode3D();


    // 2D UI Text
    DrawText("MAIN MENU", GetScreenWidth()/5 - MeasureText("MAIN MENU", 40)/2, GetScreenHeight()/4, 40, WHITE);
    DrawText("Press ENTER to PLAY", GetScreenWidth()/5 - MeasureText("Press ENTER to PLAY", 20)/2, GetScreenHeight()/2, 20, WHITE);
    DrawText("Press E to go to CREDITS", GetScreenWidth()/5 - MeasureText("Press E to go to CREDITS", 20)/2, GetScreenHeight()/2 + 30, 20, WHITE);
    DrawText("Press ESC to QUIT", GetScreenWidth()/5 - MeasureText("Press ESC to QUIT", 20)/2, GetScreenHeight()/2 + 60, 20, WHITE);
}

void UnloadMenuScreen(void)
{
    
}



void InitCreditsScreen(void)
{
    
}

void UpdateCreditsScreen(GameScreen* Screen)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        *Screen = TITLE;
    }
}

void DrawCreditsScreen(void)
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
    DrawText("Credits Screen", GetScreenWidth()/2 - MeasureText("Credits Screen", 40)/2, GetScreenHeight()/4, 40, WHITE);
    DrawText("Press ENTER to go back to the MENU", GetScreenWidth()/2 - MeasureText("Press ENTER to go back to the MENU", 20)/2, GetScreenHeight()/2, 20, GRAY);
    DrawText("Press ESC to QUIT", GetScreenWidth()/2 - MeasureText("Press ESC to QUIT", 20)/2, GetScreenHeight()/2 + 30, 20, GRAY);
}

void UnloadCreditsScreen(void)
{

}
