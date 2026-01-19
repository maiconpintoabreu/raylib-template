#ifndef IN_GAME_H
#define IN_GAME_H

#include <stdbool.h>
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
#include "enemy.h"
#include "enums.h"
#include "config.h"
#include "resource_loader.h"

typedef struct Level {
    char *name;
    int id;
    int difficult;
    int targetAmount;
    int scoreAmount;
    float size;
} Level;

typedef struct Star {
    Vector2 position;
    float size;
    float speed;
    bool IsOnScreen;
} Star;

typedef struct Particles {
    Vector2 position;
    Vector2 acceleration;
    Color color;
    float size;
    float maxLifeTime;
    float lifeTime;
    bool isAlive;
} Particles;

typedef struct Bullet{
    Vector2 position;       // Bullet position on screen
    Vector2 acceleration;
    bool isPlayer;
    int damage;
} Bullet;

typedef struct Entity{
    EntityType type;
    bool isAlive;
    union {
        Player player;
        Enemy enemy;
        Bullet bullet;
    };
} Entity;

Entity entities[MAX_ENTITIES] = {0};
Star stars[STARS_AMOUNT] = {0};
Particles particles[MAX_PARTICLES] = {0};

Level currentLevel = {0};
float currentWorldOffset = 0.0f;

// TODO: Move it somewhere else
Shader trailShader = {0};
Texture2D tailTexture = {0};
Texture2D asteroidTexture = {0};

LevelData* levelDataLoaded = NULL;

int currentScore = 0;

void RestartInGame()
{
    RestartPlayer(&entities[0].player);
    for (int i=1; i<MAX_ENTITIES; i++)
    {
        entities[i].isAlive = false;
    }
    for (int i=0; i<MAX_LEVEL_DATA; i++)
    {
        levelDataLoaded[i].isSpawned = false;
    }
    currentWorldOffset = 0.0f;
}

bool CreateInGame(Level level, float scaleRatio, float virtualLeftBorder, float virtualRightBorder)
{
    currentLevel = level;
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
    return true;
}

GameState UpdateInGame(GameState gameState, float scaleRatio, float delta)
{

    // physicDelta += delta;
    currentWorldOffset += entities[0].player.speed*delta;
    if (currentLevel.size < currentWorldOffset)
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
        // TODO: add levelDataLoaded[i].level != currentLevel.id when have more levels
        if (!levelDataLoaded[i].isLoaded) break;
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
            Enemy enemy = {0};
            float enemyX = 0.0f;
            switch (levelDataLoaded[i].entityType) {
                case ASTEROID:
                    enemyX = (float)GetScreenWidth()*0.5f;
                    enemyX += enemyX*levelDataLoaded[i].whereToSpawnX*scaleRatio;
                    positionToSpawn = (Vector2){
                        enemyX,
                        levelDataLoaded[i].whereToSpawnY*scaleRatio - currentWorldOffset
                    };
                    enemy = CreateEnemy(asteroidTexture);
                    enemy.position = positionToSpawn; 
                    enemy.positionOffset = (Vector2){positionToSpawn.x, positionToSpawn.y + currentWorldOffset};
                    enemy.acceleration = (Vector2){0, 0};
                    enemy.rotationSpeed = (float)GetRandomValue(-1, 1) * 2; // TODO: this value should be fixed
                    enemy.health = (int)(levelDataLoaded[i].health * ((float)currentLevel.difficult / levelDataLoaded[i].health));

                    entities[indexToUse].isAlive = true;
                    entities[indexToUse].type = ASTEROID;
                    entities[indexToUse].enemy = enemy;
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

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        if (particles[i].isAlive)
        {
            particles[i].lifeTime -= delta;
            if (particles[i].lifeTime <=0.0f)
            {
                particles[i].isAlive = false;
                continue;
            }

            particles[i].position = Vector2Add(particles[i].position, Vector2Scale(particles[i].acceleration, delta));
        }
    }

    for (int i = 0; i < MAX_ENTITIES; i++) 
    {
        if (entities[i].isAlive)
        {
            switch (entities[i].type) {
                case ASTEROID: 
                    UpdateEnemy(&entities[i].enemy, currentWorldOffset, delta);

                    if (entities[i].enemy.positionOffset.y >= (float)GetScreenHeight())
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
                entities[i].bullet.damage = 1;
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
                entities[i].bullet.damage = 1;
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
                                    entities[j].player.position, (float)entities[j].player.size*scaleRatio,
                                    entities[i].enemy.positionOffset, (float)entities[j].enemy.size*scaleRatio))
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
                                    entities[j].enemy.positionOffset, (float)entities[j].enemy.size*scaleRatio))
                                {
                                    entities[i].isAlive = false;
                                    entities[j].enemy.health -= entities[i].bullet.damage;
                                    int howManyParticles = 5;
                                    for (int n = 0;n < MAX_PARTICLES; n++)
                                    {
                                        if (!particles[n].isAlive)
                                        {
                                            particles[n].isAlive = true;
                                            particles[n].maxLifeTime = 0.5f;
                                            particles[n].position = entities[i].bullet.position;
                                            particles[n].size = 5*scaleRatio;
                                            particles[n].lifeTime = particles[n].maxLifeTime;
                                            particles[n].acceleration.x = (float)GetRandomValue(-100, 100) ;
                                            particles[n].acceleration.y = (float)GetRandomValue(200, 320);
                                            particles[n].color = BROWN;
                                            howManyParticles -= 1;
                                            if (howManyParticles <= 0)
                                            {
                                                break;
                                            }
                                        }
                                    }
                                    if (entities[j].enemy.health <= 0)
                                    {
                                        entities[j].isAlive = false;
                                        currentScore += 2;
                                    }
                                }
                            } 
                            else if (!entities[i].bullet.isPlayer && entities[j].type == PLAYER)
                            {
                                if (CheckCollisionCircles(
                                    entities[i].bullet.position, BULLET_SIZE*scaleRatio, 
                                    entities[j].player.position, (float)entities[j].player.size*scaleRatio))
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
        for (int i = 1; i < MAX_ENTITIES; i++)
        {
            if (entities[i].isAlive)
            {
                switch (entities[i].type) {
                    case ASTEROID: 
                        DrawEnemy(entities[i].enemy, scaleRatio);
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
        if (entities[0].isAlive)
        {
            switch (entities[0].type) {
                case PLAYER:
//                        TraceLog(LOG_INFO, "Drawing Player on: \{%3.3f, %3.3f}", entities[i].player.position.x, entities[i].player.position.y);
                    DrawPlayer(entities[0].player, scaleRatio);
                    break;
                default:
                    break;
            }
        }
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (particles[i].isAlive)
            {
                Color particleColor =  Fade(particles[i].color, particles[i].lifeTime / particles[i].maxLifeTime);
                DrawCircleV(particles[i].position, particles[i].size, particleColor);
            }
        }
        // DrawText(TextFormat("FPS: %i", GetFPS()), virtualLeftBorder + 10*scaleRatio, (int)((float)GetScreenHeight()*.1f), (int)(20.0f*scaleRatio), RED);
        DrawText(TextFormat("Level: %i", currentLevel.id), 10, 40, 20, GREEN);
        DrawText(TextFormat("offsetY: %3.3f", currentWorldOffset), 10, 80, 20, GREEN);

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