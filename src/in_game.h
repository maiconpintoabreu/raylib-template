#ifndef IN_GAME_H
#define IN_GAME_H

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

#include "player.h"
#include "asteroid.h"
#include "enums.h"
#include "config.h"
#include "resource_loader.h"

typedef struct Star {
    Vector2 position;
    float size;
    float speed;
    bool IsOnScreen;
} Star;

typedef struct Bullet{
    Vector2 position;       // Bullet position on screen
    Vector2 acceleration;
    bool isPlayer;
} Bullet;

typedef struct Entity{
    EntityType type;
    bool isAlive;
    union {
        Player player;
        Asteroid asteroid;
        Bullet bullet;
    };
} Entity;

Entity entities[MAX_ENTITIES] = {0};
Star stars[STARS_AMOUNT] = {0};

float mapSize = 10000.0f;
float currentWorldOffset = 0.0f;

// TODO: Move it somewhere else
Shader trailShader = {0};
Texture2D tailTexture = {0};
Texture2D asteroidTexture = {0};

LevelData* levelDataLoaded = NULL;

int currentLevel = 1;
int currentScore = 0;

void SetLevel(int level)
{
    currentLevel = level;
}

bool CreateInGame(float scaleRatio, float virtualLeftBorder, float virtualRightBorder)
{
    Image trailImage = GenImageColor((int)(10*scaleRatio), (int)(100*scaleRatio), BLACK);
    tailTexture = LoadTextureFromImage(trailImage);
    UnloadImage(trailImage);
    trailShader = LoadShader(0, TextFormat("resources/shaders-%s/trail.fs",GLSL_VERSION));

    asteroidTexture = LoadTexture("resources/asteroid.png");

    // Initialize Player
    Player player = CreatePlayer(virtualLeftBorder, virtualRightBorder);

    entities[0].isAlive = true;
    entities[0].type = PLAYER;
    entities[0].player = player;

    levelDataLoaded = LoadResources();

    for (int i = 0; i < STARS_AMOUNT; i++) {
        stars[i].position = (Vector2){ 
            (float)GetRandomValue(0, GetScreenWidth()), 
            (float)GetRandomValue(0, GetScreenHeight()) 
        };
        stars[i].size = (float)GetRandomValue(1, 3);
        stars[i].speed = (float)GetRandomValue(10, 50) / 10.0f; // Optional for movement
        stars[i].IsOnScreen = true;
    }
    SetLevel(1);
    return true;
}

