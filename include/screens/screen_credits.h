#ifndef SCREEN_CREDITS_H
#define SCREEN_CREDITS_H
#include <raylib.h>

struct Systems;

typedef struct{

} CreditsData;

void InitCreditsScreen(struct Systems* systems, CreditsData* data);
void UpdateCreditsScreen(struct Systems* systems, CreditsData* data);
void DrawCreditsScreen(struct Systems* systems, CreditsData* data);
void DestroyCreditsScreen(struct Systems* systems, CreditsData* data);

#endif
