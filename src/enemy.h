#ifndef ASTEROID_H
#define ASTEROID_H

#ifdef IS_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

#include "raymath.h"

typedef struct Enemy{
    Vector2 position; 
    Vector2 positionOffset; 
    Vector2 acceleration; 
    Texture2D texture;
    Rectangle sourceRec;
    float rotationSpeed;
    float rotation;
    int health;
    int size;
} Enemy;

Enemy CreateEnemy(Texture2D preLoadedTexture)
{
    Enemy enemy = {0};

    enemy.texture = preLoadedTexture;
    enemy.sourceRec = (Rectangle){.0f, .0f, (float)enemy.texture.width, (float)enemy.texture.height};
    enemy.acceleration.y = (float)GetRandomValue(-1000, 0);
    enemy.rotation = (float)GetRandomValue(-180, 180);
    enemy.size = 20;
    return enemy;
}

void UpdateEnemy(Enemy *enemy, float yOffset, float delta)
{
    enemy->position = Vector2Add(enemy->position, Vector2Scale(enemy->acceleration, delta));
    enemy->positionOffset.y  = enemy->position.y + yOffset;

    enemy->rotation += (10.0f*enemy->rotationSpeed)*delta;
    if (enemy->rotation > 180)
    {
        enemy->rotation -= 360;
    }
    if (enemy->rotation < -180)
    {
        enemy->rotation += 360;
    }
}

void DrawEnemy(Enemy enemy, float scaleRatio)
{
    Rectangle destRec = (Rectangle){
        enemy.positionOffset.x, 
        enemy.positionOffset.y, 
        (float)enemy.texture.width * scaleRatio * 4, 
        (float)enemy.texture.height * scaleRatio * 4
    };
    // TODO: remove rotationSpeed later
    DrawTexturePro(
        enemy.texture, 
        enemy.sourceRec, 
        destRec, 
        (Vector2){destRec.width * .5f, 
            destRec.height * .5f
        }, 
        enemy.rotation, 
        WHITE );
}
#endif // ASTEROID_H