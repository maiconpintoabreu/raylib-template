#ifndef MENU_H
#define MENU_H

#ifdef IS_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

#include "enums.h"

GameState UpdateMenu(GameState gameState)
{
    return gameState;
}

GameState DrawMenu(GameState gameState)
{
    return gameState;
}

#endif // MENU_H