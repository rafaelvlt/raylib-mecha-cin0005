#ifndef SCREEN_FIRST_LEVEL_H
#define SCREEN_FIRST_LEVEL_H
#include <raylib.h>

struct Systems;

typedef struct {
  Camera3D camera;
  bool levelFinished;
  float finishTimer;
  float dropTimer;
  bool hasLanded;
  bool canControl;
} FirstLevelData;

void InitFirstLevelScreen(struct Systems* systems, FirstLevelData* data);
void UpdateFirstLevelScreen(struct Systems* systems, FirstLevelData* data);
void DrawFirstLevelScreen(struct Systems* systems, FirstLevelData* data);
void DestroyFirstLevelScreen(struct Systems* systems, FirstLevelData* data);
#endif
