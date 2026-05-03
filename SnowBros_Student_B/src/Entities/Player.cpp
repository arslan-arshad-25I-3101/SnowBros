#include "Player.h"

Player::Player() : Entity(), movementSpeed(2.25f), invincible(false), visible(true), playerScale(1.f) {}

void Player::init(const std::string& path0, const std::string& path1,
                  const std::string& path2, const std::string& path3) {
    pTex[0].loadFromFile(path0);
    pTex[1].loadFromFile(path1);
    pTex[2].loadFromFile(path2);
    pTex[3].loadFromFile(path3);

    if (sprite) delete sprite;
    sprite = new sf::Sprite(pTex[0]);

    float sx = Level::TILE_W / (float)pTex[0].getSize().x;
    float sy = Level::TILE_H / (float)pTex[0].getSize().y;
    playerScale = (sx < sy) ? sx : sy;
    sprite->setScale({ playerScale, playerScale });
    sprite->setOrigin({ pTex[0].getSize().x / 2.f, pTex[0].getSize().y / 2.f });
}

void Player::startInvincibility() {
    invincible = true;
    invincibilityClock.restart();
    blinkClock.restart();
}

void Player::updateInvincibility() {
    if (!invincible) {
        if (sprite) sprite->setColor(sf::Color::White);
        return;
    }
    if (invincibilityClock.getElapsedTime().asSeconds() > 4.f) {
        invincible = false;
        if (sprite) sprite->setColor(sf::Color::White);
        return;
    }
    if (blinkClock.getElapsedTime().asSeconds() > 0.1f) {
        visible = !visible;
        if (sprite) sprite->setColor(visible ? sf::Color::White : sf::Color(255, 255, 255, 80));
        blinkClock.restart();
    }
}

bool Player::isInvincible() const { return invincible; }
void Player::setSpeed(float s) { movementSpeed = s; }
void Player::setGravity(float g) { gravity = g; }

bool Player::checkCollision(Tiles* tiles, int tileCount) {
    if (!sprite) return false;
    auto b = sprite->getGlobalBounds();
    sf::FloatRect hitbox({ b.position.x + 6.f, b.position.y + 6.f },
                        { b.size.x - 12.f, b.size.y - 12.f });
    for (int i = 0; i < tileCount; i++) {
        if (hitbox.findIntersection(tiles[i].boun())) return true;
    }
    return false;
}

void Player::applyGravity(Tiles* tiles, int tileCount) {
    velocityY += gravity;
    if (sprite) sprite->move({ 0.f, velocityY });

    if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
        if (sprite) sprite->move({ 0.f, -velocityY });
        velocityY = 0.f;
        onGround = true;
    } else {
        onGround = false;
        if (sprite) {
            if (velocityY < 0)
                sprite->setTexture(pTex[1]);
            else
                sprite->setTexture(pTex[2]);
        }
    }
}

void Player::move(sf::Keyboard::Key left, sf::Keyboard::Key right,
                  sf::Keyboard::Key jump, Tiles* tiles, int tileCount) {
    if (!sprite) return;

    static sf::Clock animClock;
    static int frames = 0;
    float frametime = 0.15f;

    if (sf::Keyboard::isKeyPressed(left)) {
        sprite->move({ -movementSpeed, 0.f });
        if (checkCollision(tiles, tileCount)) {
            sprite->move({ movementSpeed, 0.f });
        } else {
            sprite->setScale({ playerScale, playerScale });
            if (animClock.getElapsedTime().asSeconds() > frametime) {
                frames++;
                if (frames >= 3) frames = 0;
                sprite->setTexture(pTex[frames]);
                animClock.restart();
            }
        }
    }
    if (sf::Keyboard::isKeyPressed(right)) {
        sprite->move({ movementSpeed, 0.f });
        if (checkCollision(tiles, tileCount)) {
            sprite->move({ -movementSpeed, 0.f });
        } else {
            sprite->setScale({ -playerScale, playerScale });
            if (animClock.getElapsedTime().asSeconds() > frametime) {
                frames++;
                if (frames >= 3) frames = 0;
                sprite->setTexture(pTex[frames]);
                animClock.restart();
            }
        }
    }
    if (sf::Keyboard::isKeyPressed(jump)) {
        if (onGround) {
            velocityY = -10.f;
            onGround = false;
        }
    }

    // Screen wrapping — exit left, enter right and vice versa
    auto pos = sprite->getPosition();
    if (pos.x < 0.f)    sprite->setPosition({ 800.f, pos.y });
    if (pos.x > 800.f)  sprite->setPosition({ 0.f,   pos.y });
}

float Player::getDirectionX() {
    if (!sprite) return 1.f;
    return (sprite->getScale().x > 0.f) ? -1.f : 1.f;
}

void Player::setPos(int row, int col) {
    if (sprite) sprite->setPosition({ col * Level::TILE_W + Level::TILE_W / 2.f,
                                      row * Level::TILE_H + Level::TILE_H / 2.f });
}

void Player::setPos(float x, float y) {
    if (sprite) sprite->setPosition({ x, y });
}

void Player::update() {
    updateInvincibility();
}

sf::Sprite& Player::getSprite() { return *sprite; }

sf::FloatRect Player::boun() const {
    return sprite ? sprite->getGlobalBounds() : sf::FloatRect();
}
