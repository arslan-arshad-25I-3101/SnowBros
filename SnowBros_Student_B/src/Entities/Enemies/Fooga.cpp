#include "Fooga.h"

Fooga::Fooga() : Botom(), isFlying(false), velocity_X(0.125f) {}

void Fooga::init() {
    tex[0].loadFromFile("fooga/flying_01.png");
    tex[1].loadFromFile("fooga/flying_02.png");
    tex[2].loadFromFile("fooga/flying_03.png");
    tex[3].loadFromFile("fooga/flying_01.png");
    tex[4].loadFromFile("fooga/flying_01.png");

    if (sprite) delete sprite;
    sprite = new sf::Sprite(tex[0]);

    if (frozenSharedTexture.getSize().x == 0 || frozenSharedTexture.getSize().y == 0) {
        frozenSharedTexture.loadFromFile("Sprite Sheet/Player_Blue.png");
    }
    frozenFrames[0] = sf::IntRect({ 13, 919 }, { 57, 49 });
    frozenFrames[1] = sf::IntRect({ 83, 903 }, { 65, 65 });
    frozenFrames[2] = sf::IntRect({ 176, 899 }, { 72, 70 });
    frozenFrames[3] = sf::IntRect({ 260, 883 }, { 69, 86 });

    sprite->setOrigin({ tex[0].getSize().x / 2.f, tex[0].getSize().y / 2.f });

    if (!frozenOverlay) {
        frozenOverlay = new sf::Sprite(frozenSharedTexture);
    }
    frozenIndex = 0;
}

void Fooga::movement(Tiles* tiles, int tileCount) {
    if (!alive || dying) return;
    if (rolling) {
        if (rollingClock.getElapsedTime().asSeconds() > 3.f) { kill(); return; }
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

    auto pos = sprite->getPosition();
    if (mv1 && (pos.y <= 550 && pos.y >= 35)) {
        velocity_X -= 0.0625f;
        sprite->move({ -speed, -velocityY + speed });
        sprite->setScale({ 177.f / 455.f, 180.f / 455.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            frames++; if (frames >= 3) frames = 0;
            sprite->setTexture(tex[frames]); clocks.restart();
        }
    } else if (!mv1 && (pos.y <= 550 && pos.y >= 35)) {
        velocity_X -= 0.0625f;
        sprite->move({ speed, velocityY });
        sprite->setScale({ -177.f / 455.f, 180.f / 455.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            frames++; if (frames >= 3) frames = 0;
            sprite->setTexture(tex[frames]); clocks.restart();
        }
    } else if (pos.y < 25.f) {
        velocity_X += 0.0625f;
        velocityY += 0.75f;
        sprite->move({ speed, velocityY });
        sprite->setScale({ -177.f / 455.f, 180.f / 455.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            frames++; if (frames >= 3) frames = 0;
            sprite->setTexture(tex[frames]); clocks.restart();
        }
    } else {
        velocity_X -= 0.0625f;
        velocityY -= 0.5f;
        sprite->move({ speed, velocityY });
        sprite->setScale({ -177.f / 455.f, 180.f / 455.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            frames++; if (frames >= 3) frames = 0;
            sprite->setTexture(tex[frames]); clocks.restart();
        }
    }

    pos = sprite->getPosition();
    if (pos.x >= 750.f) {
        mv1 = true; sprite->setTexture(tex[3]); pausef = 60;
        velocity_X = -0.125f; velocityY = -velocityY;
    }
    if (pos.x <= 50.f) {
        mv1 = false; sprite->setTexture(tex[3]); pausef = 60;
        velocity_X = 0.125f; velocityY = -velocityY;
    }
    if (pos.y <= 15.f) {
        mv1 = !mv1; sprite->setTexture(tex[1]); pausef = 60;
        velocityY = -velocityY;
    }
    if (pos.y >= 550.f) {
        mv1 = !mv1; sprite->setTexture(tex[1]); pausef = 60;
        velocityY = -velocityY;
    }
}
