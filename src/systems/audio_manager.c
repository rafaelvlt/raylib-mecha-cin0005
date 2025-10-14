#include <raylib.h>
#include "utility.h"


void InitAudioManager(struct Systems* systems){

    InitAudioDevice();
    systems->audioManager.playingNow = GetMusic(&systems->resourceManager, MUSIC_ID_MENU);
    systems->audioManager.playingNow->looping = true;

}
void UpdateAudioManager(struct Systems* systems){
    // If the current screen is one of the menu screen, stay playing the music
    if (systems->stateManager.currentScreen < SCREEN_FIRST_LEVEL) systems->audioManager.playingNow = GetMusic(&systems->resourceManager, MUSIC_ID_MENU);

    UpdateMusicStream(*systems->audioManager.playingNow);
}

void ShutdownAudioManager(){
    CloseAudioDevice();     // Close audio device (music streaming is automatically stopped)
}
