#include "pch.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <ctime>
#include <iostream>
#include "Game.h"
#if defined(DEBUG) | defined(_DEBUG)
#include <crtdbg.h>
#endif

static void StartHeapControl()
{
#if defined(DEBUG) | defined(_DEBUG)
    // Notify user if heap is corrupt
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    // Report detected leaks when the program exits
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Set a breakpoint on the specified object allocation order number
    //_CrtSetBreakAlloc( 156 );
#endif
}

static void DumpMemoryLeaks()
{
#if defined(DEBUG) | defined(_DEBUG)
    _CrtDumpMemoryLeaks();
#endif
}

int main(int argc, char* argv[])
{
    srand(static_cast<unsigned int>(time(nullptr)));

    StartHeapControl();


    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }


    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0) {
        std::cerr << "SDL_GetDesktopDisplayMode failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }


    float screenWidth = static_cast<float>(displayMode.w);
    float screenHeight = static_cast<float>(displayMode.h);

    std::cout << "Detected screen resolution: " << screenWidth << "x" << screenHeight << std::endl;

    Game* pGame{ new Game{ Window{ "Tower Defence - Meyen, Xander - 1DAE29", screenWidth, screenHeight } } };
    pGame->Run();
    delete pGame;

    SDL_Quit();
    DumpMemoryLeaks();
    return 0;
}