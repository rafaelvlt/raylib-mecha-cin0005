#include "resource_manager.h"
#include <raylib.h>
#include "systems.h"
#include "utility.h"

/****************************************************
See the resource_manager.h for how to use guideline *
Only Change the init function to add more assets    *
Don't forget to add the ID to the enum              *
*****************************************************/

void InitResourceManager(ResourceManager* resourceManager) {
  

  // ---------------------------------------------------------
  // MODELS 
  // ---------------------------------------------------------
  
  //Mechas
  resourceManager->models[MODEL_ID_MENU] = LoadModel("resources/models/player/mechafullmenu.obj");
  resourceManager->models[MODEL_ID_ENEMY_SCOUT] = LoadModel("resources/models/enemies/scout.glb");

  //Projectiles
  //Pulse Laser 
  Mesh pulseLaserGunProjectileMesh = GenMeshCube(0.025f, 0.025f, 3.0f);
  resourceManager->models[MODEL_ID_PROJECTILE_PULSE_LASER] = LoadModelFromMesh(pulseLaserGunProjectileMesh);
  // Lighting up
  resourceManager->models[MODEL_ID_PROJECTILE_PULSE_LASER].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

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

  resourceManager->sounds[SOUND_ID_PULSE_LASER_FIRING] = LoadSound("resources/sounds/pulse_laser_firing.wav");
  resourceManager->sounds[SOUND_ID_PULSE_LASER_IMPACT] = LoadSound("resources/sounds/pulse_laser_impact.wav");
  // ---------------------------------------------------------
  // MUSIC 
  // ---------------------------------------------------------
  resourceManager->musics[MUSIC_ID_MENU] = LoadMusicStream("resources/musics/menu_music.mp3");

  // ---------------------------------------------------------
  // TEXTURES 
  // ---------------------------------------------------------
  resourceManager->textures[TEXTURE_ID_EXPLOSION_SPRITESHEET] = LoadTexture("resources/textures/explosion.png");
  resourceManager->textures[TEXTURE_ID_CROSSHAIR_SPRITE] = LoadTexture("resources/textures/crosshair.png");
  
  // ---------------------------------------------------------
  // RENDER TEXTURES 
  // ---------------------------------------------------------
  resourceManager->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MENU] = LoadRenderTexture(SCREEN_WIDTH/2, SCREEN_HEIGHT);
  resourceManager->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MECHA] = LoadRenderTexture(SCREEN_WIDTH/2, SCREEN_HEIGHT);
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
  }
}


Model* GetModel(ResourceManager* resourceManager, AssetModelID id) {
  if (id < MODEL_ID_COUNT) {
    return &resourceManager->models[id];
  }
  // Return nullpointer if ID is not valid
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Model access");
  return NULL;
}

Font* GetFont(ResourceManager* resourceManager, AssetFontID id) {
  if (id < FONT_ID_COUNT) {
    return &resourceManager->fonts[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Font access");
  return NULL;
}

Sound* GetSound(ResourceManager* resourceManager, AssetSoundID id) {
  if (id < SOUND_ID_COUNT) {
    return &resourceManager->sounds[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Sound access");
  return NULL;
}

Music* GetMusic(ResourceManager* resourceManager, AssetMusicID id) {
  if (id < MUSIC_ID_COUNT) {
    return &resourceManager->musics[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Music access");
  return NULL;
}

Texture* GetTexture(ResourceManager* resourceManager, AssetTextureID id){
  if (id < TEXTURE_ID_COUNT) {
    return &resourceManager->textures[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Texture access");
  return NULL;
}

RenderTexture* GetRenderTexture(ResourceManager* resourceManager, AssetRenderTextureID id){
  if (id < RENDERTEXTURE_ID_COUNT) {
    return &resourceManager->renderTextures[id];
  }
  TraceLog(LOG_WARNING, "RESOURCE MANAGER: Invalid Render Texture access");
  return NULL;
}
