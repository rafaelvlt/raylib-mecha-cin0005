#include "resource_manager.h"
#include <raylib.h>
#include "systems.h"
#include "utility.h"
#include "raymath.h"

/****************************************************
See the resource_manager.h for how to use guideline *
Only Change the init function to add more assets    *
Don't forget to add the ID to the enum              *
*****************************************************/


void InitResourceManager(struct Systems* systems) {
  
  ResourceManager* rm = &systems->resourceManager;

  // ---------------------------------------------------------
  // MODELS 
  // ---------------------------------------------------------

  //Mechas
  rm->models[MODEL_ID_MENU] = LoadModel("resources/models/player/mechafullmenu.obj");
  rm->models[MODEL_ID_ENEMY_SCOUT] = LoadModel("resources/models/enemies/scout.glb");
  rm->models[MODEL_ID_ENEMY_SCOUT].transform = MatrixMultiply(
    MatrixScale(0.5f, 0.5f, 0.5f), 
    MatrixRotateY(PI)
  );
  rm->models[MODEL_ID_DEBRIEFING] = LoadModel("resources/models/player/debriefing.glb");
  rm->models[MODEL_ID_DEBRIEFING].transform = MatrixMultiply(MatrixScale(3.5f, 3.5f, 3.5f), MatrixRotateY(0)); 
  //Structures
  rm->models[MODEL_ID_TURRET_STRUCTURE] = LoadModel("resources/models/enemies/turret.glb");
  rm->models[MODEL_ID_TURRET_STRUCTURE].transform = MatrixScale(8.0f, 8.0f, 8.0f);
  
  rm->models[MODEL_ID_HANGAR_ENV] = LoadModel("resources/models/environment/hangar.glb");
  // Darken the model
  for(int i = 0; i < rm->models[MODEL_ID_HANGAR_ENV].materialCount; i++) {
    rm->models[MODEL_ID_HANGAR_ENV].materials[i].maps[MATERIAL_MAP_DIFFUSE].color = (Color){100, 100, 100, 255};
  }

  //Projectiles
  //Pulse Laser 
  Mesh pulseLaserGunProjectileMesh = GenMeshCube(0.025f, 0.025f, 3.0f);
  rm->models[MODEL_ID_PROJECTILE_PULSE_LASER] = LoadModelFromMesh(pulseLaserGunProjectileMesh);
  // Lighting up
  rm->models[MODEL_ID_PROJECTILE_PULSE_LASER].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = WHITE;

  //Missile
  Mesh missileMesh = GenMeshCone(0.3f, 1.0f, 4);
  Matrix rotation = MatrixRotateX(-90 * DEG2RAD);
  Model missileModel = LoadModelFromMesh(missileMesh);
  missileModel.transform = rotation;
  rm->models[MODEL_ID_PROJECTILE_MISSILE_LAUNCHER] = missileModel;

  // Dummy
  Mesh cubmesh = GenMeshCube(3.0f, 3.0f, 3.0f);
  rm->models[MODEL_ID_DUMMY] =  LoadModelFromMesh(cubmesh); 
  // ---------------------------------------------------------
  // FONTS 
  // ---------------------------------------------------------
  rm->fonts[FONT_ID_OXIDO_ERODE] = LoadFontEx("resources/fonts/oxido_erode.ttf", 150, NULL, 0);
  rm->fonts[FONT_ID_CODE_PREDATORS] = LoadFontEx("resources/fonts/code_predators.ttf", 150, NULL, 0);
  rm->fonts[FONT_ID_CAPTURE_IT] = LoadFontEx("resources/fonts/capture_it.ttf", 150, NULL, 0);

  // ---------------------------------------------------------
  // SOUNDS 
  // ---------------------------------------------------------
  rm->sounds[SOUND_ID_MECHA_FOOTSTEP] = LoadSound("resources/sounds/mecha_footstep.wav");
  rm->sounds[SOUND_ID_MECHA_ZOOM] = LoadSound("resources/sounds/zoom.wav");

  rm->sounds[SOUND_ID_PULSE_LASER_FIRING] = LoadSound("resources/sounds/pulse_laser_firing.wav");
  rm->sounds[SOUND_ID_PULSE_LASER_IMPACT] = LoadSound("resources/sounds/pulse_laser_impact.wav");

  rm->sounds[SOUND_ID_MISSILE_LAUNCHER_FIRING] = LoadSound("resources/sounds/missile_launcher_firing.wav");
  rm->sounds[SOUND_ID_MISSILE_LAUNCHER_IMPACT] = LoadSound("resources/sounds/missile_launcher_impact.wav");
  rm->sounds[SOUND_ID_MISSILE_FAILED] = LoadSound("resources/sounds/missile_failed.wav");

  rm->sounds[SOUND_ID_ENEMY_MECH_DESTROYED] = LoadSound("resources/sounds/enemy_mech_death.wav");
  rm->sounds[SOUND_ID_ENEMY_TARGET_DESTROYED] = LoadSound("resources/sounds/target_destroyed.wav");

  rm->sounds[SOUND_ID_MISSION_SUCCESS] = LoadSound("resources/sounds/mission_success.wav");

  rm->sounds[SOUND_ID_HUMAN_FOOTSTEP] = LoadSound("resources/sounds/human_footstep.wav");
  rm->sounds[SOUND_ID_SKYDROP] = LoadSound("resources/sounds/skydrop.wav");
  rm->sounds[SOUND_ID_STARTUP_SEQUENCE] = LoadSound("resources/sounds/startup_sequence.wav");
  // ---------------------------------------------------------
  // MUSIC 
  // ---------------------------------------------------------
  rm->musics[MUSIC_ID_MENU] = LoadMusicStream("resources/musics/menu_music.mp3");
  rm->musics[MUSIC_ID_FIRST_LEVEL] = LoadMusicStream("resources/musics/first_level_music.mp3");

  rm->musics[MUSIC_ID_DEBRIEFING] = LoadMusicStream("resources/musics/debriefing.mp3");
  // ---------------------------------------------------------
  // TEXTURES 
  // ---------------------------------------------------------
  rm->textures[TEXTURE_ID_LASER_EXPLOSION_SPRITESHEET] = LoadTexture("resources/textures/laser_pulse_explosion.png");
  rm->textures[TEXTURE_ID_MISSILE_EXPLOSION_SPRITESHEET] = LoadTexture("resources/textures/missile_explosion.png");
  // Smoke
  for (int i = 0; i < 10; i++) {
    const char* fileName = TextFormat("resources/textures/smoke_%02d.png", i + 1);
    rm->textures[TEXTURE_ID_SMOKE + i] = LoadTexture(fileName);
    SetTextureFilter(rm->textures[TEXTURE_ID_SMOKE + i], TEXTURE_FILTER_BILINEAR);
  }

  rm->textures[TEXTURE_ID_CROSSHAIR_SPRITE] = LoadTexture("resources/textures/crosshair.png");

  // ---------------------------------------------------------
  // RENDER TEXTURES 
  // ---------------------------------------------------------
  rm->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MENU] = LoadRenderTexture(systems->configManager.screenResolution.x/2, systems->configManager.screenResolution.y);
  rm->renderTextures[RENDERTEXTURE_ID_SPLITSCREEN_MECHA] = LoadRenderTexture(systems->configManager.screenResolution.x/2, systems->configManager.screenResolution.y);
  
  rm->textures[TEXTURE_ID_SAND] = LoadTexture("resources/textures/floor.png");

  GenTextureMipmaps(&rm->textures[TEXTURE_ID_SAND]);

  SetTextureFilter(rm->textures[TEXTURE_ID_SAND], TEXTURE_FILTER_ANISOTROPIC_16X); 

  SetTextureWrap(rm->textures[TEXTURE_ID_SAND], TEXTURE_WRAP_REPEAT); 

  Mesh groundMesh = GenMeshPlane(2000.0f, 2000.0f, 40, 40);

  for (int i = 0; i < groundMesh.vertexCount * 2; i++) {
    groundMesh.texcoords[i] *= 2000.0f; 
  }

  rm->models[MODEL_ID_TERRAIN] = LoadModelFromMesh(groundMesh);
  Texture2D* tex = &rm->textures[TEXTURE_ID_SAND];
  *tex = LoadTexture("resources/textures/floor.png"); 
  SetTextureWrap(*tex, TEXTURE_WRAP_REPEAT); 
  SetTextureFilter(*tex, TEXTURE_FILTER_ANISOTROPIC_16X);


  rm->models[MODEL_ID_TERRAIN].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = *tex;
}

void ShutdownResourceManager(ResourceManager* rm) {

  for (int i = 0; i < MODEL_ID_COUNT; i++) {
    UnloadModel(rm->models[i]);
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
