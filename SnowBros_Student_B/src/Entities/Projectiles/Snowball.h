#pragma once
#include "../Entity.h"
#include "../../Levels/Tiles.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

class Snowball : public Entity {
public:
    static sf::Texture sharedTexture;
    sf::IntRect projFrames[2];
    int frameIndex;
    float renderScale;
    float velocityX;
    bool active;
    bool bursting;
    sf::Clock clock;
    sf::Clock burstClock;

    Snowball();

    void spawn(float x, float y, float directionX, bool hasDistanceIncrease = false);
    void applyGravity(Tiles* tiles, int tileCount) override;
    void update(Tiles* tiles, int tileCount, bool hasDistanceIncrease = false);
    void update() override;
    bool isActive() const;
    void Draw(sf::RenderWindow& window) override;
};
