#ifndef SCREEN_TITLE_H
#define SCREEN_TITLE_H

#include <raylib.h>

struct Systems;

typedef struct {
    int framesCounter;
    Font* titleFontPtr;
    Music* titleMusicPtr;
} TitleData;

void InitTitleScreen(struct Systems* systems, TitleData* data);
void UpdateTitleScreen(struct Systems* systems, TitleData* data);
void DrawTitleScreen(struct Systems* systems, TitleData* data);
void DestroyTitleScreen(struct Systems* systems, TitleData* data);
#endif
