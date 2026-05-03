#pragma once
#include <SFML/Graphics.hpp>
#include "LevelData.h"

class Tiles {
private:
    sf::RectangleShape tile;
    sf::FloatRect bounds;

public:
    Tiles() {
        tile.setSize({ (float)Level::TILE_W, (float)Level::TILE_H });
        tile.setFillColor(sf::Color::Transparent);
    }

    void setpost(float x, float y) {
        tile.setPosition({ x, y });
    }

    void Draw(sf::RenderWindow& window) {
        window.draw(tile);
    }
    float getX() const {
        return tile.getPosition().x;
    }
    float getY() const {
        return tile.getPosition().y;
    }
    sf::FloatRect boun() const {
        return tile.getGlobalBounds();
    }
};
