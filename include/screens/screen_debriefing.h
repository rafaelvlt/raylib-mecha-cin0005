#ifndef SCREEN_DEBRIEFING_H
#define SCREEN_DEBRIEFING_H
#include <raylib.h>

struct Systems;

typedef struct {
  Camera camera;
  float timer;
  float lastStepTime; 
  bool audioStarted;
} DebriefingData;

void InitDebriefingScreen(struct Systems* systems, DebriefingData* data);
void UpdateDebriefingScreen(struct Systems* systems, DebriefingData* data);
void DrawDebriefingScreen(struct Systems* systems, DebriefingData* data);
void DestroyDebriefingScreen(struct Systems* systems, DebriefingData* data);
#endif
