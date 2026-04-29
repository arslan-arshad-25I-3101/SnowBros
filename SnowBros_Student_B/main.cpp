#include "src/Game.h"
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "src/Levels/LevelData.h"

using namespace std;
using namespace sf;

// Forward declaration for Tiles
class Tiles;

class Botom {
protected:
    bool mv1 = false; // direction of movement: false = right, true = left
    Texture tex[4];
    Texture spriteSheet;
    IntRect walkFrames[3];
    Clock clocks;
    int frames = 0;
    float frametime = 0.15f; // time between animation frames in seconds
    Sprite* enemy; 
    FloatRect bounds;
    float velocityY = 0.f;      // current vertical speed
    float gravity = 0.5f;     // pulls player down each frame
    float jumpForce = -10.f;    // negative = upward in SFML
    bool onGround = false;    // can only jump if standing on something
    bool frozen = false;    // whether encased in snowball
    Clock freezeClock;       // timer for snowball encasement
    IntRect frozenFrames[4]; // 4 stages of ice overlay
    int frozenIndex = 0;    // current frame (0-3)
    Sprite frozenOverlay;   // overlay sprite drawn on top of enemy
    static Texture frozenSharedTexture;  // ONE shared texture for all enemies
    float enemyScale = 1.f;  // scale for rendering

    // Rolling state — after player kicks a fully-frozen enemy
    bool rolling = false;
    float rollingVelocityX = 0.f;
    Clock rollingClock;          // rolling timeout — dies after a few seconds

    // Death state
    bool alive = true;           // dead enemies are skipped entirely
    bool dying = false;          // brief shrink animation before truly dead
    Clock dyingClock;            // timer for death animation

    bool checkCollision(Tiles* tiles, int tileCount);

public:
    Botom() : enemy(nullptr), frozenOverlay(frozenSharedTexture) {
        bool loaded = spriteSheet.loadFromFile("Sprite Sheet/Botom_Orange.png");

        enemy = new Sprite(spriteSheet);

        walkFrames[0] = IntRect({ 96, 360 }, { 94, 95 });
        walkFrames[1] = IntRect({ 190, 395 }, { 102, 96 });
        walkFrames[2] = IntRect({ 7, 469 }, { 91, 107 });

        enemy->setTextureRect(walkFrames[0]);
        
        float sx = Level::TILE_W / (float)walkFrames[0].size.x;
        float sy = Level::TILE_H / (float)walkFrames[0].size.y;
        enemyScale = (sx < sy) ? sx : sy;  // Scale based on frame size, not full sheet
        enemy->setScale({ enemyScale, enemyScale });
        enemy->setOrigin({ walkFrames[0].size.x / 2.f, walkFrames[0].size.y / 2.f });

        // Load frozen texture once (shared across all enemies)
        if (frozenSharedTexture.getSize().x == 0 || frozenSharedTexture.getSize().y == 0) {
            frozenSharedTexture.loadFromFile("Sprite Sheet/Player_Blue.png");
        }

        // 4-stage ice overlay animation frames
        frozenFrames[0] = IntRect({ 13, 919 }, { 57, 49 });
        frozenFrames[1] = IntRect({ 83, 903 }, { 65, 65 });
        frozenFrames[2] = IntRect({ 176, 899 }, { 72, 70 });
        frozenFrames[3] = IntRect({ 260, 883 }, { 69, 86 });

        frozenIndex = 0;
    }

    void movement(Tiles* tiles, int tileCount);

    void updateFrozen() {
        if (!frozen) return;

        float elapsed = freezeClock.getElapsedTime().asSeconds();

        // Every 1.5s, melt one stage (decrement frozenIndex)
        if (elapsed > 1.0f) {
            frozenIndex--;
            if (frozenIndex < 0) {
                // Fully melted — enemy is free again
                frozen = false;
                frozenIndex = 0;
            }
            freezeClock.restart(); // restart timer for next melt stage
        }
    }

    void freeze() {
        if (frozen) {
            // Already frozen — stack more ice (max frame 3)
            if (frozenIndex < 3) {
                frozenIndex++;
            }
        } else {
            // First hit — start at frame 0 (lightest ice)
            frozenIndex = 0;
        }
        frozen = true;
        freezeClock.restart(); // reset melt timer on each hit
    }

    // Kick a fully-frozen enemy — starts the rolling state
    void kick(float directionX) {
        rolling = true;
        rollingVelocityX = directionX * 8.f;  // fast horizontal speed
        rollingClock.restart();                // start the timeout
    }

