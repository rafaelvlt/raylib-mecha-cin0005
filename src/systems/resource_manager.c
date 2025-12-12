#include "resource_manager.h"
#include <raylib.h>
<<<<<<< HEAD
#include "systems.h"
#include "utility.h"
#include "raymath.h"
=======
#include <raymath.h>
#include "systems.h"
#include "utility.h"
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee

/****************************************************
See the resource_manager.h for how to use guideline *
Only Change the init function to add more assets    *
Don't forget to add the ID to the enum              *
*****************************************************/

<<<<<<< HEAD

void InitResourceManager(ResourceManager* resourceManager) {

  // ---------------------------------------------------------
  // MODELS 
  // ---------------------------------------------------------

  //Mechas
  resourceManager->models[MODEL_ID_MENU] = LoadModel("resources/models/player/mechafullmenu.obj");
  resourceManager->models[MODEL_ID_ENEMY_SCOUT] = LoadModel("resources/models/enemies/scout.glb");
  resourceManager->models[MODEL_ID_ENEMY_SCOUT].transform = MatrixMultiply(
    MatrixScale(0.5f, 0.5f, 0.5f), 
    MatrixRotateY(PI)
  );

  //Structures
  resourceManager->models[MODEL_ID_TURRET_STRUCTURE] = LoadModel("resources/models/enemies/turret.glb");
  resourceManager->models[MODEL_ID_TURRET_STRUCTURE].transform = MatrixScale(8.0f, 8.0f, 8.0f);

  //Projectiles
  //Pulse Laser 
  Mesh pulseLaserGunProjectileMesh = GenMeshCube(0.025f, 0.025f, 3.0f);
  resourceManager->models[MODEL_ID_PROJECTILE_PULSE_LASER] = LoadModelFromMesh(pulseLaserGunProjectileMesh);
  // Lighting up
  resourceManager->models[MODEL_ID_PROJECTILE_PULSE_LASER].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  //Missile
  Mesh missileMesh = GenMeshCone(0.3f, 1.0f, 4);
  Matrix rotation = MatrixRotateX(-90 * DEG2RAD);
  Model missileModel = LoadModelFromMesh(missileMesh);
  missileModel.transform = rotation;
  resourceManager->models[MODEL_ID_PROJECTILE_MISSILE_LAUNCHER] = missileModel;

  // Dummy
  Mesh cubmesh = GenMeshCube(3.0f, 3.0f, 3.0f);
  resourceManager->models[MODEL_ID_DUMMY] =  LoadModelFromMesh(cubmesh); 
  // ---------------------------------------------------------
  // FONTS 
  // ---------------------------------------------------------
  resourceManager->fonts[FONT_ID_OXIDO_ERODE] = LoadFontEx("resources/fonts/oxido_erode.ttf", 150, NULL, 0);
  resourceManager->fonts[FONT_ID_CODE_PREDATORS] = LoadFontEx("resources/fonts/code_predators.ttf", 150, NULL, 0);
  resourceManager->fonts[FONT_ID_CAPTURE_IT] = LoadFontEx("resources/fonts/capture_it.ttf", 150, NULL, 0);

  // ---------------------------------------------------------
  // SOUNDS 
  // ---------------------------------------------------------
  resourceManager->sounds[SOUND_ID_MECHA_FOOTSTEP] = LoadSound("resources/sounds/mecha_footstep.wav");
  resourceManager->sounds[SOUND_ID_MECHA_ZOOM] = LoadSound("resources/sounds/zoom.wav");

  resourceManager->sounds[SOUND_ID_PULSE_LASER_FIRING] = LoadSound("resources/sounds/pulse_laser_firing.wav");
  resourceManager->sounds[SOUND_ID_PULSE_LASER_IMPACT] = LoadSound("resources/sounds/pulse_laser_impact.wav");

  resourceManager->sounds[SOUND_ID_MISSILE_LAUNCHER_FIRING] = LoadSound("resources/sounds/missile_launcher_firing.wav");
  resourceManager->sounds[SOUND_ID_MISSILE_LAUNCHER_IMPACT] = LoadSound("resources/sounds/missile_launcher_impact.wav");
  resourceManager->sounds[SOUND_ID_MISSILE_FAILED] = LoadSound("resources/sounds/missile_failed.wav");

  resourceManager->sounds[SOUND_ID_ENEMY_MECH_DESTROYED] = LoadSound("resources/sounds/enemy_mech_death.wav");
  resourceManager->sounds[SOUND_ID_ENEMY_TARGET_DESTROYED] = LoadSound("resources/sounds/target_destroyed.wav");

  resourceManager->sounds[SOUND_ID_MISSION_SUCCESS] = LoadSound("resources/sounds/mission_success.wav");
  // ---------------------------------------------------------
  // MUSIC 
  // ---------------------------------------------------------
  resourceManager->musics[MUSIC_ID_MENU] = LoadMusicStream("resources/musics/menu_music.mp3");
  resourceManager->musics[MUSIC_ID_FIRST_LEVEL] = LoadMusicStream("resources/musics/first_level_music.mp3");

  // ---------------------------------------------------------
  // TEXTURES 
  // ---------------------------------------------------------
  resourceManager->textures[TEXTURE_ID_LASER_EXPLOSION_SPRITESHEET] = LoadTexture("resources/textures/laser_pulse_explosion.png");
  resourceManager->textures[TEXTURE_ID_MISSILE_EXPLOSION_SPRITESHEET] = LoadTexture("resources/textures/missile_explosion.png");
  // Smoke
  for (int i = 0; i < 10; i++) {
    const char* fileName = TextFormat("resources/textures/smoke_%02d.png", i + 1);
    resourceManager->textures[TEXTURE_ID_SMOKE + i] = LoadTexture(fileName);
    SetTextureFilter(resourceManager->textures[TEXTURE_ID_SMOKE + i], TEXTURE_FILTER_BILINEAR);
  }

  resourceManager->textures[TEXTURE_ID_CROSSHAIR_SPRITE] = LoadTexture("resources/textures/crosshair.png");

  // ---------------------------------------------------------
  // RENDER TEXTURES 
  // ---------------------------------------------------------
  resourceManager->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MENU] = LoadRenderTexture(SCREEN_WIDTH/2, SCREEN_HEIGHT);
  resourceManager->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MECHA] = LoadRenderTexture(SCREEN_WIDTH/2, SCREEN_HEIGHT);
  resourceManager->textures[TEXTURE_ID_BASE_HQ] = LoadTexture("resources/textures/base_texture.png");
  GenTextureMipmaps(&resourceManager->textures[TEXTURE_ID_BASE_HQ]);
  SetTextureFilter(resourceManager->textures[TEXTURE_ID_BASE_HQ], TEXTURE_FILTER_ANISOTROPIC_16X);

  resourceManager->textures[TEXTURE_ID_SAND] = LoadTexture("resources/textures/sand.png");
  GenTextureMipmaps(&resourceManager->textures[TEXTURE_ID_SAND]);

  SetTextureFilter(resourceManager->textures[TEXTURE_ID_SAND], TEXTURE_FILTER_ANISOTROPIC_16X);

  SetTextureWrap(resourceManager->textures[TEXTURE_ID_SAND], TEXTURE_WRAP_REPEAT);
  SetTextureWrap(resourceManager->textures[TEXTURE_ID_SAND], TEXTURE_WRAP_REPEAT); 

  Mesh floorMesh = GenMeshPlane(2000.0f, 2000.0f, 20, 20);


  if (floorMesh.texcoords) {
    for (int i = 0; i < floorMesh.vertexCount * 2; i++) {
      floorMesh.texcoords[i] *= 400.0f;
    }
  }
  Mesh hqMesh = GenMeshCube(50.0f, 40.0f, 40.0f);
  Mesh doorMesh = GenMeshCube(15.0f, 12.0f, 1.0f);
  resourceManager->models[MODEL_ID_BASE_DOOR] = LoadModelFromMesh(doorMesh); 
  resourceManager->models[MODEL_ID_BASE_DOOR].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){20, 20, 20, 255};

  // Tiling
  if (hqMesh.texcoords) {
    for (int i = 0; i < hqMesh.vertexCount * 2; i++) {
      hqMesh.texcoords[i] *= 4.0f; 
    }
  }

  // Creates Model and apply texture
  resourceManager->models[MODEL_ID_TERRAIN] = LoadModelFromMesh(floorMesh);
  resourceManager->models[MODEL_ID_TERRAIN].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = resourceManager->textures[TEXTURE_ID_SAND];

  resourceManager->models[MODEL_ID_TERRAIN].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;
  resourceManager->models[MODEL_ID_BASE_HQ] = LoadModelFromMesh(hqMesh);
  resourceManager->models[MODEL_ID_BASE_HQ].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = resourceManager->textures[TEXTURE_ID_BASE_HQ];
  resourceManager->models[MODEL_ID_BASE_HQ].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

}

