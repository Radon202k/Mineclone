#ifndef GAME_H
#define GAME_H

typedef struct Game {
    Camera playerCamera;
    v2 lastMouseP;
    
    s32 chunkLoadingDiameter;

    s32 centerChunkX;
    s32 centerChunkY;
    s32 centerChunkZ;
} Game;

internal void player_movement_input(void);

global Game game;


#endif //GAME_H
