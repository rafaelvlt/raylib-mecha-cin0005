#include "resource_manager.h"
#include <raylib.h>
#include <raymath.h>
#include "systems.h"
#include "utility.h"

/****************************************************
See the resource_manager.h for how to use guideline *
Only Change the init function to add more assets    *
Don't forget to add the ID to the enum              *
*****************************************************/

// Forward declarations
static void LoadModels(ResourceManager* rm);
static void LoadFonts(ResourceManager* rm);
static void LoadSounds(ResourceManager* rm);
static void LoadMusic(ResourceManager* rm);
static void LoadTextures(ResourceManager* rm);
static void LoadRenderTextures(ResourceManager* rm, Vector2 screenResolution);

void InitResourceManager(struct Systems* systems) {
  ResourceManager* rm = &systems->resourceManager;

  LoadModels(rm);
  LoadFonts(rm);
  LoadSounds(rm);
  LoadMusic(rm);
  LoadTextures(rm);
  LoadRenderTextures(rm, GetScreenResolution(&systems->configManager));
}

// Load all models
static void LoadModels(ResourceManager* rm) {
  // Player Models
  rm->models[MODEL_ID_MENU] = LoadModel("resources/models/player/mechafullmenu.obj");

  rm->models[MODEL_ID_MISSION_BRIEFING] = LoadModel("resources/models/player/debriefing.glb");
  rm->models[MODEL_ID_MISSION_BRIEFING].transform = MatrixMultiply(MatrixScale(3.5f, 3.5f, 3.5f), MatrixRotateY(0.0f));

  // Enemy Models
  rm->models[MODEL_ID_ENEMY_SCOUT] = LoadModel("resources/models/enemies/enemy_scout.glb");
  rm->models[MODEL_ID_ENEMY_SCOUT].transform = MatrixMultiply(MatrixScale(0.25f, 0.25f, 0.25f), MatrixRotateY(PI));
  rm->modelAnimations[MODEL_ID_ENEMY_SCOUT] = LoadModelAnimations("resources/models/enemies/enemy_scout.glb",&rm->modelAnimCounts[MODEL_ID_ENEMY_SCOUT]);

  rm->models[MODEL_ID_ENEMY_FIGHTER] = LoadModel("resources/models/enemies/enemy_fighter.glb");
  rm->models[MODEL_ID_ENEMY_FIGHTER].transform = MatrixMultiply(MatrixScale(0.35f, 0.35f, 0.35f), MatrixRotateY(PI));
  rm->modelAnimations[MODEL_ID_ENEMY_FIGHTER] = LoadModelAnimations("resources/models/enemies/enemy_fighter.glb",&rm->modelAnimCounts[MODEL_ID_ENEMY_FIGHTER]);

  rm->models[MODEL_ID_ENEMY_BOSS] = LoadModel("resources/models/enemies/enemy_boss.glb");
  rm->models[MODEL_ID_ENEMY_BOSS].transform = MatrixMultiply(MatrixScale(0.40f, 0.40f, 0.40f), MatrixRotateY(0.0f));
  rm->modelAnimations[MODEL_ID_ENEMY_BOSS] = LoadModelAnimations("resources/models/enemies/enemy_boss.glb", &rm->modelAnimCounts[MODEL_ID_ENEMY_BOSS]);

  rm->models[MODEL_ID_TURRET_STRUCTURE] = LoadModel("resources/models/enemies/turret.glb");
  rm->models[MODEL_ID_TURRET_STRUCTURE].transform = MatrixScale(24.0f, 24.0f, 24.0f);

  // Environment Models
  rm->models[MODEL_ID_HANGAR_ENV] = LoadModel("resources/models/environment/hangar.glb");
  for (int i = 0; i < rm->models[MODEL_ID_HANGAR_ENV].materialCount; i++) {
    rm->models[MODEL_ID_HANGAR_ENV].materials[i].maps[MATERIAL_MAP_DIFFUSE].color = MATERIAL_GRAY;
  }

  // Projectile Models (Procedural)
  Mesh pulseLaserMesh = GenMeshCube(0.075f, 0.075f, 5.0f);
  rm->models[MODEL_ID_PROJECTILE_PULSE_LASER] = LoadModelFromMesh(pulseLaserMesh);
  rm->models[MODEL_ID_PROJECTILE_PULSE_LASER].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  Mesh missileMesh = GenMeshCone(0.3f, 1.0f, 4);
  Model missileModel = LoadModelFromMesh(missileMesh);
  missileModel.transform = MatrixRotateX(-90 * DEG2RAD);
  rm->models[MODEL_ID_PROJECTILE_MISSILE_LAUNCHER] = missileModel;

  // Utility Models
  Mesh dummyMesh = GenMeshCube(3.0f, 3.0f, 3.0f);
  rm->models[MODEL_ID_DUMMY] = LoadModelFromMesh(dummyMesh);

  // Terrain (Procedural)
  const float PLANE_SIZE = 2000.0f;
  const int SUBDIVISIONS = 20;
  const float TEXTURE_REPEAT = 200.0f;

  Mesh groundMesh = GenMeshPlane(PLANE_SIZE, PLANE_SIZE, SUBDIVISIONS, SUBDIVISIONS);
  for (int i = 0; i < groundMesh.vertexCount * 2; i++) {
    groundMesh.texcoords[i] *= TEXTURE_REPEAT;
  }
  rm->models[MODEL_ID_TERRAIN] = LoadModelFromMesh(groundMesh);

  AssetModelID animatedEnemies[] = {MODEL_ID_ENEMY_BOSS, MODEL_ID_ENEMY_SCOUT, MODEL_ID_ENEMY_FIGHTER};
  for (int i = 0; i < (int)(sizeof(animatedEnemies)/sizeof(animatedEnemies[0])); i++) {
    AssetModelID id = animatedEnemies[i];
    if (rm->modelAnimations[id] == NULL || rm->modelAnimCounts[id] <= 0) {
      TraceLog(LOG_WARNING, "RM: Missing animations for model ID %d", id);
    }
  }
}

