#ifndef GAME_STATES_H
#define GAME_STATES_H

#include "raylib.h" // Inclui raylib para tipos como Color, Rectangle, etc.

// Enumeração para os diferentes estados do jogo
typedef enum GameScreen {
    TITLE = 0,
    GAMEPLAY,
    ENDING
} GameScreen;

// Variável global para controlar o estado atual do jogo
// É crucial que seja extern para ser declarada aqui e definida em game_states.c
extern GameScreen currentScreen;

// Funções para cada estado
// Inicialização
void InitTitleScreen(void);
void InitGameplayScreen(void);
void InitEndingScreen(void);

// Atualização (lógica)
void UpdateTitleScreen(void);
void UpdateGameplayScreen(void);
void UpdateEndingScreen(void);

// Desenho (renderização)
void DrawTitleScreen(void);
void DrawGameplayScreen(void);
void DrawEndingScreen(void);

// Desinicialização (liberação de recursos)
void UnloadTitleScreen(void);
void UnloadGameplayScreen(void);
void UnloadEndingScreen(void);

#endif // GAME_STATES_H