/**
 * @file main.cpp
 * @brief Entry point for Snow Bros game
 * 
 * Object-Oriented Programming Project - CS1004
 * Snow Bros Game Implementation
 */

#include "src/Game.h"

/**
 * @brief Main entry point
 * @return Exit code (0 for success)
 */
int main()
{
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
