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
    Texture tex[4];
    Clock clocks;
    int frames = 0;
    float frametime = 0.15f;
    Sprite* enemy;
    FloatRect bounds;
    float velocityY = 0.f;      // current vertical speed
    float gravity = 0.5f;     // pulls player down each frame
    float jumpForce = -10.f;    // negative = upward in SFML
    bool onGround = false;    // can only jump if standing on something
public:
    Botom() {
        bool ok0 = tex[0].loadFromFile("botom_orange/botom_orange_walk_11.png");
        bool ok1 = tex[1].loadFromFile("botom_orange/botom_orange_walk_12.png");
        bool ok2 = tex[2].loadFromFile("botom_orange/botom_orange_walk_13.png");

        if (!ok0) tex[0].loadFromFile("Characters/Enemies/nick_1.png");
        if (!ok1) tex[1].loadFromFile("Characters/Enemies/nick_2.png");
        if (!ok2) tex[2].loadFromFile("Characters/Enemies/nick_3.png");

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
    void setPos(int row, int col) {
        enemy->setPosition({ col * Level::TILE_W + Level::TILE_W / 2.f,
                             row * Level::TILE_H + Level::TILE_H / 2.f });
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
        //tile.setFillColor(Color(255, 0, 0, 191));
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


//////////////////////////////// SNOWBALL CLASS //////////////////////////////////////

class Snowball {
public:
    static Texture sharedTexture;
    Sprite snowball;
    IntRect projFrames[2];
    int frameIndex;
    float renderScale;
    float velocityX;
    float velocityY;
    float gravity;
    bool active;
    Clock clock;
    Clock animClock;

    Snowball() : snowball(sharedTexture) {
        if (sharedTexture.getSize().x == 0 || sharedTexture.getSize().y == 0) {
            sharedTexture.loadFromFile("Sprite Sheet/Items.png");
        }
        snowball.setTexture(sharedTexture);

        projFrames[0] = IntRect({ 307, 1121 }, { 179, 152 }); // blue blob
        projFrames[1] = IntRect({ 487, 1115 }, { 161, 161 }); // orange burst

        frameIndex = 0;
        snowball.setTextureRect(projFrames[frameIndex]);
        snowball.setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });

        float sx = (Level::TILE_W * 0.8f) / (float)projFrames[frameIndex].size.x;
        float sy = (Level::TILE_H * 0.8f) / (float)projFrames[frameIndex].size.y;
        renderScale = (sx < sy) ? sx : sy;
        snowball.setScale({ renderScale, renderScale });

        active = false;
        velocityX = 0.f;
        velocityY = 0.f;
        gravity = 0.3f;
    }

    void spawn(float x, float y, float directionX) {
        float spawnX = x + directionX * (Level::TILE_W * 0.65f);
        float spawnY = y - Level::TILE_H * 0.15f;
        snowball.setPosition({spawnX, spawnY});

        velocityX = directionX * 4.5f;   // horizontal speed
        velocityY = -3.0f;               // slight upward launch for arc

        frameIndex = 0;
        snowball.setTextureRect(projFrames[frameIndex]);
        snowball.setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });

        if (directionX > 0.f)
            snowball.setScale({ -renderScale, renderScale });
        else
            snowball.setScale({ renderScale, renderScale });

        active = true;
        clock.restart();
        animClock.restart();
    }

    void update(Tiles* tiles, int tileCount) {
        if (!active) return;

        // 1) Animate between blob -> burst frames while projectile is active.
        if (animClock.getElapsedTime().asSeconds() > 0.06f) {
            frameIndex++;
            if (frameIndex > 1) frameIndex = 1;
            snowball.setTextureRect(projFrames[frameIndex]);
            snowball.setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });
            animClock.restart();
        }

        // 2) Save previous-frame hitbox BEFORE moving.
        //    We use this to check if snowball came from above a tile.
        auto before = snowball.getGlobalBounds();
        float insetX = before.size.x * 0.22f;
        float insetY = before.size.y * 0.28f;
        FloatRect prevHitbox({ before.position.x + insetX, before.position.y + insetY },
                             { before.size.x - insetX * 2.f, before.size.y - insetY * 2.f });

        // 3) Apply projectile motion.
        velocityY += gravity;
        snowball.move({velocityX, velocityY});

        // 4) Check tile collision only when falling.
        //    This prevents "head hit" while moving upward.
        if (velocityY >= 0.f) {
            auto sb = snowball.getGlobalBounds();
            FloatRect hitbox({ sb.position.x + insetX, sb.position.y + insetY },
                             { sb.size.x - insetX * 2.f, sb.size.y - insetY * 2.f });

            for (int i = 0; i < tileCount; i++) {
                FloatRect tile = tiles[i].boun();
                if (hitbox.findIntersection(tile)) {
                    float prevBottom = prevHitbox.position.y + prevHitbox.size.y;
                    float tileTop = tile.position.y;

                    // 5) Top-surface-only rule:
                    //    Deactivate only if projectile was above tile top in previous frame.
                    //    +4.f gives tolerance so tiny jitter doesn't count as roof hit.
                    if (prevBottom <= tileTop + 4.f) {
                        active = false;
                        return;
                    }
                }
            }
        }

        auto pos = snowball.getPosition();

        // Screen wrap horizontally
        if (pos.x > 800.f) snowball.setPosition({0.f, pos.y});
        if (pos.x < 0.f)   snowball.setPosition({800.f, pos.y});

        // Deactivate if off-screen or after short range
        if (pos.y > 600.f || clock.getElapsedTime().asSeconds() > 0.6f) {
            active = false;
        }
    }

    void draw(RenderWindow& window) {
        if (active) window.draw(snowball);
    }

    FloatRect getBounds() {
        return snowball.getGlobalBounds();
    }
};



