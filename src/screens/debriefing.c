#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <math.h>
#include "resource_manager.h"
#include "screens/screen_debriefing.h"
#include "systems.h"

// Positions
#define START_Z         50.0f  
#define STOP_Z          10.0f  
#define MECHA_Z         -10.0f  
#define CENTER_OFFSET_X  1.7f //Because the Hangar and the mecha are off center 

// Timers
#define DELAY_AUDIO_START  2.5f   
#define TIME_WALK_END     28.0f  
#define TIME_FADE_START   30.0f 
#define TIME_CUTSCENE_END 33.0f

// Animation Parameter
#define BOB_FREQ  1.5f   
#define BOB_AMP   0.08f 
#define HANGAR_SCALE 0.35f
#define MECHA_SCALE 0.9f

#define STEP_VOL  0.5f   // Volume

void InitDebriefingScreen(struct Systems* systems, DebriefingData* data) {
  // Reset
  data->timer = 0.0f;
  data->audioStarted = false;

  // Human POV
  data->camera.position = (Vector3){ CENTER_OFFSET_X, 1.8f, START_Z }; 
  data->camera.target   = (Vector3){ CENTER_OFFSET_X, 5.0f, MECHA_Z }; 
  data->camera.up       = (Vector3){ 0.0f, 1.0f, 0.0f };
  data->camera.fovy     = 27.5f; 
  data->camera.projection = CAMERA_PERSPECTIVE;

  DisableCursor();
}

void UpdateDebriefingScreen(struct Systems* systems, DebriefingData* data) {
  //Timers
  float dt = systems->delta_time;
  float tNow = data->timer;
  float tPrev = data->timer - dt;

  // Bobbing Angle
  float angleNow  = tNow  * BOB_FREQ * 2.0f * PI;
  float anglePrev = tPrev * BOB_FREQ * 2.0f * PI;

  float currentBob = sinf(angleNow);
  float prevBob    = sinf(anglePrev);

  data->timer += dt;

  // Audio Logic
  if (data->timer >= DELAY_AUDIO_START && !data->audioStarted) {
    PlayMusicStream(systems->resourceManager.musics[MUSIC_ID_DEBRIEFING]);
    data->audioStarted = true;
  }

  if (data->audioStarted) {
    UpdateMusicStream(systems->resourceManager.musics[MUSIC_ID_DEBRIEFING]);
  }

  // Movement logic
  if (data->timer < TIME_WALK_END) {
    // Speed for reaching at the right time
    float distTotal = START_Z - STOP_Z;
    float walkDuration = TIME_WALK_END; 
    float speed = distTotal / walkDuration; 

    data->camera.position.z -= speed * dt;
    data->camera.position.y = 1.8f + currentBob * BOB_AMP;

    // Footsteps sound logic with sine waves
    if (prevBob > -0.95f && currentBob <= -0.95f) { 
      Sound* step = GetSound(&systems->resourceManager, SOUND_ID_HUMAN_FOOTSTEP); 
      SetSoundPitch(*step, 0.9f + ((float)GetRandomValue(-5, 5) / 100.0f)); 
      SetSoundVolume(*step, STEP_VOL); 
      PlaySound(*step);
    }
  }
  else {
    // Stop in the front of the Mecha and looks ups
    data->camera.target.y = Lerp(data->camera.target.y, 12.5f, dt * 1.5f);
  }

  // Audio stop
  float audioLen = GetMusicTimeLength(systems->resourceManager.musics[MUSIC_ID_DEBRIEFING]);
  float audioPos = GetMusicTimePlayed(systems->resourceManager.musics[MUSIC_ID_DEBRIEFING]);

  // End of cutscene logic (Audio & Screen change) 
  if (audioPos >= audioLen - 0.2f || data->timer >= TIME_CUTSCENE_END) {
    StopMusicStream(systems->resourceManager.musics[MUSIC_ID_DEBRIEFING]);
  }
  if (data->timer > TIME_CUTSCENE_END || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
    StopMusicStream(systems->resourceManager.musics[MUSIC_ID_DEBRIEFING]);
    RequestScreenChange(systems, SCREEN_FIRST_LEVEL);
  }
}

void DrawDebriefingScreen(struct Systems* systems, DebriefingData* data) {
  ClearBackground(BLACK); 

  BeginMode3D(data->camera);
  Model* hangar = GetModel(&systems->resourceManager, MODEL_ID_HANGAR_ENV);
  if (hangar) {
    DrawModel(*hangar, (Vector3){0, 0, 0}, HANGAR_SCALE, WHITE);
  }
  Model* mecha = GetModel(&systems->resourceManager, MODEL_ID_DEBRIEFING);
  if (mecha) {
    Vector3 mechaPos = {CENTER_OFFSET_X, 0.0f, MECHA_Z}; 
    DrawModel(*mecha, mechaPos, MECHA_SCALE, WHITE);
  }

  EndMode3D();

  // Transition Fade to Black Logic
  if (data->timer >= TIME_FADE_START) {
    float fadeDuration = TIME_CUTSCENE_END - TIME_FADE_START;
    float alpha = (data->timer - TIME_FADE_START) / fadeDuration;

    if (alpha > 1.0f) alpha = 1.0f;

    // Draw Black Rect on the screen, decreasing transparency with time
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, alpha));
  }

  // Hide Skip option during fade
  if (data->timer < TIME_FADE_START) {
    Vector2 resolution = GetScreenResolution(&systems->configManager);
    Vector2 textPos = {20, resolution.y - 40};
    int textSpacing = 5;
    int textSize = 20;
    DrawTextEx(*GetFont(&systems->resourceManager, FONT_ID_CAPTURE_IT), "PRESS ENTER TO SKIP", textPos, textSize, textSpacing, WHITE);
  }
}

void DestroyDebriefingScreen(struct Systems* systems, DebriefingData* data) {}
