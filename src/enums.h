#ifndef ENUMS_H
#define ENUMS_H

typedef enum EntityType{
    PLAYER,
    ASTEROID,
    BULLET
} EntityType;

typedef enum GameState{
    MAIN_MENU,
    IN_GAME,
    GAME_OVER,
    PAUSE,
    QUIT
} GameState;

#endif // ENUMS_H