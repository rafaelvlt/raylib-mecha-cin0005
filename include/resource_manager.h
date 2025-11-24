#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <raylib.h>

struct Systems;


/*******************************************************************************************************
Every time you add some asset to the game, you need to add it as a ID to the correct enum depending    *
on the asset type. Example: added a font, i need to go to AssetFontID and add a ID above FONT_ID_COUNT.*
All the ID ending in count should be the last ID of the enum.                                          *
After adding a ID, you need to import it at resource_manager.c on the init function.                   *
Just see the template for the other assets and follow it.                                              *
                                                                                                       *
To get an asset from the resource manager just use one of the get functions.                           *
Example: Get<AssetType>(&(systems->resourceManager), <ASSET>_ID_<ASSETNAME>);                          *
                                                                                                       *
To add any other asset type, just make a enum for it, just make a enum using the same template         *
and add a array to the resourceManager Struct + Make a Get function for it                             *
********************************************************************************************************/

typedef enum {
    MODEL_ID_MENU = 0,
    MODEL_ID_ENEMY_SCOUT,
    MODEL_ID_PROJECTILE_PULSE_LASER,
    MODEL_ID_PROJECTILE_MISSILE_LAUNCHER,
    MODEL_ID_DUMMY,
    MODEL_ID_TERRAIN,
    MODEL_ID_BASE_HQ,
    MODEL_ID_BASE_DOOR,
    MODEL_ID_COUNT
} AssetModelID;

typedef enum {
    FONT_ID_OXIDO_ERODE = 0,
    FONT_ID_CODE_PREDATORS,
    FONT_ID_CAPTURE_IT,
    FONT_ID_COUNT
} AssetFontID;

typedef enum {
    MUSIC_ID_MENU = 0,
    MUSIC_ID_FASE_1,
    MUSIC_ID_COUNT
} AssetMusicID;

typedef enum {
  SOUND_ID_MECHA_FOOTSTEP = 0,
  SOUND_ID_MECHA_ROTATING,
  SOUND_ID_MECHA_ZOOM,
  SOUND_ID_PULSE_LASER_FIRING,
  SOUND_ID_PULSE_LASER_IMPACT,
  SOUND_ID_MISSILE_LAUNCHER_FIRING,
  SOUND_ID_MISSILE_LAUNCHER_IMPACT,
  SOUND_ID_MISSILE_FAILED,
  SOUND_ID_COUNT
} AssetSoundID;

typedef enum{
  EFFECT_ID_EXPLOSION,
  EFFECT_ID_COUNT
} AssetEffectID;

typedef enum{
  TEXTURE_ID_EXPLOSION_SPRITESHEET = 0,
  TEXTURE_ID_SAND,
  TEXTURE_ID_BASE_HQ,
  TEXTURE_ID_COUNT 
} AssetTextureID;

typedef enum {
    RENDERTEXTURE_ID_SPLITSCREEN_MENU = 0,
    RENDERTEXTURE_ID_SPLITSCREEN_MECHA,
    RENDERTEXTURE_ID_COUNT
} AssetRenderTextureID;


typedef struct ResourceManager {
    //Resources loaded in memory
    Model models[MODEL_ID_COUNT];
    Font  fonts[FONT_ID_COUNT];
    Music musics[MUSIC_ID_COUNT];
    Sound sounds[SOUND_ID_COUNT];
    Texture textures[TEXTURE_ID_COUNT];
    RenderTexture renderTextures[RENDERTEXTURE_ID_COUNT];
} ResourceManager;

void InitResourceManager(ResourceManager* resourceManager) ;
void ShutdownResourceManager(ResourceManager* resourceManager) ;

Model* GetModel(ResourceManager* resourceManager, AssetModelID id);
Font* GetFont(ResourceManager* resourceManager, AssetFontID id);
Music* GetMusic(ResourceManager* resourceManager, AssetMusicID id);
Sound* GetSound(ResourceManager* resourceManager, AssetSoundID id);
Texture* GetTexture(ResourceManager* resourceManager, AssetTextureID id);
RenderTexture* GetRenderTexture(ResourceManager* resourceManager, AssetRenderTextureID id);

#endif
