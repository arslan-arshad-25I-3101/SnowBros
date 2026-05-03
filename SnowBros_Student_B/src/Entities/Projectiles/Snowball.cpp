#include "Snowball.h"

sf::Texture Snowball::sharedTexture;

Snowball::Snowball() : Entity(), frameIndex(0), renderScale(1.f), velocityX(0.f), active(false), bursting(false) {
    if (sharedTexture.getSize().x == 0) {
        sharedTexture.loadFromFile("Sprite Sheet/Items.png");
    }
    sprite = new sf::Sprite(sharedTexture);

    projFrames[0] = sf::IntRect({ 307, 1121 }, { 179, 152 });
    projFrames[1] = sf::IntRect({ 487, 1115 }, { 161, 161 });

    sprite->setTextureRect(projFrames[0]);
    sprite->setOrigin({ projFrames[0].size.x / 2.f, projFrames[0].size.y / 2.f });

    float sx = (Level::TILE_W * 0.8f) / (float)projFrames[0].size.x;
    float sy = (Level::TILE_H * 0.8f) / (float)projFrames[0].size.y;
    renderScale = (sx < sy) ? sx : sy;
    sprite->setScale({ renderScale, renderScale });

    gravity = 0.3f;
}

void Snowball::spawn(float x, float y, float directionX, bool hasDistanceIncrease) {
    bursting = false;
    float spawnX = x + directionX * (Level::TILE_W * 0.65f);
    float spawnY = y - Level::TILE_H * 0.15f;
    sprite->setPosition({ spawnX, spawnY });

    velocityX = directionX * 4.5f;
    velocityY = -3.0f;

    frameIndex = 0;
    sprite->setTextureRect(projFrames[frameIndex]);
    sprite->setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });

    if (directionX > 0.f)
        sprite->setScale({ -renderScale, renderScale });
    else
        sprite->setScale({ renderScale, renderScale });

    active = true;
    clock.restart();
}

void Snowball::applyGravity(Tiles* tiles, int tileCount) {
    velocityY += gravity;
}

void Snowball::update(Tiles* tiles, int tileCount, bool hasDistanceIncrease) {
    if (!active) return;

    if (bursting) {
        if (burstClock.getElapsedTime().asSeconds() > 0.12f) {
            active = false;
            bursting = false;
            if (sprite) sprite->setPosition({ -999.f, -999.f });
        }
        return;
    }

    auto before = sprite->getGlobalBounds();
    float insetX = before.size.x * 0.22f;
    float insetY = before.size.y * 0.28f;
    sf::FloatRect prevHitbox({ before.position.x + insetX, before.position.y + insetY },
                            { before.size.x - insetX * 2.f, before.size.y - insetY * 2.f });

    velocityY += gravity;
    sprite->move({ velocityX, velocityY });

    if (velocityY >= 0.f) {
        auto sb = sprite->getGlobalBounds();
        sf::FloatRect hitbox({ sb.position.x + insetX, sb.position.y + insetY },
                            { sb.size.x - insetX * 2.f, sb.size.y - insetY * 2.f });

        for (int i = 0; i < tileCount; i++) {
            sf::FloatRect tile = tiles[i].boun();
            if (hitbox.findIntersection(tile)) {
                float prevBottom = prevHitbox.position.y + prevHitbox.size.y;
                float tileTop = tile.position.y;
                if (prevBottom <= tileTop + 4.f) {
                    bursting = true;
                    velocityX = 0.f;
                    velocityY = 0.f;
                    frameIndex = 1;
                    sprite->setTextureRect(projFrames[frameIndex]);
                    sprite->setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });
                    burstClock.restart();
                    return;
                }
            }
        }
    }

    auto pos = sprite->getPosition();
    if (pos.x > 800.f) sprite->setPosition({ 0.f, pos.y });
    if (pos.x < 0.f)   sprite->setPosition({ 800.f, pos.y });

    float maxTime = hasDistanceIncrease ? 2.5f : 0.6f;
    if (pos.y > 600.f || clock.getElapsedTime().asSeconds() > maxTime) {
        active = false;
    }
}

void Snowball::update() {}

bool Snowball::isActive() const { return active; }

void Snowball::Draw(sf::RenderWindow& window) {
    if (active && sprite) {
        window.draw(*sprite);
    }
}
