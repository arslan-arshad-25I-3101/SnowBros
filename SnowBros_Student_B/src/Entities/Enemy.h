#pragma once
#include "Entity.h"
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics.hpp>
#include "../Levels/LevelData.h"

class Enemy : public Entity {
protected:
    bool frozen;
    int frozenIndex;
    sf::Clock freezeClock;
    sf::Sprite* frozenOverlay;
    sf::IntRect frozenFrames[4];
    static sf::Texture frozenSharedTexture;

    bool rolling;
    float rollingVelocityX;
    sf::Clock rollingClock;

    bool alive;
    bool dying;
    sf::Clock dyingClock;
    float enemyScale;

public:
    Enemy();
    virtual ~Enemy();

    bool isAlive() const;
    bool isDying() const;
    bool isFrozen() const;
    bool isRolling() const;
    bool isFullyFrozen() const;

    virtual void setPos(float i);
    void setPos(int row, int col);
    void setPos(float x, float y, float originX, float originY);
    void setOrigin(float x, float y);

    virtual void reset();

    virtual void freeze(bool full = false) = 0;
    virtual void kick(float directionX) = 0;
    virtual void kill() = 0;
    virtual void movement(class Tiles* tiles, int tileCount);
    
    virtual sf::Sprite& getSprite();
    virtual sf::Sprite& getFrozenOverlay();
};
