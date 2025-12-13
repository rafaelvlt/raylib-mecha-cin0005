#ifndef SCREEN_MISSION_BRIEFING_H
#define SCREEN_MISSION_BRIEFING_H
#include <raylib.h>

struct Systems;

typedef struct {
  Camera camera;
  float timer;
  float lastStepTime; 
  bool audioStarted;
} MissionBriefingData;

void InitMissionBriefingScreen(struct Systems* systems, MissionBriefingData* data);
void UpdateMissionBriefingScreen(struct Systems* systems, MissionBriefingData* data);
void DrawMissionBriefingScreen(struct Systems* systems, MissionBriefingData* data);
void DestroyMissionBriefingScreen(struct Systems* systems, MissionBriefingData* data);
#endif
