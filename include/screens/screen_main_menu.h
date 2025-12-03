#ifndef SCREEN_MAIN_MENU_H
#define SCREEN_MAIN_MENU_H

#include "raylib.h"

struct Systems;

typedef enum MenuButton{
    BUTTON_NONE = 0,
    BUTTON_START_GAME,
    BUTTON_LOADOUT,
    BUTTON_OPTIONS,
    BUTTON_CREDITS,
    BUTTON_EXIT,
    BUTTON_COUNT
} MenuButton;

typedef struct {
    Camera3D camera;
    //Buttons
    MenuButton buttonHovered;
    MenuButton buttonPressed;
    Color buttonNormalColor;
    Color buttonHoverColor;
    Rectangle buttonRects[BUTTON_COUNT];
    Vector2 mousePos;
    Font* buttonsFont;
    float fontSize;
    float fontSpacing;
    //Assets
    Model* mechaModelPtr;
    RenderTexture* splitScreenMenuPtr;
    RenderTexture* splitScreenMechaPtr;
    Music* menuMusicPtr;
} MainMenuData;


void InitMainMenuScreen(struct Systems* systems, MainMenuData* data);
void UpdateMainMenuScreen(struct Systems* systems, MainMenuData* data);
void DrawMainMenuScreen(struct Systems* systems, MainMenuData* data);
void DestroyMainMenuScreen(struct Systems* systems, MainMenuData* data);

#endif
