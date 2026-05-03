#pragma once
#include "Entity.h"
#include "../Levels/Tiles.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <string>

class Player : public Entity {
protected:
    float movementSpeed;
    bool invincible;
    sf::Clock invincibilityClock;
    sf::Clock blinkClock;
    bool visible;
    float playerScale;
    sf::Texture pTex[4];

public:
    Player();

    void init(const std::string& path0, const std::string& path1,
              const std::string& path2, const std::string& path3);

    void startInvincibility();
    void updateInvincibility();

    bool isInvincible() const;
    void setSpeed(float s);
    void setGravity(float g);

    bool checkCollision(Tiles* tiles, int tileCount);
    void applyGravity(Tiles* tiles, int tileCount) override;
    void move(sf::Keyboard::Key left, sf::Keyboard::Key right,
              sf::Keyboard::Key jump, Tiles* tiles, int tileCount);

    float getDirectionX();
    void setPos(int row, int col);
    void setPos(float x, float y) override;
    void update() override;
    sf::Sprite& getSprite();
    sf::FloatRect boun() const;
};
