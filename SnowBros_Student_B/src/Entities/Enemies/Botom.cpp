#include "Botom.h"
#include <iostream>
#include <cmath>

Botom::Botom() : Enemy(), mv1(false), frames(0), frametime(0.15f), speed(2.25f), 
          onUpper(false), anti(false), pausef(0), currentState(NORMAL) {}

void Botom::init() {
    tex[0].loadFromFile("botom_orange/botom_orange_walk_11.png");
    tex[1].loadFromFile("botom_orange/botom_orange_walk_12.png");
    tex[2].loadFromFile("botom_orange/botom_orange_walk_13.png");
    tex[3].loadFromFile("botom_orange/Botom_bruh.png");
    tex[4].loadFromFile("botom_orange/Botom_orange_fall.png");
    tex[5].loadFromFile("botom_orange/botom_jump.png");

    if (sprite) delete sprite;
    sprite = new sf::Sprite(tex[0]);

    // Scale logic from main.cpp
    float sx = (float)Level::TILE_W / 94.f;
    float sy = (float)Level::TILE_H / 95.f;
    enemyScale = (sx < sy) ? sx : sy;
    sprite->setScale({ enemyScale, enemyScale });
    // Load frozen texture once (shared across all enemies)
    if (frozenSharedTexture.getSize().x == 0 || frozenSharedTexture.getSize().y == 0) {
        frozenSharedTexture.loadFromFile("Sprite Sheet/Player_Blue.png");
    }

    // 4-stage ice overlay animation frames
    frozenFrames[0] = sf::IntRect({ 13, 919 }, { 57, 49 });
    frozenFrames[1] = sf::IntRect({ 83, 903 }, { 65, 65 });
    frozenFrames[2] = sf::IntRect({ 176, 899 }, { 72, 70 });
    frozenFrames[3] = sf::IntRect({ 260, 883 }, { 69, 86 });
    sprite->setOrigin({ 94.f / 2.f, 95.f / 2.f });

    if (!frozenOverlay) {
        frozenOverlay = new sf::Sprite(frozenSharedTexture);
    }
    frozenIndex = 0;
}

bool Botom::checkCollision(Tiles* tiles, int tileCount) {
    if (!sprite) return false;
    auto b = sprite->getGlobalBounds();
    sf::FloatRect hitbox({ b.position.x + 6.f, b.position.y + 6.f },
                        { b.size.x - 12.f, b.size.y - 12.f });

    for (int i = 0; i < tileCount; i++) {
        if (hitbox.findIntersection(tiles[i].boun())) {
            return true;
        }
    }
    return false;
}

float Botom::distanceChecker(Tiles* tiles, int tileCount) {
    if (!sprite) return 0.f;
    auto b = sprite->getGlobalBounds();
    sf::FloatRect hitbox({ b.position.x + 6.f, b.position.y + 6.f },
                        { b.size.x - 12.f, b.size.y - 12.f });
    for (int i = 0; i < tileCount; i++) {
        if (hitbox.findIntersection(tiles[i].boun())) {
            for (int j = 0; j < tileCount; j++) {
                if (tiles[i].getX() == tiles[j].getX() && tiles[j].getY() < tiles[i].getY()) {
                    double distance = std::hypot(tiles[j].getX() - sprite->getPosition().x,
                                                 tiles[j].getY() - sprite->getPosition().y);
                    return (float)distance;
                }
            }
        }
    }
    return 0.f;
}

void Botom::freeze(bool full) {
    if (full) {
        frozenIndex = 3;
        frozen = true;
        freezeClock.restart();
        return;
    }
    if (frozen) {
        if (frozenIndex < 3) frozenIndex++;
    } else {
        frozenIndex = 0;
    }
    frozen = true;
    freezeClock.restart();
}

void Botom::kick(float directionX) {
    rolling = true;
    rollingVelocityX = directionX * 8.f;
    rollingClock.restart();
}

void Botom::kill() {
    dying = true;
    dyingClock.restart();
    frozen = false;
    rolling = false;

    // STORE DROP POSITION AND FLAG
    if (sprite) {
        dropPosition = sprite->getPosition();
        shouldDropItem = true;
    }
}

void Botom::updateFrozen() {
    if (!frozen) return;
    if (freezeClock.getElapsedTime().asSeconds() > 1.0f) {
        frozenIndex--;
        if (frozenIndex < 0) {
            frozen = false;
            frozenIndex = 0;
        }
        freezeClock.restart();
    }
}

