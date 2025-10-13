#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <raylib.h>

struct Systems;

typedef enum {
    MODEL_ID_MENU = 0,
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
    MUSIC_ID_COUNT
} AssetMusicID;

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
    RenderTexture renderTextures[RENDERTEXTURE_ID_COUNT];
} ResourceManager;

void InitResourceManager(ResourceManager* resourceManager) ;
void ShutdownResourceManager(ResourceManager* resourceManager) ;

Model* GetModel(ResourceManager* resourceManager, AssetModelID id);
Font* GetFont(ResourceManager* resourceManager, AssetFontID id);
Music* GetMusic(ResourceManager* resourceManager, AssetMusicID id);
RenderTexture* GetRenderTexture(ResourceManager* resourceManager, AssetRenderTextureID id);

#endif
