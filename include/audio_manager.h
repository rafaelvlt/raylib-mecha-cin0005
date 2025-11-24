#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

struct Systems;

#include <raylib.h>
#include "event_manager.h"


typedef struct{
    Music* playingNow;
} AudioManager;

void InitAudioManager(struct Systems* systems);
void AudioManagerOnEvent(struct Systems* systems, Event event);
void UpdateAudioManager(struct Systems* systems);
void ShutdownAudioManager();
#endif
