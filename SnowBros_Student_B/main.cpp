/**
 * @file main.cpp
 * @brief Entry point for Snow Bros game
 * 
 * Object-Oriented Programming Project - CS1004
 * Snow Bros Game Implementation
 */

#include "src/Game.h"
#include <cstdlib>
#include <ctime>

/**
 * @brief Main entry point
 * @return Exit code (0 for success)
 */
int main()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    Game game;

    // Main game loop
    while (game.IsRunning())
    {
        if (!game.Update())
            break;

        game.Render();
    }

    return 0;
}
