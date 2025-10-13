#include <raylib.h>
#include "utility.h"

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>


void InitMainMenuScreen(struct Systems* systems, MainMenuData* data)
{
    //Menu Splitscreen
    data->splitScreenMenuPtr = GetRenderTexture(&(systems->resourceManager), RENDERTEXTURE_ID_SPLITSCREEN_MENU);
    if (data->splitScreenMenuPtr == NULL)
    {
        TraceLog(LOG_FATAL, "Failed to load Main Menu splitscreen menu.");
        exit(1);
    }
    data->splitScreenMechaPtr = GetRenderTexture(&(systems->resourceManager), RENDERTEXTURE_ID_SPLITSCREEN_MECHA);
    if (data->splitScreenMenuPtr == NULL)
    {
        TraceLog(LOG_FATAL, "Failed to load Main Menu splitscreen mecha.");
        exit(1);
    }


    // Creates the Mecha Entity 
    ClearEntityManager(&(systems->entityManager));

    Entity player = CreateEntity(&(systems->entityManager));

    data->mechaModelPtr = GetModel(&(systems->resourceManager), MODEL_ID_MENU);
    if (data->mechaModelPtr == NULL)
    {
        TraceLog(LOG_FATAL, "Failed to load Main Menu Mecha Model.");
        exit(1);
    }

    AddTransformComponent(&(systems->entityManager), player, (Vector3){ 0.0f, 0.0f, 0.0f });
    AddRenderComponent(&(systems->entityManager), player, data->mechaModelPtr, GREENISH_BLACK);

    data->camera.position = (Vector3){ 8.0f, 7.0f, 10.0f };
    data->camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    data->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    data->camera.fovy = 60.0f;
    data->camera.projection = CAMERA_PERSPECTIVE;
    
    data->buttonPressed = BUTTON_NONE;

    EnableCursor();
}

void UpdateMainMenuScreen(struct Systems* systems, MainMenuData* data)
{

    UpdateCamera(&(data->camera), CAMERA_ORBITAL);

    // Changing State Logic
    if (data->buttonPressed == BUTTON_START_GAME)
    {
        data->buttonPressed = BUTTON_NONE;
        RequestScreenChange(systems, SCREEN_FIRST_LEVEL);
    }
    if (data->buttonPressed == BUTTON_CREDITS)
    {
        data->buttonPressed = BUTTON_NONE;
        RequestScreenChange(systems, SCREEN_CREDITS);
    }
}

void DrawMainMenuScreen(struct Systems* systems, MainMenuData* data)
{
    // Source Rect to be drawn over at the end 
    Rectangle splitScreenRect = { 0.0f, 0.0f, (float)data->splitScreenMechaPtr->texture.width, (float)-data->splitScreenMechaPtr->texture.height};
    //-------- 3D Background -----------
    BeginTextureMode(*(data->splitScreenMechaPtr));
    ClearBackground(BLACK);
        BeginMode3D(data->camera);
            //PROVISORY CODE Draw model wires to be a neon outline before drawing
            DrawModelWires(*(systems->entityManager.renderComponents->model), systems->entityManager.transformComponents->position, 1.0f, GREEN);
            //====================================================================
            RenderSystem_Draw(&(systems->entityManager));
            DrawGrid(15, 0.45f);
        EndMode3D();
    EndTextureMode();
    
    //------- 2D GUI ----------
    BeginTextureMode(*(data->splitScreenMenuPtr));
    ClearBackground(BLACK);
    
    // ------- Menu Panel Specs ----------
    const float panelWidth = SCREEN_WIDTH / 3.0f;
    const float panelHeight = SCREEN_HEIGHT * 0.75f;
    const float panelX = SCREEN_WIDTH * 0.045f;
    const float panelY = (SCREEN_HEIGHT - panelHeight) / 1.3f;

    // ------- Draw Menu Panel ----------
    Rectangle menuPanel = {panelX, panelY, panelWidth, panelHeight};
    DrawRectangleRec(menuPanel, Fade(BLACK, 0.75f));
    DrawRectangleLinesEx(menuPanel, 4, GREEN);
    GuiGroupBox(menuPanel, "");

    //------- Buttons Specs ----------
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
    
    // Checks if the button is pressed and draw them too
    if (GuiButton(startButtonRect, "START GAME")) data->buttonPressed = BUTTON_START_GAME;
    if (GuiButton(loadoutButtonRect, "LOADOUT"))  data->buttonPressed = BUTTON_LOADOUT;
    if (GuiButton(optionsButtonRect, "OPTIONS"))  data->buttonPressed = BUTTON_OPTIONS;
    if (GuiButton(creditsButtonRect, "CREDITS"))  data->buttonPressed = BUTTON_CREDITS;
    if (GuiButton(exitButtonRect, "EXIT"))        data->buttonPressed = BUTTON_EXIT;
    
    EndTextureMode();

    // -------- Draw both screens ----------
    ClearBackground(BLACK);
    DrawTextureRec(data->splitScreenMenuPtr->texture, splitScreenRect, (Vector2){0, 0}, WHITE);
    DrawTextureRec(data->splitScreenMechaPtr->texture, splitScreenRect, (Vector2){SCREEN_WIDTH/2.0f, 0 }, WHITE);
    // ------- Title Specs + Draw ----------
    const float titleX = SCREEN_WIDTH/2 - MeasureText(GAME_TITLE, 60)/2;
    const float titleY = 25;
    const float titleFontSize = 60;
    DrawText(GAME_TITLE, titleX, titleY, titleFontSize, WHITE);


}

void DestroyMainMenuScreen(struct Systems* systems, MainMenuData* data)
{

}
