#ifndef PLAYER_H
#define PLAYER_H

#ifdef IS_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

typedef struct Player{
    Vector2 position; 
    Texture2D spaceship;
    Rectangle spaceshipSourceRec;
    Rectangle spaceshipRec;
    float playerGunCDDefault;
    float playerSecondaryGunCDDefault;
    float playerGunCD;
    float playerSecondaryGunCD;
    float speed;
    float leftLimit;
    float rightLimit;
} Player;

Player CreatePlayer(float leftLimit, float rightLimit)
{
    Player player = {0};
    player.position.x = (float)GetScreenWidth() * 0.5f;
    player.position.y = (float)GetScreenHeight() * .8f;

    Image spaceship_image = GenImageColor(16, 16, BLANK);

    // Draw spaceship Base
    ImageDrawRectangleV(&spaceship_image, (Vector2){5, 6}, (Vector2){6, 8}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, (Vector2){4, 8}, (Vector2){1, 4}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, (Vector2){11, 8}, (Vector2){1, 4}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, (Vector2){7, 7}, (Vector2){2, 5}, GRAY);
    ImageDrawPixelV(&spaceship_image, (Vector2){5, 10}, GRAY);
    ImageDrawPixelV(&spaceship_image, (Vector2){10, 10}, GRAY);
    ImageDrawPixelV(&spaceship_image, (Vector2){7, 5}, LIGHTGRAY);
    ImageDrawPixelV(&spaceship_image, (Vector2){8, 5}, LIGHTGRAY);

    // Draw Propeller
    ImageDrawRectangleV(&spaceship_image, (Vector2){4, 13}, (Vector2){2, 2}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, (Vector2){10, 13}, (Vector2){2, 2}, LIGHTGRAY);
    ImageDrawRectangleV(&spaceship_image, (Vector2){4, 15}, (Vector2){2, 1}, DARKGRAY);
    ImageDrawRectangleV(&spaceship_image, (Vector2){10, 15}, (Vector2){2, 1}, DARKGRAY);

    Image propeller_image_left = GenImageColor(16, 19, BLANK);
    ImageDrawPixelV(&propeller_image_left, (Vector2){4, 16}, LIGHTGRAY);
    ImageDrawPixelV(&propeller_image_left, (Vector2){5, 17}, LIGHTGRAY);
    ImageDrawPixelV(&propeller_image_left, (Vector2){4, 18}, LIGHTGRAY);
    Image propeller_image_right = GenImageColor(16, 19, BLANK);
    ImageDrawPixelV(&propeller_image_right, (Vector2){10, 16}, LIGHTGRAY);
    ImageDrawPixelV(&propeller_image_right, (Vector2){11, 17}, LIGHTGRAY);
    ImageDrawPixelV(&propeller_image_right, (Vector2){10, 18}, LIGHTGRAY);

    // Draw spaceship Gun
    ImageDrawPixelV(&spaceship_image, (Vector2){ 5, 5 }, BLACK);
    ImageDrawPixelV(&spaceship_image, (Vector2){ 10, 5 }, BLACK);

    player.spaceship = LoadTextureFromImage(spaceship_image);

    UnloadImage(spaceship_image);

    player.spaceshipSourceRec = (Rectangle){.0f, .0f, (float)player.spaceship.width, (float)player.spaceship.height};
    player.playerGunCDDefault = 0.5f;
    player.playerSecondaryGunCDDefault = 1.4f;
    player.playerGunCD = player.playerGunCDDefault;
    player.playerSecondaryGunCD = player.playerSecondaryGunCDDefault;
    player.leftLimit = leftLimit;
    player.rightLimit = rightLimit;
    player.speed = 100.0f;

    return player;
}

void UpdatePlayer(Player *player, float delta)
{

#ifdef IS_ANDROID
    if (GetTouchPointCount() > 0) 
    {
        player->position.x = (float)GetTouchX();
        player->position.y = (float)GetTouchY();
    } 
#else
    if(IsWindowFocused())
    {
        player->position = GetMousePosition();
    }
#endif
    if (player->position.x < player->leftLimit)
    {
        player->position.x = player->leftLimit;
    }
    else if (player->position.x > player->rightLimit)
    {
        player->position.x = player->rightLimit;
    }

    if (player->position.y < 0.0f)
    {
        player->position.y = 0.0f;
    }
    else if (player->position.y > (float)GetScreenHeight())
    {
        player->position.y = (float)GetScreenHeight();
    }

    // Limit mouse to the borders
    SetMousePosition(player->position.x, player->position.y);

    player->playerGunCD -= delta;
    player->playerSecondaryGunCD -= delta;
}

void DrawPlayer(Player player, float scaleRatio)
{
    player.spaceshipRec = (Rectangle){
        player.position.x, 
        player.position.y, 
        (float)player.spaceship.width * scaleRatio * 4, 
        (float)player.spaceship.height * scaleRatio * 4
    };
    DrawTexturePro(
        player.spaceship, 
        player.spaceshipSourceRec, 
        player.spaceshipRec, 
        (Vector2){player.spaceshipRec.width * .5f, 
            player.spaceshipRec.height * .5f
        }, 
        .0f, 
        WHITE );
}

void DestroyPlayer(Player player)
{
    UnloadTexture(player.spaceship);
}

#endif //PLAYER_H