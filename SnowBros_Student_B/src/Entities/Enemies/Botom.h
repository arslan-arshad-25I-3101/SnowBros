#pragma once
#include "../Enemy.h"
#include "../../Levels/Tiles.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

class Botom : public Enemy {
public:
    using Enemy::setPos; // Bring all overloads into scope
protected:
    bool mv1;
    sf::Texture tex[6];
    sf::Texture spriteSheet;
    sf::Clock clocks;
    int frames;
    float frametime;
    float speed;
    bool onUpper;
    bool anti;
    int pausef;

public:
    enum GravityState { NORMAL, JUMPING_UP };
    GravityState currentState;

    Botom();

    void init();
    bool checkCollision(Tiles* tiles, int tileCount);
    float distanceChecker(Tiles* tiles, int tileCount);
    void freeze(bool full = false) override;
    void kick(float directionX) override;
    void kill() override;
    void updateFrozen();
    void updateDying();
    void applyGravity(Tiles* tiles, int tileCount) override;
    void antiGravity(Tiles* tiles, int tileCount);
    bool checkTileBelow(Tiles* tiles, int tileCount);
    void movement(Tiles* tiles, int tileCount) override;
    void update() override;
    void setmv1(bool x);
    void setPos(int row, int col);
};
