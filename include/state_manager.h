#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

struct Systems;

#include <raylib.h>
#include "screens/screen_title.h"
#include "screens/screen_main_menu.h"
#include "screens/screen_loadout.h"
#include "screens/screen_options.h"
#include "screens/screen_credits.h"
#include "screens/screen_first_level.h"
#include "screens/screen_second_level.h"


// Enum for all Game States
typedef enum GameScreen {
    SCREEN_NONE = 0,
    SCREEN_TITLE,
    SCREEN_MAIN_MENU,
    SCREEN_LOADOUT,
    SCREEN_OPTIONS,
    SCREEN_CREDITS,
    SCREEN_FIRST_LEVEL,
    SCREEN_SECOND_LEVEL,
} GameScreen;

typedef union {
    TitleData title;
    MainMenuData mainMenu;
    LoadoutData loadout;
    OptionsData options;
    CreditsData credits;
    FirstLevelData firstLevel;
    SecondLevelData secondLevel;
} ScreenData;

typedef struct {
    GameScreen currentScreen;
    ScreenData data;
    GameScreen nextScreen;
} StateManager;


void InitStateManager(struct Systems* systems, GameScreen initialScreen);
void UpdateStateManager(struct Systems* systems);
void DrawStateManager(struct Systems* systems);
void ShutdownStateManager(struct Systems* systems);
void RequestScreenChange(struct Systems* systems, GameScreen nextScreen);

#endif