//////////////////////////////// SNOWBALL end //////////////////////////////////////

class Player : public Botom {
protected:
    bool isJump;
    float playerScale = 1.f;

    bool checkCollision(Tiles* tiles, int tileCount) {
        auto bounds = enemy->getGlobalBounds();
        FloatRect hitbox({ bounds.position.x + 6.f, bounds.position.y + 6.f },
                         { bounds.size.x - 12.f, bounds.size.y - 12.f });

        for (int i = 0; i < tileCount; i++) {
            if (hitbox.findIntersection(tiles[i].boun())) {
                return true;
            }
        }
        return false;
    }

public:
    Player() {
        tex[0].loadFromFile("Characters/Enemies/nick_1.png");
        tex[1].loadFromFile("Characters/Enemies/nick_2.png");
        tex[2].loadFromFile("Characters/Enemies/nick_3.png");
        tex[3].loadFromFile("Characters/Enemies/nick_0.png"); // stationary 
        delete enemy;
        enemy = new Sprite(tex[0]);

        float sx = Level::TILE_W / (float)tex[0].getSize().x;
        float sy = Level::TILE_H / (float)tex[0].getSize().y;
        playerScale = (sx < sy) ? sx : sy;
        enemy->setScale({ playerScale, playerScale });
        enemy->setOrigin({ tex[0].getSize().x / 2.f, tex[0].getSize().y / 2.f });
    }


