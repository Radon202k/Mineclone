#ifndef GAME_H
#define GAME_H

typedef struct Game {
    Camera playerCamera;
    v2 lastMouseP;
    
    s32 chunkLoadingRadius;
    f32 chunkLoadingMaxDistance;
    v3 chunkLoadingCenter;
} Game;

internal void player_movement_input(void);

global Game game;


#endif //GAME_H
