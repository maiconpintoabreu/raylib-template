#ifndef ASTEROID_H
#define ASTEROID_H

#ifdef IS_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

#include "raymath.h"

typedef struct Asteroid{
    Vector2 position; 
    Vector2 positionOffset; 
    Vector2 acceleration; 
    Texture2D texture;
    Rectangle sourceRec;
    float rotationSpeed;
    float rotation;
} Asteroid;

Asteroid CreateAsteroid(Texture2D preLoadedTexture)
{
    Asteroid asteroid = {0};

    asteroid.texture = preLoadedTexture;
    asteroid.sourceRec = (Rectangle){.0f, .0f, (float)asteroid.texture.width, (float)asteroid.texture.height};
    asteroid.rotation = (float)GetRandomValue(-180, 180);
    return asteroid;
}

void UpdateAsteroid(Asteroid *asteroid, float yOffset, float delta)
{
    asteroid->position = Vector2Add(asteroid->position, Vector2Scale(asteroid->acceleration, delta));
    asteroid->positionOffset.y  = asteroid->position.y + yOffset;

    asteroid->rotation += (10.0f*asteroid->rotationSpeed)*delta;
    if (asteroid->rotation > 180)
    {
        asteroid->rotation -= 360;
    }
    if (asteroid->rotation < -180)
    {
        asteroid->rotation += 360;
    }
}

void DrawAsteroid(Asteroid asteroid, float scaleRatio)
{
    Rectangle destRec = (Rectangle){
        asteroid.positionOffset.x, 
        asteroid.positionOffset.y, 
        (float)asteroid.texture.width * scaleRatio * 4, 
        (float)asteroid.texture.height * scaleRatio * 4
    };
    // TODO: remove rotationSpeed later
    DrawTexturePro(
        asteroid.texture, 
        asteroid.sourceRec, 
        destRec, 
        (Vector2){destRec.width * .5f, 
            destRec.height * .5f
        }, 
        asteroid.rotation, 
        WHITE );
}
#endif // ASTEROID_H