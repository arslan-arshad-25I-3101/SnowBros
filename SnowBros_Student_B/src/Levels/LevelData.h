#pragma once

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;
using namespace sf;

struct Level {
    static const int ROWS = 14;
    static const int COLS = 16;
    static const float TILE_W;
    static const float TILE_H;

    string backgroundPath;
    IntRect bgRect;
    int grid[ROWS][COLS] = { 0 };
};

void SetupLevel1(Level& level);
void SetupLevel2(Level& level);
void SetupLevel3(Level& level);
void SetupLevel4(Level& level);
void SetupLevel5(Level& level);
void SetupLevel6(Level& level);
void SetupLevel7(Level& level);
void SetupLevel8(Level& level);
void SetupLevel9(Level& level);
void SetupLevel10(Level& level);
