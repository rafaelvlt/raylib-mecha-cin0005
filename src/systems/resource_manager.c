#include "resource_manager.h"
#include <raylib.h>
#include <raymath.h>
#include "systems.h"
#include "utility.h"
#include "raymath.h"

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
  // ---------------------------------------------------------
  // MUSIC 
  // ---------------------------------------------------------
  resourceManager->musics[MUSIC_ID_MENU] = LoadMusicStream("resources/musics/menu_music.mp3");

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

  // Carregar Textura de Grama
  resourceManager->textures[TEXTURE_ID_SAND] = LoadTexture("resources/textures/sand.png");
  GenTextureMipmaps(&resourceManager->textures[TEXTURE_ID_SAND]);

    // Configurar Filtro Anisotrópico
  SetTextureFilter(resourceManager->textures[TEXTURE_ID_SAND], TEXTURE_FILTER_ANISOTROPIC_16X);
    
    // Repetição (Wrap)
  SetTextureWrap(resourceManager->textures[TEXTURE_ID_SAND], TEXTURE_WRAP_REPEAT);
  SetTextureWrap(resourceManager->textures[TEXTURE_ID_SAND], TEXTURE_WRAP_REPEAT); 

    // Gerar a Malha do Terreno
  Mesh floorMesh = GenMeshPlane(2000.0f, 2000.0f, 20, 20);

    // Multiplicamos as coordenadas UV para que a grama se repita 400 vezes
  if (floorMesh.texcoords) {
      for (int i = 0; i < floorMesh.vertexCount * 2; i++) {
            floorMesh.texcoords[i] *= 400.0f;
        }
    }
  Mesh hqMesh = GenMeshCube(50.0f, 40.0f, 40.0f);
  Mesh doorMesh = GenMeshCube(15.0f, 12.0f, 1.0f);
  resourceManager->models[MODEL_ID_BASE_DOOR] = LoadModelFromMesh(doorMesh); 
  resourceManager->models[MODEL_ID_BASE_DOOR].materials[0].maps[MATERIAL_MAP_DIFFUSE].color = (Color){20, 20, 20, 255};
    
    // Tiling: Para a textura não ficar esticada gigante, vamos fazê-la repetir
    // Isso faz a textura se repetir 4 vezes horizontalmente e 2 verticalmente
    if (hqMesh.texcoords) {
         for (int i = 0; i < hqMesh.vertexCount * 2; i++) {
            hqMesh.texcoords[i] *= 4.0f; 
        }
    }

    // 4. Criar o Modelo
  resourceManager->models[MODEL_ID_TERRAIN] = LoadModelFromMesh(floorMesh);
    
    // Aplicar a textura
  resourceManager->models[MODEL_ID_TERRAIN].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = resourceManager->textures[TEXTURE_ID_SAND];

    // =======================================================
    // CORREÇÃO CRÍTICA AQUI
    // =======================================================
    // Define a cor base como BRANCO. Sem isso, a textura fica multiplicada por 0 (preto).
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
