#include <raylib.h>
#include "utility.h"

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
        case SCREEN_CREDITS:
            DestroyCreditsScreen(systems, &stateManager->data.credits);
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
        case SCREEN_CREDITS:
            InitCreditsScreen(systems, &stateManager->data.credits);
            break;
        default:
            break;
    }
}

void InitStateManager(struct Systems* systems, GameScreen initialScreen) {
    // Cleans current screen
    systems->stateManager.currentScreen = SCREEN_NONE;
    // change to the initial
    systems->stateManager.nextScreen = initialScreen;

    SwitchScreen(systems);
}

void UpdateStateManager(struct Systems* systems) {

    // Checks if needs to change screen
    if (systems->stateManager.nextScreen != SCREEN_NONE) {
        SwitchScreen(systems);
    }

    StateManager* stateManager = &systems->stateManager;

    // Calls the update function based on the current screen
    switch (stateManager->currentScreen) {
        case SCREEN_TITLE:
            UpdateTitleScreen(systems, &stateManager->data.title);
            break;
        case SCREEN_MAIN_MENU:
            UpdateMainMenuScreen(systems, &stateManager->data.mainMenu);
            break;
        case SCREEN_CREDITS:
            UpdateCreditsScreen(systems, &stateManager->data.credits);
            break;
        default:
            break;
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
        case SCREEN_CREDITS:
            DrawCreditsScreen(systems, &stateManager->data.credits);
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
    systems->stateManager.nextScreen = nextScreen;
}
