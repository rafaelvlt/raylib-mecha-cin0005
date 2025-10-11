#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "states.h"
#include "utility.h"

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
    DrawText(GAME_TITLE, GetScreenWidth()/2 - MeasureText(GAME_TITLE, 100)/2, GetScreenHeight()/2 - 100, 100, Fade(WHITE, alphaValue));
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

    camera->position = (Vector3){ 5.0f, 3.0f, 10.0f };
    camera->target = (Vector3){ 0.0f, 2.0f, 0.0f }; // Camera Targeting Mecha Position
    camera->up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera->fovy = 60.0f;
    camera->projection = CAMERA_PERSPECTIVE;
    
    EnableCursor();
}

void UpdateMenuScreen(GameScreen* Screen, Camera3D* camera, MenuButton* menuButtonSelected)
{

    UpdateCamera(camera, CAMERA_ORBITAL);
    // Changing State Logic
    if (*menuButtonSelected == START_GAME)
    {
        *menuButtonSelected = NONE;
        *Screen = FIRST_LEVEL;
    }
    if (*menuButtonSelected == CREDITS)
    {
        *menuButtonSelected = NONE;
        *Screen = CREDITS_SCREEN;
    }

}

void DrawMenuScreen(Camera3D* camera, EntityManager* entityManager, MenuButton* menuButtonSelected, int framesCounter)
{
    //-------- 3D Background -----------
    BeginMode3D(*camera);
        DrawGrid(10, 0.45f);
        RenderSystem_Draw(entityManager);
    EndMode3D();


    //------- 2D GUI ----------
    const float titleX = SCREEN_WIDTH/2 - MeasureText(GAME_TITLE, 60)/2;
    const float titleY = 25;
    const float titleFontSize = 60;
    DrawText(GAME_TITLE, titleX, titleY, titleFontSize, WHITE);
    
    
    const float panelWidth = SCREEN_WIDTH / 3.0f;
    const float panelHeight = SCREEN_HEIGHT * 0.75f;
    const float panelX = SCREEN_WIDTH * 0.045f;
    const float panelY = (SCREEN_HEIGHT - panelHeight) / 1.3f;
    Rectangle menuPanel = {panelX, panelY, panelWidth, panelHeight};
    DrawRectangleRec(menuPanel, Fade(BLACK, 0.75f));
    DrawRectangleLinesEx(menuPanel, 4, GREEN);

    
    GuiGroupBox(menuPanel, "");
    int originalTextSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    int originalTextAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 30);           
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    const float buttonWidth = panelWidth * 0.8f;
    const float buttonHeight = 50.0f;
    const float buttonSpacing = 20.0f;
    const float buttonX = menuPanel.x + (menuPanel.width - buttonWidth) / 2.0f;
    Rectangle startButtonRect = { buttonX, menuPanel.y + 60, buttonWidth, buttonHeight };
    Rectangle loadoutButtonRect = { buttonX, startButtonRect.y + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };
    Rectangle optionsButtonRect = { buttonX, loadoutButtonRect.y + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };
    Rectangle creditsButtonRect = { buttonX, optionsButtonRect.y + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };
    Rectangle exitButtonRect = { buttonX, creditsButtonRect.y + buttonHeight + buttonSpacing, buttonWidth, buttonHeight };

    // Logic to
    if (GuiButton(startButtonRect, "START GAME")) *menuButtonSelected = START_GAME;
    if (GuiButton(loadoutButtonRect, "LOADOUT"))  *menuButtonSelected = LOADOUT;
    if (GuiButton(optionsButtonRect, "OPTIONS"))  *menuButtonSelected = OPTIONS;
    if (GuiButton(creditsButtonRect, "CREDITS"))  *menuButtonSelected = CREDITS;
    if (GuiButton(exitButtonRect, "EXIT"))        *menuButtonSelected = EXIT;
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