static void LoadFonts(ResourceManager* rm) {
  rm->fonts[FONT_ID_OXIDO_ERODE] = LoadFontEx("resources/fonts/oxido_erode.ttf", 150, NULL, 0);
  rm->fonts[FONT_ID_CODE_PREDATORS] = LoadFontEx("resources/fonts/code_predators.ttf", 150, NULL, 0);
  rm->fonts[FONT_ID_CAPTURE_IT] = LoadFontEx("resources/fonts/capture_it.ttf", 150, NULL, 0);

  for (int i = 0; i < FONT_ID_COUNT; i++) {
      if (rm->fonts[i].baseSize == 0) {
          TraceLog(LOG_WARNING, "RM: Font ID %d failed to load", i);
      }
  }
}

static void LoadSounds(ResourceManager* rm) {
  // Mecha Sounds
  rm->sounds[SOUND_ID_MECHA_FOOTSTEP] = LoadSound("resources/sounds/mecha_footstep.wav");
  rm->sounds[SOUND_ID_MECHA_ZOOM] = LoadSound("resources/sounds/zoom.wav");

  // Weapon Sounds
  rm->sounds[SOUND_ID_PULSE_LASER_FIRING] = LoadSound("resources/sounds/pulse_laser_firing.wav");
  rm->sounds[SOUND_ID_PULSE_LASER_IMPACT] = LoadSound("resources/sounds/pulse_laser_impact.wav");
  rm->sounds[SOUND_ID_MISSILE_LAUNCHER_FIRING] = LoadSound("resources/sounds/missile_launcher_firing.wav");
  rm->sounds[SOUND_ID_MISSILE_LAUNCHER_IMPACT] = LoadSound("resources/sounds/missile_launcher_impact.wav");
  rm->sounds[SOUND_ID_MISSILE_FAILED] = LoadSound("resources/sounds/missile_failed.wav");

  // Event Sounds
  rm->sounds[SOUND_ID_ENEMY_MECH_DESTROYED] = LoadSound("resources/sounds/enemy_mech_death.wav");
  rm->sounds[SOUND_ID_ENEMY_TARGET_DESTROYED] = LoadSound("resources/sounds/target_destroyed.wav");
  rm->sounds[SOUND_ID_BOSS_BRIEFING] = LoadSound("resources/sounds/boss-briefing.wav");
  rm->sounds[SOUND_ID_MISSION_SUCCESS] = LoadSound("resources/sounds/mission_success.wav");
  rm->sounds[SOUND_ID_END_GAME] = LoadSound("resources/sounds/audio_end_game.wav");
  // Ambient/Sequence Sounds
  rm->sounds[SOUND_ID_HUMAN_FOOTSTEP] = LoadSound("resources/sounds/human_footstep.wav");
  rm->sounds[SOUND_ID_SKYDROP] = LoadSound("resources/sounds/skydrop.wav");
  rm->sounds[SOUND_ID_STARTUP_SEQUENCE] = LoadSound("resources/sounds/startup_sequence.wav");

  for (int i = 0; i < SOUND_ID_COUNT; i++) {
      if (rm->sounds[i].frameCount == 0) {
          TraceLog(LOG_WARNING, "RM: Sound ID %d failed to load", i);
      }
  }
}

static void LoadMusic(ResourceManager* rm) {
  rm->musics[MUSIC_ID_MENU] = LoadMusicStream("resources/musics/menu_music.mp3");
  rm->musics[MUSIC_ID_FIRST_LEVEL] = LoadMusicStream("resources/musics/first_level_music.mp3");
  rm->musics[MUSIC_ID_SECOND_LEVEL] = LoadMusicStream("resources/musics/second_level_music.mp3");
  rm->musics[MUSIC_ID_MISSION_BRIEFING] = LoadMusicStream("resources/musics/debriefing.mp3");

  for (int i = 0; i < MUSIC_ID_COUNT; i++) {
      if (rm->musics[i].stream.buffer == NULL) {
           TraceLog(LOG_WARNING, "RM: Music ID %d failed to load", i);
      }
  }
}

