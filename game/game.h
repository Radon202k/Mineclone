#ifndef GAME_H
#define GAME_H

typedef struct Game {
    Camera playerCamera;
    v2 lastMouseP;
} Game;

global Game game;

#endif //GAME_H
