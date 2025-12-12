#ifndef SYSTEMS_H
#define SYSTEMS_H

#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "resource_manager.h"
#include "event_manager.h"
#include "state_manager.h"
#include "audio_manager.h"
#include "config_manager.h"
#include "ecs/components.h"
#include "ecs/entitymanager.h"
#include "ecs/systems.h"

// Holds all managers and global systems, is passed around alot as a pointer to have a "global" system that any function can acess
struct Systems{
  // Managers
  EntityManager entityManager;
  StateManager stateManager;
  ResourceManager resourceManager;
  AudioManager audioManager;
  ConfigManager configManager;
  EventManager eventManager;
  //TBD Below
  //LanguageSystem languageSystem;

  float delta_time;
  bool shouldExit;
};




#endif