static void LoadTextures(ResourceManager* rm) {
  // Effect Textures
  rm->textures[TEXTURE_ID_LASER_EXPLOSION_SPRITESHEET] = LoadTexture("resources/textures/laser_pulse_explosion.png");
  rm->textures[TEXTURE_ID_MISSILE_EXPLOSION_SPRITESHEET] = LoadTexture("resources/textures/missile_explosion.png");

  // Smoke Textures (Sequential)
  for (int i = 0; i < 10; i++) {
    const char* fileName = TextFormat("resources/textures/smoke_%02d.png", i + 1);
    rm->textures[TEXTURE_ID_SMOKE + i] = LoadTexture(fileName);
    SetTextureFilter(rm->textures[TEXTURE_ID_SMOKE + i], TEXTURE_FILTER_BILINEAR);
  }

  // UI Textures
  rm->textures[TEXTURE_ID_CROSSHAIR_SPRITE] = LoadTexture("resources/textures/crosshair.png");

  // Environment Textures
  rm->textures[TEXTURE_ID_SAND] = LoadTexture("resources/textures/floor.png");
  GenTextureMipmaps(&rm->textures[TEXTURE_ID_SAND]);
  SetTextureFilter(rm->textures[TEXTURE_ID_SAND], TEXTURE_FILTER_ANISOTROPIC_16X);
  SetTextureWrap(rm->textures[TEXTURE_ID_SAND], TEXTURE_WRAP_REPEAT);

  // Apply terrain texture to terrain model
  rm->models[MODEL_ID_TERRAIN].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = rm->textures[TEXTURE_ID_SAND];

  for (int i = 0; i < TEXTURE_ID_COUNT; i++) {
      if (rm->textures[i].id == 0) {
          TraceLog(LOG_WARNING, "RM: Texture ID %d failed to load", i);
      }
  }
}

static void LoadRenderTextures(ResourceManager* rm, Vector2 screenResolution) {
  rm->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MENU] = LoadRenderTexture(screenResolution.x/2, screenResolution.y);
  rm->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MECHA] = LoadRenderTexture(screenResolution.x/2, screenResolution.y);

  for (int i = 0; i < RENDERTEXTURE_ID_COUNT; i++) {
      if (rm->renderTextures[i].id == 0) {
          TraceLog(LOG_WARNING, "RM: RenderTexture ID %d failed to load", i);
      }
  }
}

void ShutdownResourceManager(ResourceManager* rm) {

  for (int i = 0; i < MODEL_ID_COUNT; i++) {
    UnloadModel(rm->models[i]);
  }

  // Unload model animations
  for (int i = 0; i < MODEL_ID_COUNT; i++) {
    if (rm->modelAnimations[i] != NULL && rm->modelAnimCounts[i] > 0) {
      UnloadModelAnimations(rm->modelAnimations[i], rm->modelAnimCounts[i]);
      rm->modelAnimations[i] = NULL;
      rm->modelAnimCounts[i] = 0;
    }
  }
  for (int i = 0; i < FONT_ID_COUNT; i++) {
    UnloadFont(rm->fonts[i]);
  }
  for(int i = 0; i < MUSIC_ID_COUNT; i++){
    UnloadMusicStream(rm->musics[i]);
  }
  for (int i = 0; i < SOUND_ID_COUNT; i++){
    UnloadSound(rm->sounds[i]);
  }
  for (int i = 0; i < TEXTURE_ID_COUNT; i++){
    UnloadTexture(rm->textures[i]);
  }
  for(int i = 0; i < RENDERTEXTURE_ID_COUNT; i++){
    UnloadRenderTexture(rm->renderTextures[i]);
  }
}


Model* GetModel(ResourceManager* rm, AssetModelID id) {
  if (id < MODEL_ID_COUNT) {
    return &rm->models[id];
  }
  // Return nullpointer if ID is not valid
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Model access");
  return NULL;
}

Font* GetFont(ResourceManager* rm, AssetFontID id) {
  if (id < FONT_ID_COUNT) {
    return &rm->fonts[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Font access");
  return NULL;
}

Sound* GetSound(ResourceManager* rm, AssetSoundID id) {
  if (id < SOUND_ID_COUNT) {
    return &rm->sounds[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Sound access");
  return NULL;
}

Music* GetMusic(ResourceManager* rm, AssetMusicID id) {
  if (id < MUSIC_ID_COUNT) {
    return &rm->musics[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Music access");
  return NULL;
}

Texture* GetTexture(ResourceManager* rm, AssetTextureID id){
  if (id < TEXTURE_ID_COUNT) {
    return &rm->textures[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Texture access");
  return NULL;
}

RenderTexture* GetRenderTexture(ResourceManager* rm, AssetRenderTextureID id){
  if (id < RENDERTEXTURE_ID_COUNT) {
    return &rm->renderTextures[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Render Texture access");
  return NULL;
}
