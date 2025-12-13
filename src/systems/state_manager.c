#include "state_manager.h"
#include <raylib.h>
#include "systems.h"

/*==========================
See the state_manager.h for how to use guideline
don't change anything here, its already complete
===========================*/

void DestroyCurrentScreen(struct Systems* systems) {
  StateManager* stateManager = &systems->stateManager;
  // Calls the destroy function based on the current screen
  // Destroy functions are TBD and should remove their own data from memory
  switch (stateManager->currentScreen) {
    case SCREEN_TITLE:
      DestroyTitleScreen(systems, &stateManager->data.title);
      break;
    case SCREEN_MAIN_MENU:
      DestroyMainMenuScreen(systems, &stateManager->data.mainMenu);
      break;
    case SCREEN_OPTIONS:
      DestroyOptionsScreen(systems, &stateManager->data.options);
      break;
    case SCREEN_CREDITS:
      DestroyCreditsScreen(systems, &stateManager->data.credits);
      break;
    case SCREEN_DEBRIEFING:
      DestroyDebriefingScreen(systems, &stateManager->data.debriefing);
      break;
    case SCREEN_FIRST_LEVEL:
      DestroyFirstLevelScreen(systems, &stateManager->data.firstLevel);
      break;
    case SCREEN_SECOND_LEVEL:
      DestroySecondLevelScreen(systems, &stateManager->data.secondLevel);
      break;
    case SCREEN_DEATH:
      DeathUnload(systems, &stateManager->data.death);
      break;
    case SCREEN_END_GAME:
      DestroyEndGameScreen(systems, &stateManager->data.endGame);
      break;
    default:
      break;
  }
}

void SwitchScreen(struct Systems* systems) {
  StateManager* stateManager = &systems->stateManager;

  // Cleanup
  DestroyCurrentScreen(systems);

  // Change to the next screen
  stateManager->currentScreen = stateManager->nextScreen;
  stateManager->nextScreen = SCREEN_NONE;

  // Initialize the newScreen
  switch (stateManager->currentScreen) {
    case SCREEN_TITLE:
      InitTitleScreen(systems, &stateManager->data.title);
      break;
    case SCREEN_MAIN_MENU:
      InitMainMenuScreen(systems, &stateManager->data.mainMenu);
      break;
    case SCREEN_OPTIONS:
      InitOptionsScreen(systems, &stateManager->data.options);
      break;
    case SCREEN_CREDITS:
      InitCreditsScreen(systems, &stateManager->data.credits);
      break;
    case SCREEN_DEBRIEFING:
      InitDebriefingScreen(systems, &stateManager->data.debriefing);
      break;
    case SCREEN_FIRST_LEVEL:
      InitFirstLevelScreen(systems, &stateManager->data.firstLevel);
      break;
    case SCREEN_SECOND_LEVEL:
      InitSecondLevelScreen(systems, &stateManager->data.secondLevel);
      break;
    case SCREEN_DEATH:
      DeathInit(systems, &stateManager->data.death);
      break;
    case SCREEN_END_GAME:
      InitEndGameScreen(systems, &stateManager->data.endGame);
      break;
    default:
      break;
  }
}

void InitStateManager(struct Systems* systems, GameScreen initialScreen) {
  // Cleans current screen
  systems->stateManager.currentScreen = SCREEN_NONE;
  // Change to the initial
  systems->stateManager.nextScreen = initialScreen;

  SwitchScreen(systems);
}

void UpdateStateManager(struct Systems* systems) {

  EventManager* ev = &systems->eventManager;
  StateManager* sm = &systems->stateManager;

  // Calls the update function based on the current screen
  switch (sm->currentScreen) {
    case SCREEN_TITLE:
      UpdateTitleScreen(systems, &sm->data.title);
      break;
    case SCREEN_MAIN_MENU:
      UpdateMainMenuScreen(systems, &sm->data.mainMenu);
      break;
    case SCREEN_OPTIONS:
      UpdateOptionsScreen(systems, &sm->data.options);
      break;
    case SCREEN_CREDITS:
      UpdateCreditsScreen(systems, &sm->data.credits);
      break;
    case SCREEN_DEBRIEFING:
      UpdateDebriefingScreen(systems, &sm->data.debriefing);
      break;
    case SCREEN_FIRST_LEVEL:
      UpdateFirstLevelScreen(systems, &sm->data.firstLevel);
      break;
    case SCREEN_SECOND_LEVEL:
      UpdateSecondLevelScreen(systems, &sm->data.secondLevel);
      break;
    case SCREEN_DEATH:
      UpdateDeathScreen(systems, &sm->data.death);
      break;
    case SCREEN_END_GAME:
      UpdateEndGameScreen(systems, &sm->data.endGame);
      break;
    default:
      break;
  }

  // Checks event queue for a screenChange request
  for (int i = 0; i < ev->eventCounter; i++) {
    if (ev->eventQueue[i].type == EVENT_SCREEN_CHANGE) {
      systems->stateManager.nextScreen = ev->eventQueue[i].data.screenChange.nextScreen;
    }
  }

  // Checks if needs to change screen
  if (sm->nextScreen != SCREEN_NONE) {
    SwitchScreen(systems);
  }

}

void DrawStateManager(struct Systems* systems) {
  StateManager* stateManager = &systems->stateManager;
  // Calls the draw function based on the current screen
  switch (stateManager->currentScreen) {
    case SCREEN_TITLE:
      DrawTitleScreen(systems, &stateManager->data.title);
      break;
    case SCREEN_MAIN_MENU:
      DrawMainMenuScreen(systems, &stateManager->data.mainMenu);
      break;
    case SCREEN_OPTIONS:
      DrawOptionsScreen(systems, &stateManager->data.options);
      break;
    case SCREEN_CREDITS:
      DrawCreditsScreen(systems, &stateManager->data.credits);
      break;
    case SCREEN_DEBRIEFING:
      DrawDebriefingScreen(systems, &stateManager->data.debriefing);
      break;
    case SCREEN_FIRST_LEVEL:
      DrawFirstLevelScreen(systems, &stateManager->data.firstLevel);
      break;
    case SCREEN_SECOND_LEVEL:
      DrawSecondLevelScreen(systems, &stateManager->data.secondLevel);
      break;
    case SCREEN_DEATH:
      DrawDeathScreen(systems, &stateManager->data.death);
      break;
    case SCREEN_END_GAME:
      DrawEndGameScreen(systems, &stateManager->data.endGame);
      break;
    default:
      break;
  }
}

void ShutdownStateManager(struct Systems* systems) {
  // Garantee of destroying the final screen at the end of exectuion
  DestroyCurrentScreen(systems);
}

void RequestScreenChange(struct Systems* systems, GameScreen nextScreen) {
  EventData data;
  data.screenChange.nextScreen = nextScreen;

  PushEvent(systems, EVENT_SCREEN_CHANGE, data);
}