    void kill() {
        // Don't die instantly — enter the "dying" state first
        // This gives us time to play a shrink/flash animation
        dying = true;
        dyingClock.restart();
        // Stop all other behavior
        frozen = false;
        rolling = false;
    }

    // Called every frame — handles the death animation
    void updateDying() {
        if (!dying) return;

        float elapsed = dyingClock.getElapsedTime().asSeconds();
        float duration = 0.4f;  // total death animation time

        if (elapsed < duration) {
            // Shrink from full size → 0 over 0.4 seconds
            //   "elapsed / duration" goes from 0.0 → 1.0
            //   "1.0 - that" goes from 1.0 → 0.0  (shrinking)
            float t = 1.f - (elapsed / duration);
            enemy->setScale({ enemyScale * t, enemyScale * t });
        } else {
            // Animation done — truly dead now
            alive = false;
            enemy->setPosition({ -999.f, -999.f });
        }
    }

    Sprite getEnemy() {
        return *enemy;
    }

    Sprite getFrozenOverlay() {
        // Return the overlay sprite positioned at enemy's position
        // Call this to draw frozen effect on top
        if (!frozen) {
            // Return a sprite that won't draw (we check in main loop)
            Sprite empty(frozenSharedTexture);
            empty.setPosition({ -999.f, -999.f });  // offscreen
            return empty;
        }

        frozenOverlay.setTextureRect(frozenFrames[frozenIndex]);
        frozenOverlay.setPosition(enemy->getPosition());
        frozenOverlay.setOrigin({ frozenFrames[frozenIndex].size.x / 2.f,
                                  frozenFrames[frozenIndex].size.y / 2.f });
        return frozenOverlay;
    }

    void setPos(float i) { 
        enemy->setPosition({ 350.f, 65.f + i * 45.f });
    }
    void setPos(int row, int col) {
        enemy->setPosition({ col * Level::TILE_W + Level::TILE_W / 2.f,
                             row * Level::TILE_H + Level::TILE_H / 2.f });
    }
    void setmv1(bool x) {
        mv1 = x;
    }
    bool getmv1() {
        return mv1;
    }
    bool isFrozen() {
        return frozen;
    }
    bool isFullyFrozen() {
        return frozen && frozenIndex == 3;
    }
    bool isRolling() {
        return rolling;
    }
    bool isDying() {
        return dying;
    }
    bool isAlive() {
        return alive && !dying;  // dying enemies count as "not alive" for gameplay
    }
    FloatRect boun() {
        return enemy->getGlobalBounds();
    }
    ~Botom() {
        delete enemy;
        enemy = nullptr;
    }
};

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

// Now that Tiles is defined, we can implement Botom::checkCollision and movement()
bool Botom::checkCollision(Tiles* tiles, int tileCount) {
    auto box = enemy->getGlobalBounds();
    FloatRect hitbox({ box.position.x + 6.f, box.position.y + 6.f },
                     { box.size.x - 12.f, box.size.y - 12.f });

    for (int i = 0; i < tileCount; i++) {
        if (hitbox.findIntersection(tiles[i].boun())) {
            return true;
        }
    }
    return false;
}

void Botom::movement(Tiles* tiles, int tileCount) {
    if (!alive || dying) return;  // dead/dying enemies do nothing

    // Safety net: if enemy falls off the bottom of the screen, kill it
    if (enemy->getPosition().y > 650.f) {
        kill();
        return;
    }

    // --- ROLLING STATE: fast horizontal movement with gravity ---
    if (rolling) {
        // Timeout: after 3 seconds of rolling, the snowball breaks
        if (rollingClock.getElapsedTime().asSeconds() > 3.f) {
            kill();
            return;
        }

        // Gravity still applies while rolling
        velocityY += gravity;
        enemy->move({ 0.f, velocityY });

        if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
            enemy->move({ 0.f, -velocityY });
            velocityY = 0.f;
        }

        // Move horizontally at high speed
        enemy->move({ rollingVelocityX, 0.f });

        // Screen wrap or break (classic Snow Bros behavior)
        auto pos = enemy->getPosition();
        if (pos.y > 500.f) {
            // On the bottom floor, the rolling snowball breaks when going off-screen
            if (pos.x > 800.f || pos.x < 0.f) {
                kill();
            }
        } else {
            // On higher floors, it wraps around
            if (pos.x > 800.f) enemy->setPosition({ 0.f, pos.y });
            if (pos.x < 0.f)   enemy->setPosition({ 800.f, pos.y });
        }

        return; // skip normal walking logic
    }

    if (frozen) return;  // frozen enemies don't walk

    // Apply gravity first
    velocityY += gravity;
    enemy->move({ 0.f, velocityY });

    if (velocityY >= 0.f && checkCollision(tiles, tileCount)) {
        enemy->move({ 0.f, -velocityY });
        velocityY = 0.f;
        onGround = true;
    }
    else {
        onGround = false;
    }

    // Horizontal movement (only if on ground)
    if (onGround) {
        if (mv1 == true) {
            enemy->move({ -2.25f, 0.f });
            enemy->setScale({ enemyScale, enemyScale });  // Use stored scale, not 1.0f
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTextureRect(walkFrames[frames]);
                clocks.restart();
            }
            // Reverse direction at left edge
            if (enemy->getPosition().x <= 50)
                mv1 = false;
        }
        else if (mv1 == false) {
            enemy->move({ +2.25f, 0.f });
            enemy->setScale({ -enemyScale, enemyScale });  // Use stored scale, flipped for direction
            if ((clocks.getElapsedTime().asSeconds()) > frametime) {
                (frames)++;
                if (frames >= 3)
                    frames = 0;
                enemy->setTextureRect(walkFrames[frames]);
                clocks.restart();
            }
            // Reverse direction at right edge
            if (enemy->getPosition().x >= 750)
                mv1 = true;
        }
    }
}

