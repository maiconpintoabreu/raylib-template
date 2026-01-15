#ifdef IS_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

#include "game_manager.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void UpdateWebFrame(void)
{
    if(!UpdateDrawFrame())
    {

#if defined(PLATFORM_WEB)
        emscripten_cancel_main_loop();
#endif
 
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    CreateGameManager();

    //--------------------------------------------------------------------------------------

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateWebFrame, 0, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && UpdateDrawFrame()) {}
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    DestroyGameManager();
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}