void ShutdownResourceManager(ResourceManager* resourceManager) {

  for (int i = 0; i < MODEL_ID_COUNT; i++) {
    UnloadModel(resourceManager->models[i]);
  }
  for (int i = 0; i < FONT_ID_COUNT; i++) {
    UnloadFont(resourceManager->fonts[i]);
  }
  for(int i = 0; i < MUSIC_ID_COUNT; i++){
    UnloadMusicStream(resourceManager->musics[i]);
  }
  for (int i = 0; i < SOUND_ID_COUNT; i++){
    UnloadSound(resourceManager->sounds[i]);
  }
  for (int i = 0; i < TEXTURE_ID_COUNT; i++){
    UnloadTexture(resourceManager->textures[i]);
  }
  for(int i = 0; i < RENDERTEXTURE_ID_COUNT; i++){
    UnloadRenderTexture(resourceManager->renderTextures[i]);
=======
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

  rm->models[MODEL_ID_DEBRIEFING] = LoadModel("resources/models/player/debriefing.glb");
  rm->models[MODEL_ID_DEBRIEFING].transform = MatrixMultiply(MatrixScale(3.5f, 3.5f, 3.5f), MatrixRotateY(0.0f));

  // Enemy Models
  rm->models[MODEL_ID_ENEMY_SCOUT] = LoadModel("resources/models/enemies/enemy_scout.glb");
  rm->models[MODEL_ID_ENEMY_SCOUT].transform = MatrixMultiply(MatrixScale(0.25f, 0.25f, 0.25f), MatrixRotateY(PI));
  rm->modelAnimations[MODEL_ID_ENEMY_SCOUT] = LoadModelAnimations("resources/models/enemies/enemy_scout.glb",&rm->modelAnimCounts[MODEL_ID_ENEMY_SCOUT]);


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
}

static void LoadFonts(ResourceManager* rm) {
  rm->fonts[FONT_ID_OXIDO_ERODE] = LoadFontEx("resources/fonts/oxido_erode.ttf", 150, NULL, 0);
  rm->fonts[FONT_ID_CODE_PREDATORS] = LoadFontEx("resources/fonts/code_predators.ttf", 150, NULL, 0);
  rm->fonts[FONT_ID_CAPTURE_IT] = LoadFontEx("resources/fonts/capture_it.ttf", 150, NULL, 0);
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
  rm->sounds[SOUND_ID_MISSION_SUCCESS] = LoadSound("resources/sounds/mission_success.wav");

  // Ambient/Sequence Sounds
  rm->sounds[SOUND_ID_HUMAN_FOOTSTEP] = LoadSound("resources/sounds/human_footstep.wav");
  rm->sounds[SOUND_ID_SKYDROP] = LoadSound("resources/sounds/skydrop.wav");
  rm->sounds[SOUND_ID_STARTUP_SEQUENCE] = LoadSound("resources/sounds/startup_sequence.wav");
}

static void LoadMusic(ResourceManager* rm) {
  rm->musics[MUSIC_ID_MENU] = LoadMusicStream("resources/musics/menu_music.mp3");
  rm->musics[MUSIC_ID_FIRST_LEVEL] = LoadMusicStream("resources/musics/first_level_music.mp3");
  rm->musics[MUSIC_ID_DEBRIEFING] = LoadMusicStream("resources/musics/debriefing.mp3");
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
}

static void LoadRenderTextures(ResourceManager* rm, Vector2 screenResolution) {
  rm->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MENU] = LoadRenderTexture(screenResolution.x/2, screenResolution.y);
  rm->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MECHA] = LoadRenderTexture(screenResolution.x/2, screenResolution.y);
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
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  }
}


