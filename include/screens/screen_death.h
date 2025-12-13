#ifndef SCREEN_DEATH_H
#define SCREEN_DEATH_H

#include <raylib.h>
#include <stdbool.h>



struct Systems;

// Death screen data
typedef struct {
    Rectangle restartButtonBounds;  // Restart button bounds
    bool isButtonHovered;           // Hover state
    float alpha;                    // Fade alpha
    const char *gameOverText;       // "GAME OVER" text
    const char *restartText;        // Restart button text
    int fontSize;                   // Title font size
} DeathData;

// Funções de ciclo de vida da tela
void DeathInit(struct Systems *systems, DeathData *data);
void UpdateDeathScreen(struct Systems *systems, DeathData *data);
void DrawDeathScreen(struct Systems *systems, DeathData *data);
void DeathUnload(struct Systems *systems, DeathData *data);

#endif
