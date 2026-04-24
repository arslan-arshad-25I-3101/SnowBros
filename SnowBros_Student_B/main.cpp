#include "src/Game.h"
#include <cstdlib>
#include <ctime>
// Quick-fix includes for testing in main.cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include "src/Levels/LevelData.h"

using namespace std;
using namespace sf;

class Botom {
protected:
    bool mv1 = false;
    Texture tex[3];
    Clock clocks;
    int frames = 0;
    float frametime = 0.15f;
    Sprite* enemy;
    FloatRect bounds;
public:
    Botom() {
        tex[0].loadFromFile("botom_orange/botom_orange_walk_11.png");
        tex[1].loadFromFile("botom_orange/botom_orange_walk_12.png");
        tex[2].loadFromFile("botom_orange/botom_orange_walk_13.png");
        enemy = new Sprite(tex[0]);
    }
    void movement() {
        if (mv1 == true) {
            enemy->move({ -2.25f, -0.0f });
            enemy->setScale({ 1.f,1.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();

            }
        }
        else if (mv1 == false) {
            enemy->move({ +2.25f, -0.0f });
            enemy->setScale({ -1.f,1.f });
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();
            }
        }
        if (enemy->getPosition().x >= 1150 && enemy->getPosition().x <= 1200)
            mv1 = true;
        if (enemy->getPosition().x >= 0 && enemy->getPosition().x <= 50)
            mv1 = false;

    }
    Sprite getEnemy() {
        return *enemy;
    }
    void setPos(float i) {
        enemy->setPosition({ 350.f, 65.f + i * 65.f });
    }
    void setOrigin(float x, float y) {
        enemy->setOrigin({ x,y });
    }
    void setmv1(bool x) {
        mv1 = x;
    }
    bool getmv1() {
        return mv1;
    }
    FloatRect boun() {
        return enemy->getGlobalBounds();
    }
    ~Botom() {
        delete enemy;
        enemy = nullptr;
    }
};

void mover(int n, Botom* other) {
    for (int i = 0; i < n; i++) {
        other[i].movement();
    }
}

void Draw(int n, Botom* other, RenderWindow& window) {
    for (int i = 0; i < n; i++) {
        window.draw(other[i].getEnemy());
    }
}

void setPos(float x, float y, int n, Botom* other) {
    for (int i = 0; i < n; i++) {
        other[i].setOrigin(x, y);
        other[i].setPos(i);
        if (i % 2 == 1)
            other[i].setmv1(false);
        else
            other[i].setmv1(true);
    }
}

class Tiles {
private:
    RectangleShape tile;
    FloatRect bounds;

public:
    Tiles() {
        tile.setSize({ Level::TILE_W, Level::TILE_H });
        tile.setFillColor(Color(255, 0, 0, 191));
        //tile.setOutlineThickness(0.f);
    }

    void setpost(float x, float y) {
        tile.setPosition({ x, y });
    }

    void Draw(RenderWindow& window) {
        window.draw(tile);
    }

    FloatRect boun() {
        return tile.getGlobalBounds();
    }
};

class Player : public Botom {
protected:
    bool isJump;
public:
    Player() {
        tex[0].loadFromFile("nick/nick_1.png");
        tex[1].loadFromFile("nick/nick_2.png");
        tex[2].loadFromFile("nick/nick_3.png");
        delete enemy;
        enemy = new Sprite(tex[0]);
    }
    void move(Keyboard::Key key) {
        if (key == Keyboard::Key::A) {
            enemy->move({ -2.25f, -0.0f });
            enemy->setScale({ 1.f,1.f });
            if (clocks.getElapsedTime().asSeconds() > frametime) {
                frames++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();
            }
        }
        if (key == Keyboard::Key::D) {
            enemy->move({ +2.25f, -0.0f });
            enemy->setScale({ -1.f,1.f });
            if (clocks.getElapsedTime().asSeconds() > frametime) {
                frames++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();
            }
        }
        if (key == Keyboard::Key::W) {
            enemy->move({ +0.0f, -2.25f });
            enemy->setScale({ 1.f,1.f });
            if (clocks.getElapsedTime().asSeconds() > frametime) {
                frames++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();
            }
        }
        if (key == Keyboard::Key::S) {
            enemy->move({ +0.0f, +2.25f });
            enemy->setScale({ 1.f,1.f });
            if (clocks.getElapsedTime().asSeconds() > frametime) {
                frames++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTexture(tex[frames]);
                clocks.restart();
            }
        }
        if (enemy->getPosition().x >= 1150 && enemy->getPosition().x <= 1200)
            mv1 = true;
        if (enemy->getPosition().x >= 0 && enemy->getPosition().x <= 50)
            mv1 = false;
    }
    Sprite Draw() {
        return *enemy;
    }
    void Setorigin(float x, float y) {

    }
    ~Player() {
    }
};


/////////////////// LOADING LEVELS //////////////////////////////



void LoadLevel(
    int levelNo,
    Level& level,
    Texture& bgTex,
    Sprite& background,
    Tiles*& tilt,
    int& count)
{
    // choose level
    if (levelNo == 1) SetupLevel1(level);
    else if (levelNo == 2) SetupLevel2(level);
    else if (levelNo == 3) SetupLevel3(level);
    else if (levelNo == 4) SetupLevel4(level);
    else if (levelNo == 5) SetupLevel5(level);
    else if (levelNo == 6) SetupLevel6(level);
    else if (levelNo == 7) SetupLevel7(level);
    else if (levelNo == 8) SetupLevel8(level);
    else if (levelNo == 9) SetupLevel9(level);
    else if (levelNo == 10) SetupLevel10(level);

    bgTex.loadFromFile(level.backgroundPath);
    background = Sprite(bgTex);

    // CHANGE LATER (check if the re-scale is needed)

    if (bgTex.getSize().x > 0 && bgTex.getSize().y > 0) {
        level.bgRect = IntRect({ 0, 0 }, { (int)bgTex.getSize().x, (int)bgTex.getSize().y });
        background.setTextureRect(level.bgRect);
        background.setScale({ 800.f / (float)bgTex.getSize().x, 600.f / (float)bgTex.getSize().y });
    }

    if (tilt != nullptr) {
        delete[] tilt;
        tilt = nullptr;
    }

    count = 0;
    for (int r = 0; r < Level::ROWS; r++)
        for (int c = 0; c < Level::COLS; c++)
            if (level.grid[r][c] != 0) count++;

    tilt = new Tiles[count];
    int idx = 0;
    for (int r = 0; r < Level::ROWS; r++) {
        for (int c = 0; c < Level::COLS; c++) {
            if (level.grid[r][c] != 0) {
                tilt[idx].setpost(c * Level::TILE_W, r * Level::TILE_H);
                idx++;
            }
        }
    }
}


///////////////////////////////////// LEVEL LOADING END ////////////////////////////////////

int main()
{
    RenderWindow window(VideoMode({ 800u, 600u }), "HOPE");

    Level level_1;
    Texture bgTex;
    Sprite background(bgTex);
    Tiles* tilt = nullptr;
    int count = 0;

    LoadLevel(1, level_1, bgTex, background, tilt, count);

    Level* currentLevel = &level_1;

    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        while (true)
        {
            auto event = window.pollEvent();
            if (!event.has_value())
                break;

            if (event->is<Event::Closed>())
                window.close();
        }

        window.clear(Color::Black);
        window.draw(background);
        for (int i = 0; i < count; i++) {
            tilt[i].Draw(window);
        }
        window.display();
    }

    delete[] tilt;
    tilt = nullptr;
    return 0;
}