<<<<<<< HEAD
Model* GetModel(ResourceManager* resourceManager, AssetModelID id) {
  if (id < MODEL_ID_COUNT) {
    return &resourceManager->models[id];
=======
Model* GetModel(ResourceManager* rm, AssetModelID id) {
  if (id < MODEL_ID_COUNT) {
    return &rm->models[id];
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  }
  // Return nullpointer if ID is not valid
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Model access");
  return NULL;
}

<<<<<<< HEAD
Font* GetFont(ResourceManager* resourceManager, AssetFontID id) {
  if (id < FONT_ID_COUNT) {
    return &resourceManager->fonts[id];
=======
Font* GetFont(ResourceManager* rm, AssetFontID id) {
  if (id < FONT_ID_COUNT) {
    return &rm->fonts[id];
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Font access");
  return NULL;
}

<<<<<<< HEAD
Sound* GetSound(ResourceManager* resourceManager, AssetSoundID id) {
  if (id < SOUND_ID_COUNT) {
    return &resourceManager->sounds[id];
=======
Sound* GetSound(ResourceManager* rm, AssetSoundID id) {
  if (id < SOUND_ID_COUNT) {
    return &rm->sounds[id];
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Sound access");
  return NULL;
}

<<<<<<< HEAD
Music* GetMusic(ResourceManager* resourceManager, AssetMusicID id) {
  if (id < MUSIC_ID_COUNT) {
    return &resourceManager->musics[id];
=======
Music* GetMusic(ResourceManager* rm, AssetMusicID id) {
  if (id < MUSIC_ID_COUNT) {
    return &rm->musics[id];
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Music access");
  return NULL;
}

<<<<<<< HEAD
Texture* GetTexture(ResourceManager* resourceManager, AssetTextureID id){
  if (id < TEXTURE_ID_COUNT) {
    return &resourceManager->textures[id];
=======
Texture* GetTexture(ResourceManager* rm, AssetTextureID id){
  if (id < TEXTURE_ID_COUNT) {
    return &rm->textures[id];
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Texture access");
  return NULL;
}

<<<<<<< HEAD
RenderTexture* GetRenderTexture(ResourceManager* resourceManager, AssetRenderTextureID id){
  if (id < RENDERTEXTURE_ID_COUNT) {
    return &resourceManager->renderTextures[id];
=======
RenderTexture* GetRenderTexture(ResourceManager* rm, AssetRenderTextureID id){
  if (id < RENDERTEXTURE_ID_COUNT) {
    return &rm->renderTextures[id];
>>>>>>> e5f71c4fbed50bc18b1bb6381d1332ffd5198bee
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Render Texture access");
  return NULL;
}
