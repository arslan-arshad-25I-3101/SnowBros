#pragma once
#include "../Enemy.h"
#include "../../Levels/Tiles.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

class Item {
private:
    Sprite* itemSprite;
    Texture tex;
    bool collected = false;

public:
    enum ItemType { SHOES, ARROW, HEALTH, SPEED_BOOST };
    ItemType type;
    Item(ItemType t, float x, float y) {
        type = t;

        if (type == SHOES) tex.loadFromFile("items/shoes.png");
        else if (type == ARROW) tex.loadFromFile("items/arrow.png");

        itemSprite = new Sprite(tex);
        itemSprite->setPosition({ x,y });
    }

    bool checkPlayerCollision(FloatRect playerBounds) {
        if (collected) return false;

        if (itemSprite->getGlobalBounds().findIntersection(playerBounds)) {
            collected = true;
            return true;
        }
        return false;
    }

    ItemType getType() { return type; }

    Sprite getSprite() {
        if (collected) {

            Sprite empty(tex);
            empty.setPosition({ -999.f, -999.f });
            return empty;
        }
        itemSprite->setScale({ 200.f / 550.f,271.f / 730.f });
        return *itemSprite;
    }

    bool isCollected() { return collected; }

    ~Item() {
        delete itemSprite;
        itemSprite = nullptr;
    }
};

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
    Vector2f dropPosition;
    bool shouldDropItem = false;
    Item* droppedItem = nullptr;
    bool itemTransferred = false;

public:
    enum GravityState { NORMAL, JUMPING_UP };
    GravityState currentState;

    Botom();
    bool hasItemToDrop() {
        return shouldDropItem;
    }
    Vector2f getDropPosition() {
        shouldDropItem = false;
        return dropPosition;
    }
    void init();
    bool checkCollision(Tiles* tiles, int tileCount);
    float distanceChecker(Tiles* tiles, int tileCount);
    void freeze(bool full = false) override;
    void kick(float directionX) override;
    void updateItem(FloatRect playerBounds, bool& hasSpeedBoost, bool& hasDistanceIncrease) {
        if (droppedItem != nullptr && !itemTransferred) {
            // Check if already collected
            if (droppedItem->isCollected()) {
                delete droppedItem;
                droppedItem = nullptr;
                return;
            }

            // Check player collision
            if (droppedItem->checkPlayerCollision(playerBounds)) {
                // Apply power-up based on type
                if (droppedItem->getType() == Item::SHOES) {
                    hasSpeedBoost = true;
                }
                else if (droppedItem->getType() == Item::ARROW) {
                    hasDistanceIncrease = true;
                }

                delete droppedItem;
                droppedItem = nullptr;
            }
        }
    }
    Item* transferItem() {
        if (droppedItem != nullptr && !itemTransferred) {
            itemTransferred = true;
            return droppedItem;  // Give it away, don't delete
        }
        return nullptr;
    }
    void drawItem(RenderWindow& window) {
        if (droppedItem != nullptr && !itemTransferred) {
            window.draw(droppedItem->getSprite());
        }
    }
    void kill() override;
    void updateFrozen();
    void updateDying();
    void applyGravity(Tiles* tiles, int tileCount) override;
    void antiGravity(Tiles* tiles, int tileCount);
    bool checkTileBelow(Tiles* tiles, int tileCount);
    Item* dropPowerUp(float x, float y);
    void movement(Tiles* tiles, int tileCount) override;
    void update() override;
    void reset() {
        alive = true;
        dying = false;
        frozen = false;
        rolling = false;
        frozenIndex = 0;
        velocityY = 0.f;
        rollingVelocityX = 0.f;
        shouldDropItem = false;
        itemTransferred = false;

        // Clean up any dropped item
        if (droppedItem != nullptr) {
            delete droppedItem;
            droppedItem = nullptr;
        }
    }
    void setmv1(bool x);
    void setPos(int row, int col);
};
