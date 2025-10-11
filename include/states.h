#ifndef GAME_STATES_H
#define GAME_STATES_H

#include "raylib.h"
#include "ecs_components.h"
#include "ecs_entitymanager.h"
#include "ecs_systems.h"

// Enum for all Game States
typedef enum GameScreen {
    TITLE = 0,
    MENU,
    FIRST_LEVEL,
    SECOND_LEVEL,
    CREDITS_SCREEN
} GameScreen;

// Funções para cada estado
// Inicialização
void InitTitleScreen(int* framesCounter);
void InitMenuScreen(Camera3D* camera, EntityManager* entitymanager, Model* playerModel);
void InitCreditsScreen(void);

// Atualização (lógica)
void UpdateTitleScreen(GameScreen* Screen);
void UpdateMenuScreen(GameScreen* Screen, Camera3D* camera);
void UpdateCreditsScreen(GameScreen* Screen);

// Desenho (renderização)
void DrawTitleScreen(int framesCounter);
void DrawMenuScreen(Camera3D* camera, EntityManager* entityManager, int framesCounter);
void DrawCreditsScreen(void);

// Desinicialização (liberação de recursos)
void UnloadTitleScreen(void);
void UnloadMenuScreen(void);
void UnloadCreditsScreen(void);

#endif
