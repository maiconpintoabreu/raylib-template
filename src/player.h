#ifndef PLAYER_H
#define PLAYER_H

#ifdef IS_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

#include "raymath.h"

typedef struct Player{
    Vector2 startMovingPoint;
    Vector2 currentMovingPoint;
    Vector2 position;
    Texture2D spaceship;
    Rectangle spaceshipSourceRec;
    Rectangle spaceshipRec;
    float playerGunCDDefault;
    float playerSecondaryGunCDDefault;
    float playerGunCD;
    float playerSecondaryGunCD;
    float speed;
    float maxMovementSpeed;
    float currentMovementSpeed;
    float leftLimit;
    float rightLimit;
    int size;
    bool isMoving;
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
    player.maxMovementSpeed = 300.0f;
    player.isMoving = false;
    player.size = 20;

    return player;
}

void UpdatePlayer(Player *player, float delta)
{
   #ifdef IS_ANDROID
    if (GetTouchPointCount() > 0) 
    {
        Vector2 currentTouch = GetTouchPosition(0);

        if (!player->isMoving)
        {
            // First frame of touch: just sync the positions
            player->isMoving = true;
            player->currentMovingPoint = currentTouch;
        }
        else
        {
            // Calculate how far the finger moved since the last frame
            Vector2 touchDelta = Vector2Subtract(currentTouch, player->currentMovingPoint);

            // Apply that exact delta to the ship's position
            player->position = Vector2Add(player->position, touchDelta);

            // Store the current touch for the next frame's calculation
            player->currentMovingPoint = currentTouch;
        }
    } 
    else 
    {
        player->isMoving = false;
    }
#else
    // Keyboard logic remains velocity-based
    Vector2 direction = { 0, 0 };
    if (IsKeyDown(KEY_LEFT))  direction.x -= 1.0f;
    if (IsKeyDown(KEY_RIGHT)) direction.x += 1.0f;
    if (IsKeyDown(KEY_UP))    direction.y -= 1.0f;
    if (IsKeyDown(KEY_DOWN))  direction.y += 1.0f;

    if (Vector2Length(direction) > 0) 
    {
        direction = Vector2Normalize(direction);
        Vector2 velocity = Vector2Scale(direction, player->maxMovementSpeed * delta);
        player->position = Vector2Add(player->position, velocity);
    }
#endif

    // Always clamp at the end to keep the ship on screen
    player->position.x = Clamp(player->position.x, player->leftLimit, player->rightLimit);
    player->position.y = Clamp(player->position.y, 0.0f, (float)GetScreenHeight());

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
    // DrawText(TextFormat("Speed: %3.3f",player.currentMovementSpeed), 10, 30, 20, WHITE);
}

void DestroyPlayer(Player player)
{
    UnloadTexture(player.spaceship);
}

#endif //PLAYER_H