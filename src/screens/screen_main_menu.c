#include <raylib.h>
#include "utility.h"



void DrawMenuButton(MainMenuData* data, Rectangle box, const char* text, MenuButton current){
    Color textColor = (data->buttonHovered == current) ? data->buttonHoverColor : data->buttonNormalColor;
    Font font = *data->buttonsFont;
    Vector2 textSize = MeasureTextEx(*data->buttonsFont, text, data->fontSize, data->fontSpacing);
    Vector2 textPosition = {
        box.x + (box.width - textSize.x) / 2,
        box.y + (box.height - textSize.y) / 2
    };

    DrawTextEx(font, text, textPosition, data->fontSize, data->fontSpacing, textColor);

}

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


    //Camera
    data->camera.position = (Vector3){ 8.0f, 7.0f, 5.0f };
    data->camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    data->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    data->camera.fovy = 60.0f;
    data->camera.projection = CAMERA_PERSPECTIVE;
    
    // Menu Buttons
    data->buttonsFont = GetFont(&(systems->resourceManager), FONT_ID_CAPTURE_IT);
    if (data->buttonsFont == NULL){
        TraceLog(LOG_FATAL, "Failed to load Main Menu font.");
        exit(1);
    }
    data->buttonHovered = BUTTON_NONE;
    data->buttonPressed = BUTTON_NONE;
    data->buttonNormalColor = GRAY;
    data->buttonHoverColor = WHITE;
    data->fontSize = 50.0f;
    data->fontSpacing = 2.0f;
    EnableCursor();
    data->mousePos = GetMousePosition();
}

void UpdateMainMenuScreen(struct Systems* systems, MainMenuData* data)
{

    UpdateCamera(&(data->camera), CAMERA_ORBITAL);
    data->mousePos = GetMousePosition();
    
    for (int i = BUTTON_START_GAME; i < BUTTON_COUNT; i++)
    {
        if (CheckCollisionPointRec(data->mousePos, data->buttonRects[i]))
        {
            data->buttonHovered = (MenuButton)i;

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                data->buttonPressed = (MenuButton)i; 
            }

        }
    }

    if (data->buttonPressed == BUTTON_START_GAME)
    {
        data->buttonPressed = BUTTON_NONE;
        RequestScreenChange(systems, SCREEN_FIRST_LEVEL);
    }
    if (data->buttonPressed == BUTTON_LOADOUT)
    {
        data->buttonPressed = BUTTON_NONE;
        RequestScreenChange(systems, SCREEN_LOADOUT);
    }
    if (data->buttonPressed == BUTTON_OPTIONS)
    {
        data->buttonPressed = BUTTON_NONE;
        RequestScreenChange(systems, SCREEN_OPTIONS);
    }
    if (data->buttonPressed == BUTTON_CREDITS)
    {
        data->buttonPressed = BUTTON_NONE;
        RequestScreenChange(systems, SCREEN_CREDITS);
    }
    if (data->buttonPressed == BUTTON_EXIT)
    {
        data->buttonPressed = BUTTON_NONE;
        systems->shouldExit = true;
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
    const float panelMarginTop = GetScreenHeight() * 0.25f; 
    const float panelWidth = GetScreenWidth() * 0.28f;     
    const float panelHeight = GetScreenHeight() * 0.60f;  
    const float panelX = GetScreenWidth() * 0.05f;        
    const float panelY = panelMarginTop;

    // ------- Draw Menu Panel ----------
    Rectangle menuPanel = {panelX, panelY, panelWidth, panelHeight};
    DrawRectangleRec(menuPanel, Fade(BLACK, 0.75f));
    DrawRectangleLinesEx(menuPanel, 4, GREEN);

    //------- Buttons Specs (to scale with Resolution) ----------
    const float contentPaddingX = panelWidth * 0.1f;  
    const float contentPaddingY = panelHeight * 0.05f; 
    const float contentAreaX = panelX + contentPaddingX;
    const float contentAreaY = panelY + contentPaddingY;
    const float contentAreaWidth = panelWidth - (contentPaddingX * 2);
    const float contentAreaHeight = panelHeight - (contentPaddingY * 2);
    const int numButtons = 5;
    const float slotHeight = contentAreaHeight / numButtons;
    const float buttonToSlotRatio = 0.8f; 
    const float singleButtonHeight = slotHeight * buttonToSlotRatio;
    const float verticalPaddingPerSlot = slotHeight * (1.0f - buttonToSlotRatio);
    data->fontSize = singleButtonHeight * 0.6f; 

    for (int i = 0; i < numButtons; i++)
    {
        float buttonY = contentAreaY + (i * slotHeight) + (verticalPaddingPerSlot / 2.0f);
        data->buttonRects[i + 1] = (Rectangle){contentAreaX, buttonY, contentAreaWidth, singleButtonHeight};
    }
    
    // Draw Buttons
    DrawMenuButton(data, data->buttonRects[BUTTON_START_GAME], "START GAME", BUTTON_START_GAME);
    DrawMenuButton(data, data->buttonRects[BUTTON_LOADOUT], "LOADOUT", BUTTON_LOADOUT);
    DrawMenuButton(data, data->buttonRects[BUTTON_OPTIONS], "OPTIONS", BUTTON_OPTIONS);
    DrawMenuButton(data, data->buttonRects[BUTTON_CREDITS], "CREDITS", BUTTON_CREDITS);
    DrawMenuButton(data, data->buttonRects[BUTTON_EXIT], "EXIT", BUTTON_EXIT);
    
    EndTextureMode();

    // -------- Draw both screens ----------
    ClearBackground(BLACK);
    DrawTextureRec(data->splitScreenMenuPtr->texture, splitScreenRect, (Vector2){0, 0}, WHITE);
    DrawTextureRec(data->splitScreenMechaPtr->texture, splitScreenRect, (Vector2){SCREEN_WIDTH/2.0f, 0 }, WHITE);
    // ------- Title Specs + Draw ----------
    Vector2 titlePos;
    titlePos.x = SCREEN_WIDTH/2 - MeasureText(GAME_TITLE, 60)/2;
    titlePos.y = 25;
    const float titleSpacing = 3.0f;
    const float titleFontSize = data->fontSize*1.8f;
    DrawTextEx(*data->buttonsFont, GAME_TITLE, titlePos, titleFontSize, titleSpacing, WHITE);


}

void DestroyMainMenuScreen(struct Systems* systems, MainMenuData* data)
{

}
