#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#ifdef IS_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

#include "raymath.h"
#include "player.h"
#include "resource_loader.h"
#include "enums.h"
#include "config.h"


typedef struct Bullet{
    Vector2 position;       // Bullet position on screen
    Vector2 acceleration;
    bool isPlayer;
} Bullet;

typedef struct Asteroid{
    Vector2 position; 
    Vector2 positionOffset; 
    Vector2 acceleration; 
    float rotationSpeed; 
} Asteroid;

typedef struct Entity{
    EntityType type;
    bool isAlive; 
    union {
        Player player;
        Asteroid asteroid;
        Bullet bullet;
    };
} Entity;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
int screenWidth = 0;
int screenHeight = 0;
float scaleRatio = 1.0f;
float mapSize = 10000.0f;
float currentWorldOffset = 0.0f;

float virtualLeftBorder = 0.0f;
float virtualRightBorder = 0.0f;

int currentScore = 0;
int currentAsteroidToSpawn = 0;
int currentMaxAsteroids = 50;
int currentLevel = 1;

Entity entities[MAX_ENTITIES] = {0};

// GUN
float delta = 0.0f;
bool gameOver = false;

LevelData* levelDataLoaded = NULL;

bool CreateGameManager(void)
{

    // Initialization
    //--------------------------------------------------------------------------------------

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
        virtualLeftBorder = (float)GetScreenWidth() * 0.5f - (225.0f * scaleRatio);
        virtualRightBorder = (float)GetScreenWidth() * 0.5f + (225.0f * scaleRatio);
    }

    DisableCursor(); // Lock mouse to window center

    // Initialize Player
    Player player = CreatePlayer(virtualLeftBorder, virtualRightBorder);

    entities[0].isAlive = true;
    entities[0].type = PLAYER;
    entities[0].player = player;

    // GUN
    delta = 0.0f;
    // float physicDelta = 0.0f;
    gameOver = false;
    levelDataLoaded = LoadResources();

    return true;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
