#ifndef SCREEN_DEATH_H
#define SCREEN_DEATH_H

#include <raylib.h>
#include <stdbool.h>



struct Systems; // Declaração forward para uso nas funções de tela

// Estrutura de dados da Tela de Morte
typedef struct {
    Rectangle restartButtonBounds;  // Limites do botão de reiniciar
    bool isButtonHovered;           // Estado de hover do botão
    float alpha;                    // Valor de transparência para efeito de fade
    const char *gameOverText;       // Texto "GAME OVER"
    const char *restartText;        // Texto do botão
    int fontSize;                   // Tamanho da fonte
} DeathData;

// Funções de ciclo de vida da tela
void DeathInit(DeathData *data);
void DeathUpdate(DeathData *data, struct Systems *systems);
void DrawDeathScreen(const DeathData *data);
void DeathUnload(DeathData *data);

#endif