#pragma once
#include "Botom.h"

class Fooga : public Botom {
protected:
    bool isFlying = false;
    float velocity_X = 0.125f;

public:
    Fooga();

    void init();
    void movement(Tiles* tiles, int tileCount) override;
};
