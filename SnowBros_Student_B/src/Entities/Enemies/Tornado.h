#pragma once
#include "Fooga.h"

class Tornado : public Fooga {
protected:
    sf::Texture spin[8];
    int spinFrames;

public:
    Tornado();

    void init();
    void movement_spin(Tiles* tiles, int tileCount);
};
