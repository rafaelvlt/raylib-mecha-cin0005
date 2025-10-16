#ifndef SCREEN_FIRST_LEVEL_H
#define SCREEN_FIRST_LEVEL_H
#include <raylib.h>


struct Systems;


typedef struct {
    //TBD
} FirstLevelData;

void InitFirstLevelScreen(struct Systems* systems, FirstLevelData* data);
void UpdateFirstLevelScreen(struct Systems* systems, FirstLevelData* data);
void DrawFirstLevelScreen(struct Systems* systems, FirstLevelData* data);
void DestroyFirstLevelScreen(struct Systems* systems, FirstLevelData* data);

#endif