void mover(int n, Botom* other, Tiles* tiles, int tileCount) {
    for (int i = 0; i < n; i++) {
        if (!other[i].isAlive()) continue;  // skip dead
        other[i].movement(tiles, tileCount);
    }
}

void Draw(int n, Botom* other, RenderWindow& window) {
    for (int i = 0; i < n; i++) {
        if (!other[i].isAlive() && !other[i].isDying()) continue;  // skip fully dead, but draw dying (shrink anim)
        window.draw(other[i].getEnemy());
    }
}

void setPos(float x, float y, int n, Botom* other) {
    for (int i = 0; i < n; i++) {
        other[i].setPos(i);
        if (i % 2 == 1)
            other[i].setmv1(false);
        else
            other[i].setmv1(true);
    }
}

//////////////////////////////// SNOWBALL CLASS //////////////////////////////////////

class Snowball {
public:
    static Texture sharedTexture;
    Sprite snowball;
    IntRect projFrames[2]; // [0] flying snowball, [1] burst on impact
    int frameIndex;
    float renderScale;
    float velocityX;
    float velocityY;
    float gravity;
    bool active;    // state 1
    bool bursting; // state 2
    Clock clock;
    Clock burstClock;

    Snowball() : snowball(sharedTexture) {
        if (sharedTexture.getSize().x == 0 || sharedTexture.getSize().y == 0) {
            sharedTexture.loadFromFile("Sprite Sheet/Items.png");
        }
        snowball.setTexture(sharedTexture);

        // Flying and burst frames
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
        bursting = false;
        velocityX = 0.f;
        velocityY = 0.f;
        gravity = 0.3f;
    }