    void applyGravity(Tiles* tiles, int tileCount) {
        velocityY += gravity;
        enemy->move({ 0.f, velocityY });

        if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
            enemy->move({ 0.f, -velocityY });
            velocityY = 0.f;
            onGround = true;
            isJump = false;
        }
        else {
            onGround = false;
            if (velocityY < 0)
                enemy->setTexture(tex[1]);
            else
                enemy->setTexture(tex[2]);
        }
    }

    void move(Keyboard::Key key, Tiles* tiles, int tileCount) {

        if (key == Keyboard::Key::A) {
            enemy->move({ -2.25f, -0.0f });
            if (checkCollision(tiles, tileCount)) {
                enemy->move({ 2.25f, -0.0f }); // undo movement
            } else {
                enemy->setScale({ playerScale, playerScale });
                if (clocks.getElapsedTime().asSeconds() > frametime) {
                    frames++;
                    if (frames >= 3)
                        frames = 0;
                    enemy->setTexture(tex[frames]);
                    clocks.restart();
                }
            }
        }
        if (key == Keyboard::Key::D) {
            enemy->move({ +2.25f, -0.0f });
            if (checkCollision(tiles, tileCount)) {
                enemy->move({ -2.25f, -0.0f }); // undo movement
            } else {
                enemy->setScale({ -playerScale, playerScale });
                if (clocks.getElapsedTime().asSeconds() > frametime) {
                    frames++;
                    if (frames >= 3)
                        frames = 0;
                    enemy->setTexture(tex[frames]);
                    clocks.restart();
                }
            }
        }

        // W key just LAUNCHES the player upward
        if (key == Keyboard::Key::W) {
            if (onGround) {             // can't double jump
                velocityY = jumpForce;  // -10.f  shoots upward
                onGround = false;
                isJump = true;
            }
        }


        //if (key == Keyboard::Key::S) {
        //    enemy->move({ +0.0f, +2.25f });
        //    if (checkCollision(tiles, tileCount)) {
        //        enemy->move({ +0.0f, -2.25f }); // undo movement
        //    } else {
        //        float dir = (enemy->getScale().x < 0.f) ? -1.f : 1.f;
        //        enemy->setScale({ dir * playerScale, playerScale });
        //        if (clocks.getElapsedTime().asSeconds() > frametime) {
        //            frames++;
        //            if (frames >= 3)
        //                frames = 0;
        //            enemy->setTexture(tex[frames]);
        //            clocks.restart();
        //        }
        //    }
        //}
    }
    
    float getDirectionX() {
        return (enemy->getScale().x > 0.f) ? -1.f : 1.f; // +scale is A (left), -scale is D (right)
    }
    
    sf::Vector2f getPosition() {
        return enemy->getPosition();
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

    Level currentLevel;
    Texture bgTex;
    Sprite background(bgTex);
    Tiles* tilt = nullptr;
    int count = 0;
    int levelNo = 1;

    LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);

    Player play;
    const int MAX_SNOWBALLS = 24;
    Snowball snowballs[MAX_SNOWBALLS];
    play.setPos(12, 5);

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

            
            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                if (keyPressed->code == Keyboard::Key::N) {
                    levelNo++;
                    if (levelNo > 10)
                        levelNo = 1;
                    LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                    play.setPos(12, 5);
                }
                if (keyPressed->code == Keyboard::Key::J) {
                    // Reuse first inactive snowball from fixed pool
                    for (int i = 0; i < MAX_SNOWBALLS; i++) {
                        if (!snowballs[i].active) {
                            snowballs[i].spawn(play.getPosition().x, play.getPosition().y, play.getDirectionX());
                            break;
                        }
                    }
                }
            }

        }

        // NOTES FOR MYSELF:
        // have to fix the left se jao, right se ao (nvm) , nick currently also falls off when 
        // he walks off from edge
     
        if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
            play.move(Keyboard::Key::A, tilt, count);
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
            play.move(Keyboard::Key::D, tilt, count);
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
            play.move(Keyboard::Key::W, tilt, count);
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
            play.move(Keyboard::Key::S, tilt, count);
        }


        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            snowballs[i].update(tilt, count);
        }

        play.applyGravity(tilt, count);

        window.clear(Color::Black);
        window.draw(background);
        for (int i = 0; i < count; i++) {
            tilt[i].Draw(window);
        }
        
        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            snowballs[i].draw(window);
        }
        window.draw(play.Draw());

        window.display();
    }

    delete[] tilt;
    tilt = nullptr;
    return 0;
}

Texture Snowball::sharedTexture;