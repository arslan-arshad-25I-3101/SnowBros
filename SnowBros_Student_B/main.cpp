#include "src/Game.h"
#include <cstdlib>
#include <ctime>


int main()
{
    
    Game game;

    
    while (game.IsRunning())
    {
        if (!game.Update())
            break;

        game.Render();
    }

    return 0;
}