    void spawn(float x, float y, float directionX) {
        // Reset state for a fresh projectile
        bursting = false;

        float spawnX = x + directionX * (Level::TILE_W * 0.65f);
        float spawnY = y - Level::TILE_H * 0.15f;
        snowball.setPosition({ spawnX, spawnY });

        velocityX = directionX * 4.5f;
        velocityY = -3.0f;

        frameIndex = 0;
        snowball.setTextureRect(projFrames[frameIndex]);
        snowball.setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });

        if (directionX > 0.f)
            snowball.setScale({ -renderScale, renderScale });
        else
            snowball.setScale({ renderScale, renderScale });

        active = true;
        clock.restart();
    }

    void update(Tiles* tiles, int tileCount) {
        if (!active) return;

        // If already bursting, show burst briefly then remove projectile.
        if (bursting) {
            if (burstClock.getElapsedTime().asSeconds() > 0.12f) {
                active = false;
                bursting = false;
            }
            return;
        }

        // Save previous-frame hitbox before movement.
        auto before = snowball.getGlobalBounds();
        float insetX = before.size.x * 0.22f;
        float insetY = before.size.y * 0.28f;
        FloatRect prevHitbox({ before.position.x + insetX, before.position.y + insetY },
                             { before.size.x - insetX * 2.f, before.size.y - insetY * 2.f });

        // Projectile motion
        velocityY += gravity;
        snowball.move({ velocityX, velocityY });

        // Check collision with floor only while falling. 
        // This prevents "head hit" while moving upward.
        if (velocityY >= 0.f) {
            auto sb = snowball.getGlobalBounds();
            FloatRect hitbox({ sb.position.x + insetX, sb.position.y + insetY },
                             { sb.size.x - insetX * 2.f, sb.size.y - insetY * 2.f });

            for (int i = 0; i < tileCount; i++) {
                FloatRect tile = tiles[i].boun();
                if (hitbox.findIntersection(tile)) {
                    float prevBottom = prevHitbox.position.y + prevHitbox.size.y;
                    float tileTop = tile.position.y;

                    // Top-surface hit only
                    if (prevBottom <= tileTop + 4.f) {
                        bursting = true;
                        velocityX = 0.f;
                        velocityY = 0.f;

                        frameIndex = 1;
                        snowball.setTextureRect(projFrames[frameIndex]);
                        snowball.setOrigin({ projFrames[frameIndex].size.x / 2.f, projFrames[frameIndex].size.y / 2.f });

                        burstClock.restart();
                        return;
                    }
                }
            }
        }

        auto pos = snowball.getPosition();

        // Screen wrap horizontally
        if (pos.x > 800.f) snowball.setPosition({ 0.f, pos.y });
        if (pos.x < 0.f)   snowball.setPosition({ 800.f, pos.y });

        // End projectile after short range
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

bool snowballHitsEnemy(Snowball& snowball, Botom& enemy) {
    if (!snowball.active) return false;
    // No isFrozen() guard — additional hits stack more ice layers

    if (snowball.getBounds().findIntersection(enemy.boun())) {
        enemy.freeze();
        return true;
    }

    return false;
}

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
    const int MAX_ENEMIES = 6;
    Botom enemies[MAX_ENEMIES];

    // Place enemies on valid platforms from Level 1:
    //   Row 4 has tiles at cols 4-11  (enemies stand ON row 4 = placed at row 3)
    //   Row 7 has tiles at cols 1-6 and 9-14
    //   Row 9 has tiles at cols 3-12
    //   Row 11 has tiles at cols 0-3, 6-9, 12-15
    // setPos(row, col) uses row/col to calculate pixel position
    // Place enemy one row ABOVE the platform so it lands on it
    enemies[0].setPos(3, 5);   enemies[0].setmv1(true);
    enemies[1].setPos(3, 9);   enemies[1].setmv1(false);
    enemies[2].setPos(6, 3);   enemies[2].setmv1(true);
    enemies[3].setPos(6, 11);  enemies[3].setmv1(false);
    enemies[4].setPos(8, 5);   enemies[4].setmv1(true);
    enemies[5].setPos(10, 8);  enemies[5].setmv1(false);

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

        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            if (!snowballs[i].active)
                continue;

            for (int j = 0; j < MAX_ENEMIES; j++) { // checking for collision between snowball and enemy
                if (!enemies[j].isAlive()) continue;
                if (snowballHitsEnemy(snowballs[i], enemies[j])) {
                    snowballs[i].active = false; // consume the snowball on hit
                    break;
                }
            }
        }

        // --- KICK DETECTION ---
        // If player overlaps a fully-frozen enemy, kick it in the player's direction
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            if (!enemies[i].isFullyFrozen()) continue;
            if (enemies[i].isRolling()) continue;  // already rolling

            if (play.boun().findIntersection(enemies[i].boun())) {
                enemies[i].kick(play.getDirectionX());
            }
        }

        // --- ROLLING KILLS ---
        // A rolling enemy kills any non-rolling enemy it overlaps
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            if (!enemies[i].isRolling()) continue;

            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (i == j) continue;  // don't collide with yourself
                if (!enemies[j].isAlive()) continue;
                if (enemies[j].isRolling()) continue;  // two rolling won't kill each other

                if (enemies[i].boun().findIntersection(enemies[j].boun())) {
                    enemies[j].kill();  // enemy in the path dies
                    // The rolling snowball keeps going to kill others in its path
                }
            }
        }

        // Move all enemies with gravity and tile collision
        mover(MAX_ENEMIES, enemies, tilt, count);

        // Update frozen state for all enemies (animate overlay, check unfreeze timer)
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            enemies[i].updateFrozen();
        }

        // Update dying animation (shrink effect) for all enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            enemies[i].updateDying();
        }

        play.applyGravity(tilt, count);

        window.clear(Color::Black);
        window.draw(background);
        for (int i = 0; i < count; i++) {
            tilt[i].Draw(window);
        }

        // Draw all enemies to screen.
        Draw(MAX_ENEMIES, enemies, window);

        // Draw frozen overlays on top of enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            if (enemies[i].isFrozen()) {
                window.draw(enemies[i].getFrozenOverlay());
            }
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
Texture Botom::frozenSharedTexture;

