#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

struct Systems;

#include <raylib.h>

/*******************
 WORK IN PROGRESS  * 
 *******************/


typedef struct{
    Music* playingNow;
} AudioManager;

void InitAudioManager(struct Systems* systems);
void UpdateAudioManager(struct Systems* systems);

#endif
