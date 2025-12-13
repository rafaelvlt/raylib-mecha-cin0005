#ifndef SCREEN_END_GAME_H
#define SCREEN_END_GAME_H

#include <raylib.h>

struct Systems;

typedef struct {
    float timer;
    float duration;
    const char* message;
} EndGameData;

void InitEndGameScreen(struct Systems* systems, EndGameData* data);
void UpdateEndGameScreen(struct Systems* systems, EndGameData* data);
void DrawEndGameScreen(struct Systems* systems, EndGameData* data);
void DestroyEndGameScreen(struct Systems* systems, EndGameData* data);

#endif
