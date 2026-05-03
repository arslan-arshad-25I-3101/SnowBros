#pragma once
#include <SFML/Graphics.hpp>



class Entity {
protected:
    sf::Sprite* sprite;
    sf::Vector2f position;
    sf::FloatRect bounds;
    float velocityY;
    float gravity;
    bool onGround;

public:
    Entity() : sprite(nullptr), velocityY(0.f), gravity(0.5f), onGround(false) {}
    virtual ~Entity() {
        if (sprite) {
            delete sprite;
            sprite = nullptr;
        }
    }

    virtual void Draw(sf::RenderWindow& window) {
        if (sprite) {
            window.draw(*sprite);
        }
    }

    virtual void setPos(float x, float y) {
        if (sprite) {
            sprite->setPosition({ x, y });
        }
    }

    virtual sf::Vector2f getPosition() const {
        return sprite ? sprite->getPosition() : sf::Vector2f(0.f, 0.f);
    }

    virtual sf::FloatRect boun() const {
        return sprite ? sprite->getGlobalBounds() : sf::FloatRect();
    }

    virtual void applyGravity(class Tiles* tiles, int tileCount) = 0;
    virtual void update() = 0;
};


