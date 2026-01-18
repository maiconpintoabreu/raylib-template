#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "config.h"
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
GameState gameState = MAIN_MENU; // TODO: Change to MAIN_MENU later
float scaleRatio = 1.0f;
int previousFrameIsGameOnScreen = false;
int isGameOnScreen = true;

float virtualLeftBorder = 0.0f;
float virtualRightBorder = 0.0f;

float delta = 0.0f;

//  Menu
Rectangle exitMenuRec = {0};
Rectangle startMenuRec = {0};
Rectangle restartMenuRec = {0};
Rectangle pauseMenuRec = {0};

Texture2D pauseTexture = {0};


void OnPause(void)
{
    isGameOnScreen = false;
    previousFrameIsGameOnScreen = false;
    TraceLog(LOG_INFO, "OnPause");
}

void OnResume(void)
{
    isGameOnScreen = true;
    TraceLog(LOG_INFO, "OnResume");
}
int MenuButtonWithTexture(Rectangle button, Texture2D texture) {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), button))
    {
        return true;
    }

    DrawTextureV(texture, (Vector2){button.x, button.y}, WHITE);
    return 0;
}

int MenuButton(Rectangle button, const char *button_text) {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), button))
    {
        return true;
    }
    DrawRectangleRounded(button,5, 4, BLACK);
    DrawRectangleRoundedLines(button,5, 4, GRAY);

    DrawText(button_text, (int)button.x + 20, (int)button.y + (int)button.height / 2 - 10, 20, WHITE);
    return 0;
}

void PlaceUIButtons(){
    // Add start button
    startMenuRec.x = ((float)GetScreenWidth() / 2) - MENU_SIZE_WIDTH / 2;
    startMenuRec.y = ((float)GetScreenHeight() / 2) - ITEM_MENU_SIZE_HEIGHT / 1.5f;
    startMenuRec.width = MENU_SIZE_WIDTH;
    startMenuRec.height = ITEM_MENU_SIZE_HEIGHT;
    // Add restart button
    restartMenuRec = startMenuRec;
    // Add exit button
    exitMenuRec.x = ((float)GetScreenWidth() / 2) - MENU_SIZE_WIDTH / 2;
    exitMenuRec.y = ((float)GetScreenHeight() / 2) + ITEM_MENU_SIZE_HEIGHT / 1.5f;
    exitMenuRec.width = MENU_SIZE_WIDTH;
    exitMenuRec.height = ITEM_MENU_SIZE_HEIGHT;
    // Add pause button
    pauseMenuRec.x = (float)GetScreenWidth() - 40.0f;
    pauseMenuRec.y = (float)GetScreenHeight() - 40.0f;
    // TODO: remove magic numbers later
    pauseMenuRec.width = 32;
    pauseMenuRec.height = 32;
}

bool CreateGameManager(void)
{

#ifdef IS_ANDROID
   SetOnPauseCallBack(&OnPause);
   SetOnResumeCallBack(&OnResume);
   InitCallBacks();
#endif
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
    PlaceUIButtons();
    Image pauseImage = GenImageColor(16, 16, BLANK);
    ImageDrawRectangleV(&pauseImage, (Vector2){4,0}, (Vector2){2,16}, WHITE);
    ImageDrawRectangleV(&pauseImage, (Vector2){10,0}, (Vector2){2,16}, WHITE);
    ImageResize(&pauseImage, 32, 32);
    pauseTexture = LoadTextureFromImage(pauseImage);
    UnloadImage(pauseImage);
    isGameOnScreen = true;
    previousFrameIsGameOnScreen = false;

    return true;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
bool UpdateDrawFrame(void)
{
    if (!isGameOnScreen) return true;
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
    if (!previousFrameIsGameOnScreen)
    {
        delta = 0.0f;
        previousFrameIsGameOnScreen = true;
    }

    // Ignore delta that are not valid
    if (delta >= 1.0f)
    {
        delta = 0.0f;
    }

    switch (gameState)
    {
        case MAIN_MENU:
            BeginDrawing();
                ClearBackground(BLACK);

                if (MenuButton(startMenuRec, "Start Game"))
                {
                    // Initialize game
                    gameState = IN_GAME;
                }
                if (MenuButton(exitMenuRec, "Exit Game")){
                    // Exit game
                    gameState = QUIT;
                    return false;
                }
                DrawFPS(10, 10);
            EndDrawing();
            break;
        case IN_GAME:
            if (IsWindowFocused())
            {
                gameState = UpdateInGame(gameState, scaleRatio, delta);

                BeginDrawing();
                    ClearBackground(BLACK);
                    DrawInGame(gameState, scaleRatio);
                    DrawRectangleRec((Rectangle){0, 0, virtualLeftBorder, (float)GetScreenHeight()}, BLACK);
                    DrawRectangleRec((Rectangle){virtualRightBorder, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, BLACK);
                    if (MenuButtonWithTexture(pauseMenuRec, pauseTexture))
                    {
                        gameState = PAUSE;
                    }
                    DrawFPS(10, 10);
                EndDrawing();
            }
            else
            {
                gameState = PAUSE;
            }
            break;
        case PAUSE:
            UpdateInGame(gameState, scaleRatio, delta);

            BeginDrawing();
                ClearBackground(BLACK);
                const Rectangle tempPauseRec = (Rectangle){restartMenuRec.x, restartMenuRec.y-restartMenuRec.height-15, restartMenuRec.width, restartMenuRec.height};
                if (MenuButton(tempPauseRec, "Continue Game"))
                {
                    gameState = IN_GAME;
                }
                if (MenuButton(restartMenuRec, "Restart Game"))
                {
                    RestartInGame();
                    gameState = IN_GAME;
                }
                if (MenuButton(exitMenuRec, "Exit Game"))
                {
                    gameState = QUIT;
                }
                DrawFPS(10, 10);
            EndDrawing();
            break;
        case GAME_OVER:
            BeginDrawing();
                ClearBackground(BLACK);
                if (MenuButton(restartMenuRec, "Restart Game"))
                {
                    // ResetGame(); // TODO: need to be implemented
                    gameState = IN_GAME;
                }
                if (MenuButton(exitMenuRec, "Exit Game"))
                {
                    gameState = QUIT;
                }           
                DrawFPS(10, 10);
            EndDrawing();
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
    UnloadTexture(pauseTexture);
}
#endif //GAMEMANAGER_H