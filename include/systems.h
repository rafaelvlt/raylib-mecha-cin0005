#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <raylib.h>
#include <stdlib.h>
#include "resource_manager.h"
#include "state_manager.h"
#include "ecs/ecs_components.h"
#include "ecs/ecs_entitymanager.h"
#include "ecs/ecs_systems.h"
#include "audio_manager.h"
#include "config_manager.h"

// Holds all managers and global systems, is passed around alot as a pointer to have a "global" system that any function can acess
struct Systems{
    // Managers
    EntityManager entityManager;
    StateManager stateManager;
    ResourceManager resourceManager;
    AudioManager audioManager;
    ConfigManager configManager;
    //TBD Below
    //EventSystem eventSystem;
    //LanguageSystem languageSystem;

    float delta_time;
    bool shouldExit;
};


#endif
