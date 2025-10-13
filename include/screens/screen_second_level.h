#ifndef SCREEN_SECOND_LEVEL_H
#define SCREEN_SECOND_LEVEL_H
#include <raylib.h>

struct Systems;

typedef struct {
    //TBD
} SecondLevelData;

void InitSecondLevelScreen(struct Systems* systems, SecondLevelData* data);
void UpdateSecondLevelScreen(struct Systems* systems, SecondLevelData* data);
void DrawSecondLevelScreen(struct Systems* systems, SecondLevelData* data);
void DestroySecondLevelScreen(struct Systems* systems, SecondLevelData* data);

#endif