GameState UpdateInGame(GameState gameState, float scaleRatio, float delta)
{

    // physicDelta += delta;
    currentWorldOffset += entities[0].player.speed*delta;
    // TraceLog(LOG_INFO, "World Offset: %3.3f", currentWorldOffset);
    if (mapSize < currentWorldOffset)
    {
        return MAIN_MENU;
    }

    for (int i = 0; i < STARS_AMOUNT; i++) {
        stars[i].position.y += stars[i].speed*entities[0].player.speed*delta;
        if (stars[i].position.y >= (float)GetScreenHeight())
        {
            stars[i].position = (Vector2){ 
                (float)GetRandomValue(0, GetScreenWidth()), 
                0.0f 
            };
            stars[i].size = (float)GetRandomValue(1, 3);
            stars[i].speed = (float)GetRandomValue(10, 50)*0.1f; // Optional for movement
            stars[i].IsOnScreen = true;
        }
    }

    for (int i = 0; i < MAX_LEVEL_DATA; i++)
    {
        if (!levelDataLoaded[i].isLoaded || levelDataLoaded[i].level != currentLevel) break;
        if (levelDataLoaded[i].isSpawned) continue;
        if (levelDataLoaded[i].whenSpawn < currentWorldOffset)
        {
            levelDataLoaded[i].isSpawned = true;
            int indexToUse = -1;
            
            for (int j = 1; j < MAX_ENTITIES; j++)
            {
                if (!entities[j].isAlive)
                {
                    indexToUse = j;
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
                    asteroidX += asteroidX*levelDataLoaded[i].whereToSpawnX*scaleRatio;
                    positionToSpawn = (Vector2){
                        asteroidX,
                        levelDataLoaded[i].whereToSpawnY*scaleRatio - currentWorldOffset
                    };
                    asteroid = CreateAsteroid(asteroidTexture);
                    asteroid.position = positionToSpawn; 
                    asteroid.positionOffset = (Vector2){positionToSpawn.x, positionToSpawn.y + currentWorldOffset};
                    asteroid.acceleration = (Vector2){0, 0};
                    asteroid.rotationSpeed = (float)GetRandomValue(-1, 1)/2;
                    
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
                    UpdateAsteroid(&entities[i].asteroid, currentWorldOffset, delta);

                    if (entities[i].asteroid.positionOffset.y >= (float)GetScreenHeight())
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

    // Player Auto shooting
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
                entities[i].bullet.acceleration = (Vector2){0, -BULLET_SPEED};
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
                    entities[i].bullet.position.x -= 10*scaleRatio;
                }
                else {
                    entities[i].bullet.position = entities[0].player.position;
                    entities[i].bullet.position.x += 10*scaleRatio;
                }
                entities[i].bullet.acceleration = (Vector2){0, -BULLET_SPEED*0.8f};
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
                                    entities[j].player.position, entities[j].player.size*scaleRatio, 
                                    entities[i].asteroid.positionOffset, entities[j].asteroid.size*scaleRatio)) 
                                {
                                    entities[i].isAlive = false;
                                    gameState = GAME_OVER;
                                    break;
                                }
                                
                            }
                            break;
                        case BULLET: 
                            if (entities[i].bullet.isPlayer && entities[j].type == ASTEROID)
                            {
                                if (CheckCollisionCircles(
                                    entities[i].bullet.position, BULLET_SIZE*scaleRatio, 
                                    entities[j].asteroid.positionOffset, entities[j].asteroid.size*scaleRatio)) 
                                {
                                    entities[i].isAlive = false;
                                    entities[j].isAlive = false;
                                    currentScore += 2;
                                }
                            } 
                            else if (!entities[i].bullet.isPlayer && entities[j].type == PLAYER)
                            {
                                if (CheckCollisionCircles(
                                    entities[i].bullet.position, BULLET_SIZE*scaleRatio, 
                                    entities[j].player.position, entities[j].player.size*scaleRatio)) 
                                {
                                    entities[i].isAlive = false;
                                    gameState = GAME_OVER;
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
    return gameState;
}
void DrawInGame(GameState gameState, float scaleRatio)
{
    if (gameState == IN_GAME)
    {
        for (int i = 0; i < STARS_AMOUNT; i++)
        {
            if (stars[i].IsOnScreen) DrawCircleV(stars[i].position, stars[i].size*scaleRatio, Fade(RAYWHITE, 0.5f));
        }
        for (int i = 0; i < MAX_ENTITIES; i++) 
        {
            if (entities[i].isAlive)
            {
                switch (entities[i].type) {
                    case ASTEROID: 
                        DrawAsteroid(entities[i].asteroid, scaleRatio);
                        break;
                    case BULLET: 
                        BeginBlendMode(BLEND_ALPHA);
                            BeginShaderMode(trailShader);
                                DrawTextureV(tailTexture, (Vector2){ entities[i].bullet.position.x - 5*scaleRatio, entities[i].bullet.position.y }, YELLOW);
                            EndShaderMode();
                        EndBlendMode();
                        DrawCircleV(entities[i].bullet.position, 5*scaleRatio, YELLOW);
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
//                        TraceLog(LOG_INFO, "Drawing Player on: \{%3.3f, %3.3f}", entities[i].player.position.x, entities[i].player.position.y);
                        DrawPlayer(entities[i].player, scaleRatio);
                        break;
                    default:
                        break;
                }
            }
        }
        // DrawText(TextFormat("FPS: %i", GetFPS()), virtualLeftBorder + 10*scaleRatio, (int)((float)GetScreenHeight()*.1f), (int)(20.0f*scaleRatio), RED);
        // DrawText(TextFormat("Score: %i", currentScore), (int)(virtualLeftBorder + 10.0f*scaleRatio), (int)((float)GetScreenHeight()*.1f), (int)(20.0f*scaleRatio), GREEN);

    }
    else 
    {
        DrawText("Game Over", 20, 20, 30, RED);
    }
}

void DestroyInGame(void)
{
    DestroyPlayer(entities[0].player);

    UnloadShader(trailShader);
    UnloadTexture(tailTexture);
    UnloadTexture(asteroidTexture);
}
#endif // IN_GAME_H