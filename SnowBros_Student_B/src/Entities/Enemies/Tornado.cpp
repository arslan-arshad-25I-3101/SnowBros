#include "Tornado.h"

Tornado::Tornado() : Fooga(), spinFrames(0) {}

void Tornado::init() {
    tex[0].loadFromFile("tornado/tornado_blue_idle.png");
    tex[1].loadFromFile("tornado/tornado_blue_01.png");
    tex[2].loadFromFile("tornado/tornado_blue_02.png");
    tex[3].loadFromFile("tornado/tornado_blue_03.png");
    tex[4].loadFromFile("tornado/tornado_blue_03.png");
    tex[5].loadFromFile("tornado/tornado_blue_03.png");

    if (sprite) delete sprite;
    sprite = new sf::Sprite(tex[0]);

    spin[0].loadFromFile("tornado/tornado_blue_spin_01.png");
    spin[1].loadFromFile("tornado/tornado_blue_spin_02.png");
    spin[2].loadFromFile("tornado/tornado_blue_spin_03.png");
    spin[3].loadFromFile("tornado/tornado_blue_spin_04.png");
    spin[4].loadFromFile("tornado/tornado_blue_spin_05.png");
    spin[5].loadFromFile("tornado/tornado_blue_spin_06.png");
    spin[6].loadFromFile("tornado/tornado_blue_spin_07.png");
    spin[7].loadFromFile("tornado/tornado_blue_spin_08.png");

    sprite->setOrigin({ tex[0].getSize().x / 2.f, tex[0].getSize().y / 2.f });
}

void Tornado::movement_spin(Tiles* tiles, int tileCount) {
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
        sprite->setScale({ 131.f / 325.f, 122.f / 325.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            spinFrames++; if (spinFrames >= 8) spinFrames = 4;
            sprite->setTexture(spin[spinFrames]); clocks.restart();
        }
    } else if (!mv1 && (pos.y <= 550 && pos.y >= 35)) {
        velocity_X -= 0.0625f;
        sprite->move({ speed, velocityY });
        sprite->setScale({ -131.f / 325.f, 122.f / 325.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            spinFrames++; if (spinFrames >= 8) spinFrames = 4;
            sprite->setTexture(spin[spinFrames]); clocks.restart();
        }
    } else if (pos.y < 25.f) {
        velocity_X += 0.0625f;
        velocityY += 0.75f;
        sprite->move({ speed, velocityY });
        sprite->setScale({ -131.f / 325.f, 122.f / 325.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            spinFrames++; if (spinFrames >= 8) spinFrames = 4;
            sprite->setTexture(spin[spinFrames]); clocks.restart();
        }
    } else {
        velocity_X -= 0.0625f;
        velocityY -= 0.5f;
        sprite->move({ speed, velocityY });
        sprite->setScale({ -131.f / 325.f, 122.f / 325.f });
        if (clocks.getElapsedTime().asSeconds() > frametime) {
            spinFrames++; if (spinFrames >= 8) spinFrames = 4;
            sprite->setTexture(spin[spinFrames]); clocks.restart();
        }
    }

    pos = sprite->getPosition();
    if (pos.x >= 725 && pos.x <= 800) {
        mv1 = true; sprite->setTexture(tex[3]); pausef = 60;
        velocity_X -= 0.0625f; velocityY = -velocityY;
    }
    if (pos.x >= 0 && pos.x <= 50) {
        mv1 = false; sprite->setTexture(tex[3]); pausef = 60;
        velocity_X += 0.0625f; velocityY = -velocityY;
    }
    if (pos.y >= 0 && pos.y <= 15) {
        mv1 = false; sprite->setTexture(tex[1]); pausef = 60;
        velocity_X += 0.0625; velocityY = -velocityY;
    }
    if (pos.y >= 550 && pos.y <= 600) {
        mv1 = false; sprite->setTexture(tex[1]); pausef = 60;
        velocity_X -= 0.25f; velocityY = -velocityY;
    }
    if (pos.x <= 0 && pos.y <= 0) {
        mv1 = false; sprite->setTexture(tex[1]); pausef = 60;
        velocity_X += 0.0625; velocityY = -velocityY;
    }
}
