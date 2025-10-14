#ifndef UTILITY_H_11102025
#define UTILITY_H_11102025

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#include <raylib.h>
#include <stdlib.h>
#include "resource_manager.h"
#include "state_manager.h"
#include "ecs/ecs_components.h"
#include "ecs/ecs_entitymanager.h"
#include "ecs/ecs_systems.h"
#include "audio_manager.h"

#define GAME_TITLE "MECHA GAME"

//Cores
#define GREENISH_BLACK  CLITERAL(Color){ 0, 25, 0, 255 }


// Holds all managers, is passed around alot as a pointer to have a "global" system that any function can acess
struct Systems{
    EntityManager entityManager;
    StateManager stateManager;
    ResourceManager resourceManager;
    AudioManager audioManager;
    //TBD Below
    //EventSystem eventSystem
    
    bool shouldExit;
};


#endif