void Botom::updateDying() {
    if (!dying) return;
    float elapsed = dyingClock.getElapsedTime().asSeconds();
    float duration = 0.4f;
    if (elapsed < duration) {
        float t = 1.f - (elapsed / duration);
        sprite->setScale({ enemyScale * t, enemyScale * t });
    } else {
        alive = false;
        dying = false;
        sprite->setPosition({ -999.f, -999.f });
    }
}

void Botom::applyGravity(Tiles* tiles, int tileCount) {
    velocityY += gravity;
    float limit = sprite->getPosition().y + velocityY;
    if (limit < 15.f) {
        velocityY = 15.f - sprite->getPosition().y;
    }
    if (sprite) sprite->move({ 0.f, velocityY });

    if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
        if (sprite) sprite->move({ 0.f, -velocityY });
        velocityY = 0.f;
        onGround = true;
    } else {
        onGround = false;
        if (sprite) sprite->setTexture(tex[4]);
    }
}

void Botom::antiGravity(Tiles* tiles, int tileCount) {
    if (currentState == NORMAL && checkCollision(tiles, tileCount)) {
        velocityY = -5.0f;
        currentState = JUMPING_UP;
        if (sprite) sprite->setTexture(tex[5]);
    }

    if (currentState == JUMPING_UP) {
        velocityY += gravity * 0.5f;
        if (sprite) sprite->move({ 0.f, velocityY });
        if (checkCollision(tiles, tileCount) && velocityY >= 0) {
            if (sprite) sprite->move({ 0.0f, -velocityY });
            velocityY = 0.0f;
            currentState = NORMAL;
            onUpper = true;
        }
    }
}

bool Botom::checkTileBelow(Tiles* tiles, int tileCount) {
    if (!sprite) return false;
    auto b = sprite->getGlobalBounds();
    sf::FloatRect checkArea({ b.position.x + 6.f, b.position.y + b.size.y },
                            { b.size.x - 20.f, 4.f });

    for (int i = 0; i < tileCount; i++) {
        if (checkArea.findIntersection(tiles[i].boun())) {
            return true;
        }
    }
    return false;
}

Item* Botom::dropPowerUp(float x, float y) {
    // Random chance to drop (e.g., 30% drop rate)
    if (rand() % 100 < 99) {
        // Randomly select power-up type
        int randomType = rand() % 2;
        Item::ItemType type;

        switch (randomType) {
        case 0: type = Item::SHOES; break;
        case 1: type = Item::ARROW; break;
            //case 2: type = Item::HEALTH; break;
            //case 3: type = Item::SPEED_BOOST; break;
        }

        return new Item(type, x, y);
    }
    return nullptr; // No drop
}

void Botom::movement(Tiles* tiles, int tileCount) {
    if (!alive || dying) return;
    if (sprite->getPosition().y > 650.f) {
        kill();
        return;
    }
    if (rolling) {
        if (rollingClock.getElapsedTime().asSeconds() > 3.f) {
            kill();
            return;
        }
        velocityY += gravity;
        sprite->move({ 0.f, velocityY });
        if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
            sprite->move({ 0.f, -velocityY });
            velocityY = 0.f;
        }
        sprite->move({ rollingVelocityX, 0.f });
        auto pos = sprite->getPosition();
        if (pos.y > 500.f) {
            if (pos.x > 800.f || pos.x < 0.f) kill();
        } else {
            if (pos.x > 800.f) sprite->setPosition({ 0.f, pos.y });
            if (pos.x < 0.f)   sprite->setPosition({ 800.f, pos.y });
        }
        return;
    }
    if (frozen) return;

    if (mv1) {
        sprite->move({ -speed, 0.f });
        sprite->setScale({ 89.f / 185.f, 97.f / 185.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            frames++;
            if (frames >= 3) frames = 0;
            sprite->setTexture(tex[frames]);
            clocks.restart();
        }
    } else {
        sprite->move({ speed, 0.f });
        sprite->setScale({ -89.f / 185.f, 97.f / 185.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            frames++;
            if (frames >= 3) frames = 0;
            sprite->setTexture(tex[frames]);
            clocks.restart();
        }
    }

    auto pos = sprite->getPosition();
    if (pos.x >= 750 && pos.x <= 800) {
        mv1 = true;
        sprite->setTexture(tex[3]);
        pausef = 60;
    }
    if (pos.x >= 0 && pos.x <= 50) {
        mv1 = false;
        sprite->setTexture(tex[3]);
        pausef = 60;
    }
}

void Botom::update() {
    updateFrozen();
    updateDying();
}

void Botom::setmv1(bool x) { mv1 = x; }

void Botom::setPos(int row, int col) {
    if (sprite) sprite->setPosition({ col * Level::TILE_W + Level::TILE_W / 2.f,
                                      row * Level::TILE_H + Level::TILE_H / 2.f });
}
