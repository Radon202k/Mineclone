#ifndef GAME_H
#define GAME_H

typedef enum {
    BLOCK_TEXTURE_DIRT,
    BLOCK_TEXTURE_STONE,
    BLOCK_TEXTURE_GRASS_BLOCK_TOP,
    BLOCK_TEXTURE_COUNT
} BlockTexture;

typedef struct Game {
    f32 elapsedTime;
    
    Lines3D lines;
    
    Camera playerCamera;
    v2 lastMouseP;
    
    s32 chunkLoadingDiameter;
    
    s32 centerChunkX;
    s32 centerChunkY;
    s32 centerChunkZ;
    
    char *textureFilenames[BLOCK_TEXTURE_COUNT];
} Game;

internal void player_movement_input(void);

global Game game;


#endif //GAME_H