bool UpdateDrawFrame(void)
{
    // Update
    //----------------------------------------------------------------------------------
    if (!gameOver)
    {
        if (IsWindowResized())
        {
            if (GetScreenWidth() < GetScreenHeight())
            {
                scaleRatio = (float)GetScreenWidth() / 450.0f;
            }
            else 
            {
                scaleRatio = (float)GetScreenHeight() / 800.0f;
                virtualLeftBorder = (float)GetScreenWidth() * 0.5f - (225.0f * scaleRatio);
                virtualRightBorder = (float)GetScreenWidth() * 0.5f + (225.0f * scaleRatio);
            }
        }

        delta = GetFrameTime();
        // physicDelta += delta;
        currentWorldOffset += entities[0].player.speed * delta;
        // TraceLog(LOG_INFO, "World Offset: %3.3f", currentWorldOffset);

        if (mapSize < currentWorldOffset)
        {
            gameOver = true;
            return false;
        }

        for (int i = 0; i < MAX_LEVEL_DATA; i++)
        {
            if (!levelDataLoaded[i].isLoaded || levelDataLoaded[i].level != currentLevel) break;
            if (levelDataLoaded[i].isSpawned) continue;
            if (levelDataLoaded[i].whenSpawn < currentWorldOffset)
            {
                levelDataLoaded[i].isSpawned = true;
                int indexToUse = -1;
                
                for (int i = 0; i < MAX_ENTITIES; i++) 
                {
                    if (!entities[i].isAlive)
                    {
                        indexToUse = i;
                        break;
                    }
                }
                
                if (indexToUse == -1)
                {
                    TraceLog(LOG_ERROR, "No space on Entities");
                    return false;
                }
                Vector2 positionToSpawn = {0};
                Asteroid asteroid = {0};
                float asteroidX = 0.0f;
                switch (levelDataLoaded[i].entityType) {
                    case ASTEROID:
                        asteroidX = (float)GetScreenWidth()*0.5f;
                        asteroidX += asteroidX*levelDataLoaded[i].whereToSpawnX;
                        positionToSpawn = (Vector2){
                            asteroidX, 
                            levelDataLoaded[i].whereToSpawnY - currentWorldOffset
                        };
                        asteroid = (Asteroid){
                            positionToSpawn, 
                            (Vector2){positionToSpawn.x, positionToSpawn.y + currentWorldOffset}, 
                            (Vector2){0, 0}, 
                            1.0f
                        };
                        entities[indexToUse].isAlive = true;
                        entities[indexToUse].type = ASTEROID;
                        entities[indexToUse].asteroid = asteroid;
                    break;
                    case PLAYER:
                        TraceLog(LOG_ERROR, "Trying to Spawn Player");
                        return false;
                    break;
                    default:
                        TraceLog(LOG_ERROR, "Trying to Spawn wrong stuff");
                        return false;
                    break;
                }
            }
            else
            {
                break; // Stop for to avoid useless computation
            }
        }

        for (int i = 0; i < MAX_ENTITIES; i++) 
        {
            if (entities[i].isAlive)
            {
                switch (entities[i].type) {
                    case ASTEROID: 
                        entities[i].asteroid.position = Vector2Add(entities[i].asteroid.position, Vector2Scale(entities[i].asteroid.acceleration, delta));
                        entities[i].asteroid.positionOffset.y  = entities[i].asteroid.position.y + currentWorldOffset;
                        if (entities[i].asteroid.positionOffset.y >= GetScreenHeight())
                        {
                            entities[i].isAlive = false;
                        }
                        break;
                    case BULLET: 
                        entities[i].bullet.position = Vector2Add(entities[i].bullet.position, Vector2Scale(entities[i].bullet.acceleration, delta));
                        if (entities[i].bullet.position.y <= 0)
                        {
                            entities[i].isAlive = false;
                        }
                        break;
                    case PLAYER: 
                        UpdatePlayer(&entities[i].player, delta);
                        break;
                }
            }
        }

        // Player Autoshooting
        if (entities[0].player.playerGunCD < 0.0f)
        {
            entities[0].player.playerGunCD = entities[0].player.playerGunCDDefault;

            // Find disabled bullet to use
            for (int i = 0; i < MAX_ENTITIES; i++) 
            {
                if (!entities[i].isAlive)
                {
                    entities[i].isAlive = true;
                    entities[i].type = BULLET;
                    entities[i].bullet.isPlayer = true;
                    entities[i].bullet.position = entities[0].player.position;
                    entities[i].bullet.acceleration = (Vector2){0, -1000};
                    break;
                }
            }

        }

        if (entities[0].player.playerSecondaryGunCD < 0.0f)
        {
            entities[0].player.playerSecondaryGunCD = entities[0].player.playerSecondaryGunCDDefault;
            int secondaryGunBulletCount = 0;
            // Find disabled bullet to use
            for (int i = 0; i < MAX_ENTITIES; i++) 
            {
                if (!entities[i].isAlive)
                {
                    entities[i].isAlive = true;
                    entities[i].type = BULLET;
                    entities[i].bullet.isPlayer = true;
                    if (secondaryGunBulletCount == 0)
                    {
                        entities[i].bullet.position = entities[0].player.position;
                        entities[i].bullet.position.x -= 10 * scaleRatio;
                    }
                    else {
                        entities[i].bullet.position = entities[0].player.position;
                        entities[i].bullet.position.x += 10 * scaleRatio;
                    }
                    entities[i].bullet.acceleration = (Vector2){0, -1000};
                    secondaryGunBulletCount += 1;
                    if(secondaryGunBulletCount >= 2)
                    {
                        break;
                    }
                }
            }
        }

        // Physics
        for (int i = 0; i < MAX_ENTITIES; i++) 
        {
            if (entities[i].isAlive)
            {
                for (int j = 0; j < MAX_ENTITIES; j++) 
                {
                    if (entities[j].isAlive)
                    {
                        switch (entities[i].type) {
                            case ASTEROID: 
                                if (entities[j].type == PLAYER)
                                {
                                    if (CheckCollisionCircles(
                                        entities[j].player.position, 20 * scaleRatio, 
                                        entities[i].asteroid.positionOffset, 20 * scaleRatio)) 
                                    {
                                        entities[i].isAlive = false;
                                        gameOver = true;
                                        break;
                                    }
                                    
                                }
                                break;
                            case BULLET: 
                                if (entities[i].bullet.isPlayer && entities[j].type == ASTEROID)
                                {
                                    if (CheckCollisionCircles(
                                        entities[i].bullet.position, 5 * scaleRatio, 
                                        entities[j].asteroid.positionOffset, 20 * scaleRatio)) 
                                    {
                                        entities[i].isAlive = false;
                                        entities[j].isAlive = false;
                                        currentScore += 2;
                                    }
                                } 
                                else if (!entities[i].bullet.isPlayer && entities[j].type == PLAYER)
                                {
                                    if (CheckCollisionCircles(
                                        entities[i].bullet.position, 5 * scaleRatio, 
                                        entities[j].player.position, 20 * scaleRatio)) 
                                    {
                                        entities[i].isAlive = false;
                                        gameOver = true;
                                        break;
                                    }
                                }
                                break;
                            case PLAYER: 
                                break;
                        }
                    }
                }
            }
        }
    }

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(DARKGRAY);
    if (!gameOver)
    {
        for (int i = 0; i < MAX_ENTITIES; i++) 
        {
            if (entities[i].isAlive)
            {
                switch (entities[i].type) {
                    case ASTEROID: 
                        TraceLog(LOG_INFO, "Drawing Asteroid on: \{%3.3f, %3.3f}", entities[i].asteroid.positionOffset.x, entities[i].asteroid.positionOffset.y);
                        DrawCircleV(entities[i].asteroid.positionOffset, 20 * scaleRatio, BROWN);
                        break;
                    case BULLET: 
                        DrawCircleV(entities[i].bullet.position, 5 * scaleRatio, YELLOW);
                        break;
                    default:
                        break;
                }
            }
        }
        for (int i = 0; i < MAX_ENTITIES; i++) 
        {
            if (entities[i].isAlive)
            {
                switch (entities[i].type) {
                    case PLAYER: 
                        TraceLog(LOG_INFO, "Drawing Player on: \{%3.3f, %3.3f}", entities[i].player.position.x, entities[i].player.position.y);
                        DrawPlayer(entities[i].player, scaleRatio);
                        break;
                    default:
                        break;
                }
            }
        }
        // DrawText(TextFormat("FPS: %i", GetFPS()), virtualLeftBorder + 10 * scaleRatio, (int)((float)GetScreenHeight() * .1f), (int)(20.0f * scaleRatio), RED);
        DrawText(TextFormat("Score: %i", currentScore), virtualLeftBorder + 10 * scaleRatio, (int)((float)GetScreenHeight() * .1f), (int)(20.0f * scaleRatio), GREEN);

        DrawRectangleRec((Rectangle){0, 0, virtualLeftBorder, GetScreenHeight()}, BLACK);
        DrawRectangleRec((Rectangle){virtualRightBorder, 0, GetScreenWidth(), GetScreenHeight()}, BLACK);
        DrawFPS(10, 10);
    }
    else 
    {
        DrawText("Game Over", 20, 20, 30, RED);
    }
    EndDrawing();
    //----------------------------------------------------------------------------------

    return true;
}

void DestroyGameManager(void)
{
    DestroyPlayer(entities[0].player);
}
#endif //GAMEMANAGER_H