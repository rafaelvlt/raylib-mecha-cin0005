#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "screens/screen_debriefing.h"
struct Systems;

#include <raylib.h>
#include "screens/screen_title.h"
#include "screens/screen_main_menu.h"
#include "screens/screen_loadout.h"
#include "screens/screen_options.h"
#include "screens/screen_credits.h"
#include "screens/screen_first_level.h"
#include "screens/screen_second_level.h"
#include "screens/screen_death.h"
#include "screens/screen_end_game.h"


// Enum for all Game States
typedef enum GameScreen {
  SCREEN_NONE = 0,
  SCREEN_TITLE,
  SCREEN_MAIN_MENU,
  SCREEN_LOADOUT,
  SCREEN_OPTIONS,
  SCREEN_CREDITS,
  SCREEN_DEBRIEFING,
  SCREEN_FIRST_LEVEL,
  SCREEN_SECOND_LEVEL,
  SCREEN_DEATH,
  SCREEN_END_GAME
} GameScreen;


// Every screen has a data struct associated, all screen functions have a pointer to the struct as an argument
// If you want to add some data that isn't present in any of the Systems, add it to the struct at the include/screen folder
// and initialize it to a value in the screen Init function.
typedef union {
  TitleData title;
  MainMenuData mainMenu;
  LoadoutData loadout;
  OptionsData options;
  CreditsData credits;
  DebriefingData debriefing;
  FirstLevelData firstLevel;
  SecondLevelData secondLevel;
  DeathData death;
  EndGameData endGame;
} ScreenData;

// The state manager hold the union to the screen data, the current state and the next state for switches
// State manager is a component of systems struct as a pointer
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
