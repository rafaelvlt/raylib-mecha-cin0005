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
