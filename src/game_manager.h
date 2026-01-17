#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#ifdef IS_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            "330"
#else
#define GLSL_VERSION            "100"
#endif

#include "enums.h"
#include "in_game.h"


//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
int screenWidth = 450;
int screenHeight = 1000;
GameState gameState = IN_GAME; // TODO: Change to MAIN_MENU later
float scaleRatio = 1.0f;

float virtualLeftBorder = 0.0f;
float virtualRightBorder = 0.0f;

float delta = 0.0f;

bool CreateGameManager(void)
{
    InitWindow(screenWidth, screenHeight, "Raylib Template");

    virtualLeftBorder = 0.0f;
    virtualRightBorder = (float)GetScreenWidth();

    if (GetScreenWidth() < GetScreenHeight())
    {
        scaleRatio = (float)GetScreenWidth() / 450.0f;
    }
    else 
    {
        scaleRatio = (float)GetScreenHeight() / 800.0f;
        virtualLeftBorder = (float)GetScreenWidth()*0.5f - (225.0f*scaleRatio);
        virtualRightBorder = (float)GetScreenWidth()*0.5f + (225.0f*scaleRatio);
    }
    CreateInGame(scaleRatio, virtualLeftBorder, virtualRightBorder);

    return true;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
bool UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    if (IsWindowResized())
    {
        if (GetScreenWidth() < GetScreenHeight())
        {
            scaleRatio = (float)GetScreenWidth() / 450.0f;
        }
        else 
        {
            scaleRatio = (float)GetScreenHeight() / 800.0f;
            virtualLeftBorder = (float)GetScreenWidth()*0.5f - (225.0f*scaleRatio);
            virtualRightBorder = (float)GetScreenWidth()*0.5f + (225.0f*scaleRatio);
        }
    }

    delta = GetFrameTime();
    
    switch (gameState)
    {
        case MAIN_MENU:

            break;
        case IN_GAME:
            gameState = UpdateInGame(gameState, scaleRatio, delta);

            BeginDrawing();
                ClearBackground(BLACK);
                DrawInGame(gameState, scaleRatio);
                DrawRectangleRec((Rectangle){0, 0, virtualLeftBorder, (float)GetScreenHeight()}, BLACK);
                DrawRectangleRec((Rectangle){virtualRightBorder, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, BLACK);
                DrawFPS(10, 10);
            EndDrawing();
            break;
        case GAME_OVER:
            break;
        default:
            return false;
            break;
    }

    return true;
}

void DestroyGameManager(void)
{
    DestroyInGame();
}
#endif //GAMEMANAGER_H