#include "src/Levels/Tiles.h"
#include "src/Entities/Player.h"
#include "src/Entities/Enemies/Botom.h"
#include "src/Entities/Enemies/Fooga.h"
#include "src/Entities/Enemies/Tornado.h"
#include "src/Entities/Enemies/Mogera.h"
#include "src/Entities/Projectiles/Snowball.h"
#include "src/Systems/DatabaseManager.h"
#include "src/Systems/ShopManager.h"
#include <fstream>

using namespace std;
using namespace sf;

// Global Power-up states
bool hasSpeedBoost = false;
bool hasSnowballPower = false;
bool hasDistanceIncrease = false;
bool hasBalloonMode = false;

enum class GameState { Login, MainMenu, ModeSelect, Shop, Playing, Paused, LevelComplete, GameOver, GameComplete, Signup, FlashScreen, LevelSelect, Ranking, Victory, CharacterSelect };

// Function declarations
void mover(int n, Botom* other, Tiles* tiles, int tileCount);
void Draw(int n, Botom* other, RenderWindow& window);
void mover(int n, Fooga* other, Tiles* tiles, int tileCount);
void Draw(int n, Fooga* other, RenderWindow& window);
void Draw(int n, Tornado* other, RenderWindow& window);
void mover(int n, Tornado* other, Tiles* tiles, int tileCount);
void Gravity(int n, Botom* other, Tiles* tiles, int c);
void Gravity(int n, Fooga* other, Tiles* tiles, int c);
void Gravity(Tornado* other, int n, Tiles* tiles, int c);

bool snowballHitsEnemy(Snowball& snowball, Botom& enemy, bool hasPower) {
    if (!snowball.active) return false;
    if (snowball.boun().findIntersection(enemy.boun())) {
        enemy.freeze(hasPower);
        return true;
    }
    return false;
}

bool snowballHitsEnemy(Snowball& snowball, Fooga& enemy, Tiles* tiles, int tileCount, bool hasPower) {
    if (!snowball.active) return false;
    if (snowball.boun().findIntersection(enemy.boun())) {
        enemy.freeze(hasPower);
        enemy.applyGravity(tiles, tileCount);
        return true;
    }
    return false;
}



bool snowballHitsMogera(Snowball& snowball, Mogera& boss) {
    if (!snowball.active) return false;
    if (snowball.boun().findIntersection(boss.boun())) {
        boss.setCol();
        Mogera::bossHp--;
        return true;
    }
    return false;
}

void LoadLevel(int levelNo, Level& level, Texture& bgTex, Sprite& background, Tiles*& tilt, int& tileCount);

struct EnemySpawnPoint { int row; int col; };



EnemySpawnPoint levelSpawns[10][6] = {
    // Level 1 — platforms at rows 4,5,7,9,11,13
    { {3,4}, {3,11}, {6,2}, {6,13}, {8,3}, {10,7}, },
    // Level 2 — platforms at rows 3,5,7,9,11,13
    { {2,5}, {2,11}, {4,7}, {6,5}, {8,7}, {10,5} },
    // Level 3 — platforms at rows 0,3,5,7,9,11,13
    { {2,6}, {2,10}, {4,5}, {4,11}, {6,5}, {10,4} },
    // Level 4 — platforms at rows 0,3,5,7,9,11,13
    { {2,7}, {4,7}, {6,4}, {6,10}, {8,4}, {10,5} },
    // Level 5 — platforms at rows 0,3,5,7,9,11,13
    { {2,5}, {2,11}, {4,8}, {6,7}, {8,7}, {10,7} },
    // Level 6 — platforms at rows 0,3,4,7,9,11,13
    { {2,5}, {2,10}, {3,7}, {6,6}, {10,5}, {10,10} },
    // Level 7 — platforms at rows 0,3,4,5,7,8,9,10,11,13
    { {2,3}, {2,12}, {4,8}, {6,5}, {8,4}, {10,2} },
    // Level 8 — platforms at rows 0,4,5,7,9,11,13
    { {3,7}, {4,8}, {6,4}, {6,11}, {8,4}, {10,6} },
    // Level 9 — platforms at rows 0,3,5,7,9,11,13
    { {2,6}, {2,10}, {4,5}, {6,4}, {8,5}, {10,4} },
    // Level 10 — platforms at rows 0,5,7,8,9,11,13
    { {4,3}, {4,10}, {6,4}, {8,4}, {10,4}, {10,10} }
};

void spawnFoogas(Fooga* foogas) {
    float fx = 177.f / 2.f;
    float fy = 180.f / 2.f;
    for (int i = 0; i < 4; i++) {
        foogas[i].init();
        foogas[i].reset();
        foogas[i].kill(); // Start inactive — reset() sets alive=true which blocks level completion
    }
    foogas[0].setPos(155.f, 125.f, fx, fy);
    foogas[1].setPos(355.f, 125.f, fx, fy);
    foogas[2].setPos(155.f, 275.f, fx, fy);
    foogas[3].setPos(555.f, 275.f, fx, fy);
}

void spawnEnemies(Botom enemies[], int levelNo) {
    for (int i = 0; i < 6; i++) {
        enemies[i].init();
        enemies[i].reset();
        enemies[i].setPos(levelSpawns[levelNo - 1][i].row, levelSpawns[levelNo - 1][i].col);
    }
}

int main()
{
    // Initializing Random Seed
    srand(static_cast<unsigned>(time(nullptr)));

    RenderWindow window(VideoMode({ 800u, 600u }), "SNOW BROS");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

    struct InputField {
        RectangleShape rect;
        Text label;
        Text content;
        string value;
        bool focused = false;
        bool isPassword = false;

        InputField(Font& font, string lbl, Vector2f pos, bool pass = false) 
            : label(font), content(font) {
            isPassword = pass;
            rect.setSize({ 300.f, 40.f });
            rect.setPosition(pos);
            rect.setFillColor(Color(50, 50, 50));
            rect.setOutlineThickness(2.f);
            rect.setOutlineColor(Color::White);

            label.setString(lbl);
            label.setCharacterSize(20);
            label.setFillColor(Color::Yellow);
            label.setPosition({ pos.x, pos.y - 30.f });

            content.setCharacterSize(24);
            content.setFillColor(Color::White);
            content.setPosition({ pos.x + 10.f, pos.y + 5.f });
        }

        void handleInput(uint32_t unicode) {
            if (!focused) return;
            if (unicode == 8) { // Backspace
                if (!value.empty()) value.pop_back();
            }
            else if (unicode >= 32 && unicode < 127) {
                if (value.length() < 15) value += static_cast<char>(unicode);
            }

            if (isPassword) {
                string masked(value.length(), '*');
                content.setString(masked);
            } else {
                content.setString(value);
            }
        }

        void update() {
            rect.setOutlineColor(focused ? Color::Cyan : Color::White);
            rect.setOutlineThickness(focused ? 3.f : 2.f);
        }

        void draw(RenderWindow& win) {
            win.draw(rect);
            win.draw(label);
            win.draw(content);
        }

        bool contains(Vector2f mPos) {
            return rect.getGlobalBounds().contains(mPos);
        }
    };

    string username, password;
    DatabaseManager db;
    ShopManager shop;
    Mogera mogera;

    int lives = 3, gems = 100, score = 0, count = 0;
    int levelNo = 1;
    bool loggedIn = false;
    bool isMultiplayer = false;
    int lives2 = 3;

    Clock speedBoostTimer;
    Clock balloonTimer;

    GameState currentState = GameState::Login;
    GameState previousState = GameState::MainMenu;
    Level currentLevel;
    Texture bgTex;
    Sprite background(bgTex);
    Tiles* tilt = nullptr;

    LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);

    Player play;
    Player play2;

    string t0 = "Characters/Enemies/nick_0.png";
    string t1 = "Characters/Enemies/nick_1.png";
    string t2 = "Characters/Enemies/nick_2.png";
    string t3 = "Characters/Enemies/nick_3.png";
    play.init(t0, t1, t2, t3);
    play2.init(t0, t1, t2, t3);
    const int MAX_ENEMIES = 6;
    Botom enemies[MAX_ENEMIES];

    spawnEnemies(enemies, levelNo);
    const int MAX_SNOWBALLS = 24;
    Snowball snowballs[MAX_SNOWBALLS];
    play.setPos(12, 5);

    //------------- Foogas ---------------
    int num = 4;
    Fooga fooga[4];
    spawnFoogas(fooga);

    float vx = 572.f / 2.f;
    float vy = 460.f / 2.f;
    mogera.setPos(650.f, 300.f, vx, vy);

    //-----------GAMAKICHI BOSS-----------------
    Gamakichi gamakichi;
    float ux = 1130.f / 2.f;
    float uy = 620.f / 2.f;
    gamakichi.setPos(400.f, 500.f, ux, uy);
    window.setFramerateLimit(60);

    Font font;
    if (!font.openFromFile("Fonts/snow-bros.ttf")) {
        // Handle error
    }

    Text title(font);
    Text prompt(font);
    title.setString("SNOW BROS");
    title.setCharacterSize(100);
    title.setFillColor(Color::White);
    title.setStyle(Text::Bold);
    title.setPosition({ 150.f, 150.f });

    prompt.setString("Press ENTER to Start");
    prompt.setCharacterSize(30);
    prompt.setFillColor(Color::Yellow);
    prompt.setPosition({ 250.f, 400.f });

    Texture flashTexture, mainMenuTexture, shopTexture;
    flashTexture.loadFromFile("Images/Flash_Screen.png");
    mainMenuTexture.loadFromFile("Images/main_menu.png");
    shopTexture.loadFromFile("Images/shop.png");

    Sprite flashSprite(flashTexture), mainMenuSprite(mainMenuTexture), shopSprite(shopTexture);
    flashSprite.setScale({ 800.f / flashTexture.getSize().x, 600.f / flashTexture.getSize().y });
    mainMenuSprite.setScale({ 800.f / mainMenuTexture.getSize().x, 600.f / mainMenuTexture.getSize().y });
    shopSprite.setScale({ 800.f / shopTexture.getSize().x, 600.f / shopTexture.getSize().y });

    while (window.isOpen())
    {
        switch (currentState) {
            case GameState::Login:
            {
                static InputField userField(font, "Username", { 250.f, 200.f });
                static InputField passField(font, "Password", { 250.f, 280.f }, true);
                static string errorMsgText = "";
                static Color errorColor = Color::Red;

                while (true) {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();

                    if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                        Vector2f mPos = window.mapPixelToCoords(Mouse::getPosition(window));
                        userField.focused = userField.contains(mPos);
                        passField.focused = passField.contains(mPos);

                        // Detection for Login Button
                        RectangleShape btn({ 300.f, 45.f });
                        btn.setPosition({ 250.f, 380.f });
                        if (btn.getGlobalBounds().contains(mPos)) {
                            if (db.verifyLogin(userField.value, passField.value)) {
                                username = userField.value;
                                db.LoadUser(username, levelNo, lives, gems, score);
                                LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                                spawnEnemies(enemies, levelNo);
                                spawnFoogas(fooga);
                                play.setPos(12, 5);
                                currentState = GameState::FlashScreen;
                                errorMsgText = "";
                            } else errorMsgText = "Invalid Credentials!";
                        }

                        // Detection for Signup Link
                        Text link(font, "Don't have an account? SIGNUP", 18);
                        link.setPosition({ 250.f, 450.f });
                        if (link.getGlobalBounds().contains(mPos)) {
                            currentState = GameState::Signup;
                            errorMsgText = "";
                        }
                    }

                    if (const auto* textEntered = event->getIf<Event::TextEntered>()) {
                        userField.handleInput(textEntered->unicode);
                        passField.handleInput(textEntered->unicode);
                    }

                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Tab) {
                            userField.focused = !userField.focused;
                            passField.focused = !passField.focused;
                        }
                        if (keyPressed->code == Keyboard::Key::Enter) {
                            if (db.verifyLogin(userField.value, passField.value)) {
                                username = userField.value;
                                db.LoadUser(username, levelNo, lives, gems, score);
                                LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                                spawnEnemies(enemies, levelNo);
                                spawnFoogas(fooga);
                                play.setPos(12, 5);
                                currentState = GameState::FlashScreen;
                            } else {
                                errorMsgText = "Invalid Credentials!";
                            }
                        }
                    }
                }

                userField.update();
                passField.update();

                window.clear(Color(20, 20, 40));
                
                Text loginTitle(font, "USER LOGIN", 40);
                loginTitle.setFillColor(Color::White);
                FloatRect ltBounds = loginTitle.getGlobalBounds();
                loginTitle.setPosition({ (800.f - ltBounds.size.x) / 2.f, 80.f });
                window.draw(loginTitle);

                userField.draw(window);
                passField.draw(window);

                if (!errorMsgText.empty()) {
                    Text err(font, errorMsgText, 18);
                    err.setFillColor(errorColor);
                    FloatRect eb = err.getGlobalBounds();
                    err.setPosition({ (800.f - eb.size.x) / 2.f, 340.f });
                    window.draw(err);
                }

                // Login Button
                RectangleShape btn({ 300.f, 45.f });
                btn.setPosition({ 250.f, 380.f });
                btn.setFillColor(Color(0, 150, 0));
                window.draw(btn);
                Text btnTxt(font, "LOGIN", 24);
                FloatRect lbBounds = btnTxt.getGlobalBounds();
                btnTxt.setPosition({ 250.f + (300.f - lbBounds.size.x) / 2.f, 388.f });
                window.draw(btnTxt);

                Text link(font, "Don't have an account? SIGNUP", 18);
                link.setFillColor(Color::Cyan);
                FloatRect lnBounds = link.getGlobalBounds();
                link.setPosition({ (800.f - lnBounds.size.x) / 2.f, 450.f });
                window.draw(link);

                window.display();
            }
            break;

            case GameState::Signup:
            {
                static InputField userField(font, "New Username", { 250.f, 200.f });
                static InputField passField(font, "New Password", { 250.f, 280.f }, true);
                static string msg = "";

                while (true) {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();

                    if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                        Vector2f mPos = window.mapPixelToCoords(Mouse::getPosition(window));
                        userField.focused = userField.contains(mPos);
                        passField.focused = passField.contains(mPos);

                        // Detection for Register Button
                        RectangleShape btn({ 300.f, 45.f });
                        btn.setPosition({ 250.f, 380.f });
                        if (btn.getGlobalBounds().contains(mPos)) {
                            if (userField.value.length() > 2 && passField.value.length() > 2) {
                                if (db.registerUser(userField.value, passField.value)) {
                                    currentState = GameState::Login;
                                    msg = "Success! Please Login.";
                                } else msg = "User already exists!";
                            } else msg = "Too short!";
                        }

                        // Detection for Login Link
                        Text link(font, "Already have an account? LOGIN", 18);
                        link.setPosition({ 250.f, 450.f });
                        if (link.getGlobalBounds().contains(mPos)) {
                            currentState = GameState::Login;
                        }
                    }
                    if (const auto* textEntered = event->getIf<Event::TextEntered>()) {
                        userField.handleInput(textEntered->unicode);
                        passField.handleInput(textEntered->unicode);
                    }
                }

                window.clear(Color(40, 20, 20));
                Text regTitle(font, "CREATE ACCOUNT", 40);
                regTitle.setFillColor(Color::White);
                FloatRect rtBounds = regTitle.getGlobalBounds();
                regTitle.setPosition({ (800.f - rtBounds.size.x) / 2.f, 80.f });
                window.draw(regTitle);

                userField.update(); passField.update();
                userField.draw(window); passField.draw(window);

                RectangleShape btn({ 300.f, 45.f });
                btn.setPosition({ 250.f, 380.f });
                btn.setFillColor(Color(0, 100, 200));
                window.draw(btn);
                Text btnTxt(font, "REGISTER", 24);
                FloatRect rbBounds = btnTxt.getGlobalBounds();
                btnTxt.setPosition({ 250.f + (300.f - rbBounds.size.x) / 2.f, 388.f });
                window.draw(btnTxt);

                Text link(font, "Already have an account? LOGIN", 18);
                link.setFillColor(Color::Yellow);
                FloatRect slBounds = link.getGlobalBounds();
                link.setPosition({ (800.f - slBounds.size.x) / 2.f, 450.f });
                window.draw(link);



                if (!msg.empty()) {
                    Text t(font, msg, 18);
                    t.setFillColor(Color::White);
                    t.setPosition({ 250.f, 340.f });
                    window.draw(t);
                }

                window.display();
            }
            break;

            case GameState::FlashScreen:
            {
                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value())
                        break;

                    if (event->is<Event::Closed>())
                        window.close();

                    // Transition on key press event (happens only once)
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Enter || keyPressed->code == Keyboard::Key::Space) {
                            currentState = GameState::MainMenu;
                        }
                    }
                }

                window.clear(Color::Black);
                window.draw(flashSprite);

                Text pressEnter(font);
                pressEnter.setString("PRESS ENTER TO START");
                pressEnter.setCharacterSize(20);
                pressEnter.setFillColor(Color::White);
                pressEnter.setPosition({ 280.f, 500.f });
                
                // Simple blink effect
                if ((int)(flashSprite.getColor().a) % 2 == 0) { // Just a placeholder for time-based logic
                     window.draw(pressEnter);
                }

                window.display();
            }
            break;
            case GameState::MainMenu:
            {
                // Corrected Mapping: 1 Play, 2 Multi, 3 Shop, 4 Lead, 5 Exit
                FloatRect btnPlay({ 336.f, 242.f }, { 129.f, 31.f });       
                FloatRect btnContinue({ 500.f, 242.f }, { 120.f, 31.f });
                FloatRect btnMulti({ 333.f, 279.f }, { 135.f, 29.f });     
                FloatRect btnShop({ 370.f, 316.f }, { 70.f, 29.f });       
                FloatRect btnLeader({ 333.f, 353.f }, { 135.f, 29.f });    
                FloatRect btnExit({ 328.f, 390.f }, { 144.f, 25.f });      

                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                int hoveredOption = 0; 

                if (btnPlay.contains(mousePos)) hoveredOption = 1;
                else if (btnMulti.contains(mousePos)) hoveredOption = 2;
                else if (btnShop.contains(mousePos)) hoveredOption = 3;
                else if (btnLeader.contains(mousePos)) hoveredOption = 4;
                else if (btnExit.contains(mousePos)) hoveredOption = 5;
                else if (levelNo > 1 && btnContinue.contains(mousePos)) hoveredOption = 6;

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();

                    if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                        if (mousePressed->button == Mouse::Button::Left) {
                            if (hoveredOption == 1) { 
                                isMultiplayer = false; 
                                lives = 3;
                                currentState = GameState::LevelSelect; 
                            }
                            if (hoveredOption == 2) { 
                                isMultiplayer = true; 
                                lives = 3;
                                lives2 = 3; 
                                currentState = GameState::LevelSelect; 
                            }
                            if (hoveredOption == 3) currentState = GameState::Shop;
                            if (hoveredOption == 4) currentState = GameState::Ranking;
                            if (hoveredOption == 5) window.close(); 
                            if (hoveredOption == 6) {
                                LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                                spawnEnemies(enemies, levelNo);
                                spawnFoogas(fooga);
                                play.setPos(12, 5);
                                if (isMultiplayer) play2.setPos(12, 7);
                                currentState = GameState::Playing;
                            }
                        }
                    }
                }

                window.clear(Color::Black);
                window.draw(mainMenuSprite);
                if (levelNo > 1) {
                    Text continueText(font);
                    continueText.setString("CONTINUE");
                    continueText.setCharacterSize(20);
                    continueText.setFillColor(hoveredOption == 6 ? Color::Cyan : Color::White);
                    continueText.setPosition({ 505.f, 245.f });
                    window.draw(continueText);
                }

                // Restore On-Hover Selectors (Little Snowballs)
                if (hoveredOption > 0) {
                    CircleShape selector(8.f);
                    selector.setFillColor(Color::White);
                    // Match the Y positions of our buttons
                    if (hoveredOption == 1) selector.setPosition({ 310.f, 250.f }); // Play
                    if (hoveredOption == 2) selector.setPosition({ 310.f, 287.f }); // Multi
                    if (hoveredOption == 3) selector.setPosition({ 345.f, 324.f }); // Shop
                    if (hoveredOption == 4) selector.setPosition({ 310.f, 361.f }); // Lead
                    if (hoveredOption == 5) selector.setPosition({ 305.f, 398.f }); // Exit
                    if (hoveredOption == 6) selector.setPosition({ 480.f, 250.f }); // Continue
                    window.draw(selector);
                }

                window.display();
            }
            break;

            case GameState::Ranking:
            {
                struct RankingEntry { string name; int level; int score; };
                RankingEntry topPlayers[100];
                int playerCount = 0;

                ifstream file("progress.txt");
                if (file.is_open()) {
                    string line;
                    while (getline(file, line) && playerCount < 100) {
                        if (line.empty()) continue;
                        stringstream ss(line);
                        string name, temp;
                        getline(ss, name, ',');
                        
                        int lvl = 0;
                        if (getline(ss, temp, ',')) lvl = atoi(temp.c_str());
                        getline(ss, temp, ','); // lives
                        getline(ss, temp, ','); // gems
                        int sc = 0;
                        if (getline(ss, temp, ',')) sc = atoi(temp.c_str());
                        
                        topPlayers[playerCount++] = { name, lvl, sc };
                    }
                    file.close();
                }

                // Bubble Sort (Descending)
                for (int i = 0; i < playerCount - 1; i++) {
                    for (int j = 0; j < playerCount - i - 1; j++) {
                        if (topPlayers[j].score < topPlayers[j + 1].score) {
                            RankingEntry t = topPlayers[j];
                            topPlayers[j] = topPlayers[j + 1];
                            topPlayers[j + 1] = t;
                        }
                    }
                }

                while (true) {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Escape || keyPressed->code == Keyboard::Key::Enter) {
                            currentState = GameState::MainMenu;
                        }
                    }
                }

                window.clear(Color(10, 10, 30));

                // 1. Center Title
                Text leadTitle(font);
                leadTitle.setCharacterSize(50);
                leadTitle.setFillColor(Color::Yellow);
                leadTitle.setString("HALL OF FAME");
                leadTitle.setStyle(Text::Bold);
                FloatRect titleBounds = leadTitle.getGlobalBounds();
                leadTitle.setPosition({ (800.f - titleBounds.size.x) / 2.f, 30.f });
                window.draw(leadTitle);

                // 2. Table Header
                RectangleShape headerBar({ 640.f, 40.f });
                headerBar.setFillColor(Color(40, 40, 80));
                headerBar.setPosition({ 80.f, 110.f });
                window.draw(headerBar);

                Text headerText(font);
                headerText.setCharacterSize(22);
                headerText.setFillColor(Color::Cyan);
                headerText.setString("RANK   NAME   LEVEL   SCORE");
                headerText.setPosition({ 100.f, 115.f });
                window.draw(headerText);

                // 3. Draw Rows
                int show = (playerCount < 8) ? playerCount : 8; // Show top 8
                for (int i = 0; i < show; i++) {
                    // Row Background (alternating)
                    RectangleShape rowBg({ 640.f, 45.f });
                    rowBg.setFillColor(i % 2 == 0 ? Color(25, 25, 55) : Color(20, 20, 50));
                    rowBg.setPosition({ 80.f, 160.f + i * 48.f });
                    window.draw(rowBg);

                    // Rank
                    string rankStr = to_string(i + 1);
                    if (i == 0) rankStr = "1ST";
                    else if (i == 1) rankStr = "2ND";
                    else if (i == 2) rankStr = "3RD";
                    else rankStr += "TH";

                    Text rowText(font);
                    rowText.setCharacterSize(20);
                    rowText.setFillColor(i < 3 ? Color::Yellow : Color::White);
                    
                    // Column Alignment (Fixed X positions)
                    rowText.setString(rankStr);
                    rowText.setPosition({ 100.f, 170.f + i * 48.f });
                    window.draw(rowText);

                    string nameStr = topPlayers[i].name;
                    if (nameStr.length() > 12) nameStr = nameStr.substr(0, 12);
                    rowText.setString(nameStr);
                    rowText.setPosition({ 200.f, 170.f + i * 48.f });
                    window.draw(rowText);

                    rowText.setString(to_string(topPlayers[i].level));
                    rowText.setPosition({ 420.f, 170.f + i * 48.f });
                    window.draw(rowText);

                    rowText.setString(to_string(topPlayers[i].score));
                    rowText.setPosition({ 580.f, 170.f + i * 48.f });
                    window.draw(rowText);
                }

                // 4. Back Prompt
                Text backPrompt(font);
                backPrompt.setCharacterSize(18);
                backPrompt.setFillColor(Color(150, 150, 150));
                backPrompt.setString("PRESS ENTER TO RETURN TO MAIN MENU");
                FloatRect backBounds = backPrompt.getGlobalBounds();
                backPrompt.setPosition({ (800.f - backBounds.size.x) / 2.f, 540.f });
                window.draw(backPrompt);

                window.display();
            }
            break;

            case GameState::LevelSelect:
            {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                Vector2i rawMouse = Mouse::getPosition(window);

                int hoveredLevel = 0;

                // Manual Coordinates provided by USER
                float xCoords[] = { 165.f, 285.f, 401.f, 517.f, 634.f };
                float yRows[] = { 280.f-20.f, 405.f-20.f };

                FloatRect levelButtons[10];
                for (int i = 0; i < 10; i++) {
                    float x = xCoords[i % 5];
                    float y = yRows[i / 5];
                    // Create a clickable area around the text center
                    levelButtons[i] = FloatRect({ x - 25.f, y - 25.f }, { 50.f, 50.f });
                    if (levelButtons[i].contains(mousePos)) hoveredLevel = i + 1;
                }
                FloatRect backBtn({ 321.f, 540.f }, { 477.f - 321.f, 566.f - 540.f });
                if (backBtn.contains(mousePos)) hoveredLevel = -1;

                while (true) {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    
                    if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                        if (mousePressed->button == Mouse::Button::Left) {
                            if (hoveredLevel > 0) {
                                levelNo = hoveredLevel;
                                LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                                spawnEnemies(enemies, levelNo);
                                spawnFoogas(fooga);
                                play.setPos(12, 5);
                                lives = 3;
                                if (isMultiplayer) {
                                    play2.setPos(12, 7);
                                    lives2 = 3;
                                }
                                currentState = GameState::Playing;
                            } else if (hoveredLevel == -1) {
                                currentState = GameState::MainMenu;
                            }
                        }
                    }

                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Escape) currentState = GameState::MainMenu;
                    }
                }

                // --- LEVEL SELECT COORDINATE HELPER (Console) ---
                cout << "LEVEL SELECT Mouse Mapped: " << (int)mousePos.x << "," << (int)mousePos.y 
                     << " | Raw: " << rawMouse.x << "," << rawMouse.y << "\r" << flush;

                window.clear(Color::Black);

                // Optimization: Load background texture once
                static Texture levelSelectTex;
                static bool texLoaded = false;
                if (!texLoaded) {
                    levelSelectTex.loadFromFile("Images/level_selection.png");
                    texLoaded = true;
                }
                Sprite levelSelectSprite(levelSelectTex);
                levelSelectSprite.setScale({ 800.f / levelSelectSprite.getGlobalBounds().size.x, 600.f / levelSelectSprite.getGlobalBounds().size.y });
                window.draw(levelSelectSprite);

                // Draw Level Numbers
                for (int i = 0; i < 10; i++) {
                    Text num(font, to_string(i + 1), 40);
                    num.setFillColor(hoveredLevel == i + 1 ? Color::Green : Color::Yellow);
                    num.setOutlineColor(Color::Black);
                    num.setOutlineThickness(2.f);
                    
                    FloatRect nb = num.getGlobalBounds();
                    // Center the number on the user-provided X,Y
                    num.setPosition({ xCoords[i % 5] - nb.size.x / 2.f, yRows[i / 5] - nb.size.y / 2.f - 5.f });
                    window.draw(num);
                }

                window.display();
            }
            break;
            case GameState::Shop:
            {
                // Define Hitboxes
                FloatRect item1({ 122.f, 378.f }, { 72.f, 25.f }); // width = 194-122, height = 403-378
                FloatRect item2({ 241.f, 378.f }, { 73.f, 25.f });
                FloatRect item3({ 365.f, 378.f }, { 70.f, 25.f });
                FloatRect item4({ 487.f, 378.f }, { 68.f, 25.f });
                FloatRect item5({ 607.f, 378.f }, { 71.f, 25.f });
                FloatRect shopExit({ 319.f, 536.f }, { 162.f, 46.f });

                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                int shopHover = 0;
                if (item1.contains(mousePos)) shopHover = 1;
                else if (item2.contains(mousePos)) shopHover = 2;
                else if (item3.contains(mousePos)) shopHover = 3;
                else if (item4.contains(mousePos)) shopHover = 4;
                else if (item5.contains(mousePos)) shopHover = 5;
                else if (shopExit.contains(mousePos)) shopHover = 6;

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value())
                        break;

                    if (event->is<Event::Closed>())
                        window.close();

                    if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                        if (mousePressed->button == Mouse::Button::Left) {
                            if (shopHover == 1) {
                                if (shop.buyExtraLife(gems, lives)) {
                                    if (isMultiplayer) lives2++;
                                    cout << "Purchased Extra Life! Lives: " << lives << " P2: " << lives2 << endl;
                                }
                            }
                            else if (shopHover == 2) {
                                if (shop.buySpeedBoost(gems, hasSpeedBoost)) {
                                    cout << "Speed Boost Activated!" << endl;
                                    play.setSpeed(3.5f);
                                    if (isMultiplayer) play2.setSpeed(3.5f);
                                    speedBoostTimer.restart();
                                }
                            }
                            else if (shopHover == 3) {
                                if (shop.buySnowballPower(gems, hasSnowballPower)) {
                                    cout << "Snowball Power Activated!" << endl;
                                }
                            }
                            else if (shopHover == 4) {
                                if (shop.buyDistanceIncrease(gems, hasDistanceIncrease)) {
                                    cout << "Distance Increase Activated!" << endl;
                                }
                            }
                            else if (shopHover == 5) {
                                if (shop.buyBalloonMode(gems, hasBalloonMode)) {
                                    cout << "Balloon Mode Activated!" << endl;
                                    play.setGravity(0.25f);
                                    if (isMultiplayer) play2.setGravity(0.25f);
                                    balloonTimer.restart();
                                }
                            }
                            if (shopHover == 6) currentState = previousState;
                        }
                    }

                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Escape) {
                            currentState = previousState;
                        }
                    }
                }

                // --- SHOP COORDINATE HELPER (Console) ---
                Vector2i rawMouse = Mouse::getPosition(window);
                cout << "SHOP Mouse Mapped: " << (int)mousePos.x << "," << (int)mousePos.y 
                     << " | Raw: " << rawMouse.x << "," << rawMouse.y << "\r" << flush;

                // --- SHOP COORDINATE HELPER (On-Screen) ---
                string coordStr = "M: " + to_string((int)mousePos.x) + "," + to_string((int)mousePos.y) + 
                                 "\nR: " + to_string(rawMouse.x) + "," + to_string(rawMouse.y);
                Text coordText(font, coordStr, 12);
                coordText.setFillColor(Color::Green);
                coordText.setOutlineColor(Color::Black);
                coordText.setOutlineThickness(1.f);
                coordText.setPosition({ mousePos.x + 15.f, mousePos.y + 15.f });
                window.draw(coordText);

                window.clear(Color::Black);
                window.draw(shopSprite);

                // Draw Gem Count
                Text gemText(font);
                gemText.setString("GEMS: " + to_string(gems));
                gemText.setCharacterSize(16);
                gemText.setFillColor(Color::Yellow);
                gemText.setPosition({ 601.f, 45.f });
                window.draw(gemText);

                // Draw Selector snowball on hover
                if (shopHover > 0 && shopHover <= 5) {
                    CircleShape selector(5.f);
                    selector.setFillColor(Color::White);
                    // Just a simple visual for now
                    selector.setPosition({ mousePos.x - 15.f, mousePos.y });
                    window.draw(selector);
                }

                window.display();
            }
            break;

            case GameState::Playing:
            {
                // Check Power-up Timers
                if (hasSpeedBoost && speedBoostTimer.getElapsedTime().asSeconds() > 30.f) {
                    hasSpeedBoost = false;
                    play.setSpeed(2.0f);
                    if (isMultiplayer) play2.setSpeed(2.0f);
                    cout << "Speed Boost Expired!" << endl;
                }
                if (hasBalloonMode && balloonTimer.getElapsedTime().asSeconds() > 15.f) {
                    hasBalloonMode = false;
                    play.setGravity(0.5f);
                    if (isMultiplayer) play2.setGravity(0.5f);
                    cout << "Balloon Mode Expired!" << endl;
                }

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value())
                        break;

                    if (event->is<Event::Closed>())
                        window.close();
            
            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                if (keyPressed->code == Keyboard::Key::P || keyPressed->code == Keyboard::Key::Escape) {
                    currentState = GameState::Paused;
                }
                if (keyPressed->code == Keyboard::Key::N) {
                    levelNo++;
                    if (levelNo > 10)
                        levelNo = 1;
                    LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                    play.setPos(12, 5);
                }
                if (lives > 0 && keyPressed->code == Keyboard::Key::J) { // P1 shoots with J
                    for (int i = 0; i < MAX_SNOWBALLS; i++) {
                        if (!snowballs[i].active) {
                            snowballs[i].spawn(play.getPosition().x, play.getPosition().y, play.getDirectionX());
                            break;
                        }
                    }
                }
                if (isMultiplayer && lives2 > 0 && keyPressed->code == Keyboard::Key::K) { // P2 shoots with K
                    for (int i = 0; i < MAX_SNOWBALLS; i++) {
                        if (!snowballs[i].active) {
                            snowballs[i].spawn(play2.getPosition().x, play2.getPosition().y, play2.getDirectionX());
                            break;
                        }
                    }
                }
            }
        }

        // Player 1 Controls (WASD)
        if (lives > 0) {
            play.move(Keyboard::Key::A, Keyboard::Key::D, Keyboard::Key::W, tilt, count);
        }
        
        // Player 2 Controls (Arrows)
        if (isMultiplayer && lives2 > 0) {
            play2.move(Keyboard::Key::Left, Keyboard::Key::Right, Keyboard::Key::Up, tilt, count);
        }

        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            snowballs[i].update(tilt, count);
        }

        // Shooting logic (Events)
        // This was already inside the event loop, let's update it there too.


        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            if (!snowballs[i].active){
            mogera.setNor();
                continue;
                }

            for (int j = 0; j < MAX_ENEMIES; j++) { // checking for collision between snowball and enemy
                if (!enemies[j].isAlive()) continue;
                if (snowballHitsEnemy(snowballs[i], enemies[j], hasSnowballPower)) {
                    snowballs[i].active = false; // consume the snowball on hit
                    break;
                }
            }
        }
        // foogas huh huh
        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            if (!snowballs[i].active)
                continue;
 
            for (int j = 0; j < 4; j++) { // checking for collision between snowball and enemy
                if (!fooga[j].isAlive()) continue;
                if (snowballHitsEnemy(snowballs[i], fooga[j], tilt, count, hasSnowballPower)) {
                    snowballs[i].active = false; // consume the snowball on hit
                    break;
                }
            }
        }

        //FOR MOGERA
        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            if(!snowballs[i].active)
            continue;
            snowballHitsMogera(snowballs[i], mogera);
        }
        // --- UPDATED LEVEL COMPLETION LOGIC IS BELOW IN THE DRAWING SECTION ---

        //        levelNo++;
        //        if (levelNo > 10)
        //           
        //            levelNo = 1;
        //        LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
        //        play.setPos(12, 5);
        //        spawnEnemies(fooga, levelNo);
        //    }
        //}
        //
        // --- KICK DETECTION ---
        // If player overlaps a fully-frozen enemy, kick it in the player's direction
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            if (!enemies[i].isFullyFrozen()) continue;
            if (enemies[i].isRolling()) continue;

            if (play.boun().findIntersection(enemies[i].boun())) {
                enemies[i].kick(play.getDirectionX());
                score += 100;
                gems += 10;
            } else if (isMultiplayer && lives2 > 0 && play2.boun().findIntersection(enemies[i].boun())) {
                enemies[i].kick(play2.getDirectionX());
                score += 100;
                gems += 10;
            }
        }
        for (int i = 0; i < 4; i++) {
            if (!fooga[i].isAlive()) continue;
            if (!fooga[i].isFullyFrozen()) continue;
            if (play.boun().findIntersection(fooga[i].boun())) {
                fooga[i].kick(play.getDirectionX());
                score += 100;
                gems += 10;
            } else if (isMultiplayer && lives2 > 0 && play2.boun().findIntersection(fooga[i].boun())) {
                fooga[i].kick(play2.getDirectionX());
                score += 100;
                gems += 10;
            }
        }
        // --- ROLLING KILLS (Cross-Enemy Collision) ---
        // A rolling enemy (Botom or Fooga) kills any non-rolling enemy it overlaps
        
        // 1. Check Botoms that are rolling
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].isAlive() && enemies[i].isRolling()) {
                // Hits other Botoms
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (i == j || !enemies[j].isAlive() || enemies[j].isRolling()) continue;
                    if (enemies[i].boun().findIntersection(enemies[j].boun())) {
                        enemies[j].kill();
                        score += 200; gems += 10;
                    }
                }
                // Hits Foogas
                for (int j = 0; j < 4; j++) {
                    if (!fooga[j].isAlive() || fooga[j].isRolling()) continue;
                    if (enemies[i].boun().findIntersection(fooga[j].boun())) {
                        fooga[j].kill();
                        score += 200; gems += 10;
                    }
                }
            }
        }

        // 2. Check Foogas that are rolling
        for (int i = 0; i < 4; i++) {
            if (fooga[i].isAlive() && fooga[i].isRolling()) {
                // Hits Botoms
                for (int j = 0; j < MAX_ENEMIES; j++) {
                    if (!enemies[j].isAlive() || enemies[j].isRolling()) continue;
                    if (fooga[i].boun().findIntersection(enemies[j].boun())) {
                        enemies[j].kill();
                        score += 200; gems += 10;
                    }
                }
                // Hits other Foogas
                for (int j = 0; j < 4; j++) {
                    if (i == j || !fooga[j].isAlive() || fooga[j].isRolling()) continue;
                    if (fooga[i].boun().findIntersection(fooga[j].boun())) {
                        fooga[j].kill();
                        score += 200; gems += 10;
                    }
                }
            }
        }

        // --- PLAYER DEATH DETECTION & INVINCIBILITY ---
        play.updateInvincibility(); // Tick the blink/shield timer
 
        if (lives > 0 && !play.isInvincible()) {
            bool playerHit = false;
            // 1. Check basic enemies
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].isAlive() || enemies[i].isFrozen() || enemies[i].isDying()) continue;
                if (play.boun().findIntersection(enemies[i].boun())) {
                    playerHit = true; break;
                }
            }
            // 2. Check Foogas
            if (!playerHit) {
                for (int i = 0; i < 4; i++) {
                    if (!fooga[i].isAlive() || fooga[i].isFrozen() || fooga[i].isDying()) continue;
                    if (play.boun().findIntersection(fooga[i].boun())) {
                        playerHit = true; break;
                    }
                }
            }

            if (playerHit) {
                lives--;
                if (lives > 0) {
                    play.setPos(12, 5);
                    play.startInvincibility();
                } else {
                    play.setPos(-999, -999);
                    if (!isMultiplayer || lives2 <= 0) {
                        db.saveProgress(username, levelNo, 0, gems, score);
                        currentState = GameState::GameOver;
                    }
                }
            }
        }

        // --- PLAYER 2 DEATH DETECTION ---
        if (isMultiplayer && lives2 > 0 && !play2.isInvincible()) {
            bool p2Hit = false;
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (!enemies[i].isAlive() || enemies[i].isFrozen() || enemies[i].isDying()) continue;
                if (play2.boun().findIntersection(enemies[i].boun())) { p2Hit = true; break; }
            }
            if (!p2Hit) {
                for (int i = 0; i < 4; i++) {
                    if (!fooga[i].isAlive() || fooga[i].isFrozen() || fooga[i].isDying()) continue;
                    if (play2.boun().findIntersection(fooga[i].boun())) { p2Hit = true; break; }
                }
            }
            if (p2Hit) {
                lives2--;
                if (lives2 > 0) {
                    play2.setPos(12, 7); // Respawn
                    play2.startInvincibility();
                } else {
                    play2.setPos(-999, -999); // Hide off-screen
                    if (lives <= 0) {
                         db.saveProgress(username, levelNo, 0, gems, score);
                         currentState = GameState::GameOver;
                    }
                }
            }
        }

        // Move all enemies with gravity and tile collision
        mover(MAX_ENEMIES, enemies, tilt, count);
        mover(4, fooga, tilt, count);
        //mover(2, tornado, tilt, count);
        if (mogera.movement(tilt, count, play, db, username, lives, gems, score, levelNo))
            currentState = GameState::GameOver;
        if (gamakichi.movements(tilt, count, play, db, username, lives, gems, score, levelNo))
            currentState = GameState::GameOver;
        //mover(opt,botom);
        Gravity(MAX_ENEMIES, enemies, tilt, count);
        Gravity(4, fooga, tilt, count);
        //Gravity(tornado, 2, tilt, count);
        //Gravity(&mogera, 1, tilt, count);
        // Update frozen state for all enemies (animate overlay, check unfreeze timer)
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (!enemies[i].isAlive()) continue;
            enemies[i].updateFrozen();
        }

        // Update dying animation (shrink effect) for all enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            enemies[i].updateDying();
        }

        //foogas
        for (int i = 0; i < 4; i++) {
            if (!fooga[i].isAlive()) continue;
            fooga[i].updateFrozen();
        }
        for (int i = 0; i < 4; i++) {
            fooga[i].updateDying();
        }
        play.applyGravity(tilt, count);

        window.clear(Color::Black);
        window.draw(background);
        for (int i = 0; i < count; i++) {
            tilt[i].Draw(window);
        }

        // Draw all enemies to screen.
        //Draw(MAX_ENEMIES, enemies, window);
        Draw(MAX_ENEMIES, enemies, window);
        Draw(4, fooga, window);
        //Draw(2, tornado, window);
        gamakichi.Draw(window, tilt, count);
        if (Mogera::bossHp > 0)
            mogera.Draw(window, tilt, count);
        //------------MOGERA DYING-------------------------------
        else if (Mogera::bossHp <= 0 && mogera.getDeath() == true) {
            mogera.setPos(-999.f, -999.f, 0.f, 0.f);
            score += 5000;
            mogera.setDead(); // sets isDead=false so this only fires once
        }
        // Frozen overlays are now handled inside the Draw functions below

        for (int i = 0; i < MAX_SNOWBALLS; i++) {
            snowballs[i].Draw(window);
        }
        if (lives > 0) {
            window.draw(play.getSprite());
        }
        if (isMultiplayer && lives2 > 0) {
            play2.updateInvincibility();
            play2.applyGravity(tilt, count);
            window.draw(play2.getSprite());
        }

        // --- HUD DRAWING ---
        RectangleShape hudBar({ 800.f, 40.f });
        hudBar.setFillColor(Color(0, 0, 0, 160));
        hudBar.setPosition({ 0.f, 0.f });
        window.draw(hudBar);

        static Texture heartTex, gemTex;
        static bool iconsLoaded = false;
        if (!iconsLoaded) {
            heartTex.loadFromFile("Images/heart.png");
            gemTex.loadFromFile("Images/gem.png");
            iconsLoaded = true;
        }

        Text hudText(font, "", 18);
        hudText.setOutlineColor(Color::Black);
        hudText.setOutlineThickness(1.2f);

        // Segment 1: Score (0-200)
        hudText.setFillColor(Color(255, 215, 0));
        hudText.setString("SCORE: " + to_string(score));
        FloatRect scB = hudText.getGlobalBounds();
        hudText.setPosition({ 100.f - scB.size.x / 2.f, 8.f });
        window.draw(hudText);

        // Lives (P1 - Left of Center)
        Sprite heart(heartTex);
        float hScale = 25.f / heartTex.getSize().y;
        heart.setScale({ hScale, hScale });
        
        // Player 1
        heart.setPosition({ 280.f, 8.f });
        window.draw(heart);
        hudText.setFillColor(Color(255, 60, 60));
        hudText.setString("P1: " + to_string(lives));
        hudText.setPosition({ 310.f, 8.f });
        window.draw(hudText);

        // Player 2
        if (isMultiplayer) {
            heart.setPosition({ 420.f, 8.f });
            window.draw(heart);
            hudText.setFillColor(Color(60, 60, 255)); // Blue for P2
            hudText.setString("P2: " + to_string(lives2));
            hudText.setPosition({ 450.f, 8.f });
            window.draw(hudText);
        }

        // Gems (Right)
        Sprite gem(gemTex);
        float gScale = 25.f / gemTex.getSize().y;
        gem.setScale({ gScale, gScale });
        gem.setPosition({ 650.f, 8.f });
        window.draw(gem);

        hudText.setFillColor(Color(0, 255, 255));
        hudText.setString("x" + to_string(gems));
        hudText.setPosition({ 685.f, 8.f });
        window.draw(hudText);

        // --- LEVEL COMPLETION CHECK ---
        bool allDead = true;
        
        // 1. Check regular enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].isAlive()) { allDead = false; break; }
        }
        
        // 2. Check Foogas
        if (allDead) {
            for (int i = 0; i < 4; i++) {
                if (fooga[i].isAlive()) { allDead = false; break; }
            }
        }


        if (allDead) {
            levelNo++;
            if (levelNo > 10) {
                currentState = GameState::Victory;
                levelNo = 1;
            } else {
                db.saveProgress(username, levelNo, lives, gems, score);
                LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                spawnEnemies(enemies, levelNo);
                spawnFoogas(fooga);
                play.setPos(12, 5);
                if (isMultiplayer) play2.setPos(12, 7);
            }
        }

        window.display();
        }
        break;

            case GameState::CharacterSelect:
            {
                int selectedChar = 1; // 1 = Nick, 2 = Tom, 3 = SnowB
                if (Keyboard::isKeyPressed(Keyboard::Key::Left)) selectedChar = 1;
                if (Keyboard::isKeyPressed(Keyboard::Key::Down)) selectedChar = 2;
                if (Keyboard::isKeyPressed(Keyboard::Key::Right)) selectedChar = 3;

                while (true) {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Enter) {
                            // Apply Stats based on character
                            levelNo = 1;
                            lives = 3;
                            lives2 = 3;
                            score = 0;
                            gems = 100;
                            if (selectedChar == 1) { /* Nick - Default */ }
                            if (selectedChar == 2) { /* Tom - Faster */ play.setSpeed(3.0f); }
                            if (selectedChar == 3) { /* SnowB - Range */ }
                            
                            LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                            spawnEnemies(enemies, levelNo);
                            spawnFoogas(fooga);
                             play.setPos(12, 5);
                             if (isMultiplayer) play2.setPos(12, 7);
                             currentState = GameState::Playing;
                        }
                        if (keyPressed->code == Keyboard::Key::Escape) currentState = GameState::MainMenu;
                    }
                }

                window.clear(Color(30, 30, 60));
                Text selectTitle(font, "SELECT YOUR SNOWMAN", 40);
                selectTitle.setFillColor(Color::Yellow);
                selectTitle.setPosition({ 150.f, 50.f });
                window.draw(selectTitle);

                // Nick
                Text nick(font, "NICK\nSPEED: 1.0\nRANGE: 1.0");
                nick.setPosition({ 100.f, 200.f });
                nick.setFillColor(selectedChar == 1 ? Color::Cyan : Color::White);
                window.draw(nick);

                // Tom
                Text tom(font, "TOM\nSPEED: 1.5\nRANGE: 0.8");
                tom.setPosition({ 350.f, 200.f });
                tom.setFillColor(selectedChar == 2 ? Color::Cyan : Color::White);
                window.draw(tom);

                // SnowB
                Text snowb(font, "SNOWB\nSPEED: 0.8\nRANGE: 1.5");
                snowb.setPosition({ 600.f, 200.f });
                snowb.setFillColor(selectedChar == 3 ? Color::Cyan : Color::White);
                window.draw(snowb);

                Text promptTxt(font, "USE ARROWS TO NAVIGATE | ENTER TO START", 18);
                promptTxt.setPosition({ 180.f, 500.f });
                window.draw(promptTxt);

                window.display();
            }
            break;

            case GameState::Paused:
            {
                static int hoveredOption = 1; // Persistent choice

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Up) hoveredOption = (hoveredOption == 1) ? 3 : hoveredOption - 1;
                        if (keyPressed->code == Keyboard::Key::Down) hoveredOption = (hoveredOption == 3) ? 1 : hoveredOption + 1;

                        if (keyPressed->code == Keyboard::Key::Enter) {
                            if (hoveredOption == 1) currentState = GameState::Playing;
                            else if (hoveredOption == 2) {
                                previousState = GameState::Paused; // Mark where we came from
                                currentState = GameState::Shop;
                            }
                            else currentState = GameState::MainMenu;
                        }
                        if (keyPressed->code == Keyboard::Key::P || keyPressed->code == Keyboard::Key::Escape) {
                            currentState = GameState::Playing;
                        }
                    }
                }

                // 1. Draw game state in background (frozen)
                window.clear(Color::Black);
                window.draw(background);
                for (int i = 0; i < count; i++) {
                    tilt[i].Draw(window);
                }
                Draw(MAX_ENEMIES, enemies, window);
                Draw(4, fooga, window);
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (enemies[i].isAlive() && enemies[i].isFrozen()) {
                        window.draw(enemies[i].getFrozenOverlay());
                    }
                }
                for (int i = 0; i < 4; i++) {
                    if (fooga[i].isAlive() && fooga[i].isFrozen()) {
                        window.draw(fooga[i].getFrozenOverlay());
                    }
                }
                for (int i = 0; i < MAX_SNOWBALLS; i++) {
                    snowballs[i].Draw(window);
                }
                window.draw(play.getSprite());

                // 2. Dim overlay (Full Screen)
                RectangleShape dim({ 800.f, 600.f });
                dim.setFillColor(Color(0, 0, 0, 180));
                window.draw(dim);

                // 3. Central Menu Box (Slightly taller for 3 options)
                RectangleShape menuBox({ 400.f, 350.f });
                menuBox.setFillColor(Color(20, 20, 50, 230));
                menuBox.setOutlineColor(Color::White);
                menuBox.setOutlineThickness(2.f);
                menuBox.setPosition({ 200.f, 125.f });
                window.draw(menuBox);

                Text pauseText(font, "PAUSED", 50);
                pauseText.setFillColor(Color::Yellow);
                pauseText.setStyle(Text::Bold);
                FloatRect pBounds = pauseText.getGlobalBounds();
                pauseText.setPosition({ (800.f - pBounds.size.x) / 2.f, 150.f });
                window.draw(pauseText);

                pauseText.setCharacterSize(25);
                pauseText.setStyle(Text::Regular);
                
                // Option 1: Resume
                pauseText.setString("RESUME");
                pauseText.setFillColor(hoveredOption == 1 ? Color::Cyan : Color::White);
                FloatRect rBounds = pauseText.getGlobalBounds();
                pauseText.setPosition({ (800.f - rBounds.size.x) / 2.f, 240.f });
                window.draw(pauseText);

                // Option 2: Shop
                pauseText.setString("SHOP");
                pauseText.setFillColor(hoveredOption == 2 ? Color::Cyan : Color::White);
                FloatRect sBounds = pauseText.getGlobalBounds();
                pauseText.setPosition({ (800.f - sBounds.size.x) / 2.f, 300.f });
                window.draw(pauseText);

                // Option 3: Main Menu
                pauseText.setString("MAIN MENU");
                pauseText.setFillColor(hoveredOption == 3 ? Color::Cyan : Color::White);
                FloatRect mBounds = pauseText.getGlobalBounds();
                pauseText.setPosition({ (800.f - mBounds.size.x) / 2.f, 360.f });
                window.draw(pauseText);

                window.display();
            }
            break;

            case GameState::Victory:
            {
                while (true) {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Enter || keyPressed->code == Keyboard::Key::Escape) {
                            currentState = GameState::MainMenu;
                        }
                    }
                }

                window.clear(Color(0, 60, 0)); // Dark Green for Victory
                Text winText(font);
                winText.setOutlineColor(Color::Black);
                winText.setOutlineThickness(2.f);

                winText.setCharacterSize(60);
                winText.setFillColor(Color::Yellow);
                winText.setString("CONGRATULATIONS!");
                FloatRect b1 = winText.getGlobalBounds();
                winText.setPosition({ (800.f - b1.size.x) / 2.f, 100.f });
                window.draw(winText);

                winText.setCharacterSize(30);
                winText.setFillColor(Color::White);
                winText.setString("YOU SAVED THE SNOW WORLD!");
                FloatRect b2 = winText.getGlobalBounds();
                winText.setPosition({ (800.f - b2.size.x) / 2.f, 220.f });
                window.draw(winText);

                winText.setString("FINAL SCORE: " + to_string(score));
                FloatRect b3 = winText.getGlobalBounds();
                winText.setPosition({ (800.f - b3.size.x) / 2.f, 320.f });
                window.draw(winText);

                winText.setCharacterSize(22);
                winText.setFillColor(Color(200, 200, 200));
                winText.setString("PRESS ENTER FOR MAIN MENU");
                FloatRect b4 = winText.getGlobalBounds();
                winText.setPosition({ (800.f - b4.size.x) / 2.f, 500.f });
                window.draw(winText);

                window.display();
            }
            break;

            case GameState::GameOver:
            {
                static int hoveredOption = 1; // Persistent selection

                while (true)
                {
                    auto event = window.pollEvent();
                    if (!event.has_value()) break;
                    if (event->is<Event::Closed>()) window.close();
                    
                    if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
                        if (keyPressed->code == Keyboard::Key::Up) hoveredOption = 1;
                        if (keyPressed->code == Keyboard::Key::Down) hoveredOption = 2;

                        if (keyPressed->code == Keyboard::Key::Enter) {
                            if (hoveredOption == 1) {
                                // Reset Game State
                                levelNo = 1;
                                lives = 3;
                                lives2 = 3;
                                score = 0;
                                gems = 100;
                                LoadLevel(levelNo, currentLevel, bgTex, background, tilt, count);
                                spawnEnemies(enemies, levelNo);
                                spawnFoogas(fooga);
                                play.setPos(12, 5);
                                if (isMultiplayer) play2.setPos(12, 7);
                                currentState = GameState::Playing;
                            } else {
                                currentState = GameState::MainMenu;
                            }
                        }
                    }
                }

                window.clear(Color(80, 0, 0)); // Dark Red
                Text loseText(font);
                loseText.setOutlineColor(Color::Black);
                loseText.setOutlineThickness(2.f);

                loseText.setCharacterSize(70);
                loseText.setFillColor(Color::White);
                loseText.setString("GAME OVER");
                FloatRect b1 = loseText.getGlobalBounds();
                loseText.setPosition({ (800.f - b1.size.x) / 2.f, 120.f });
                window.draw(loseText);

                loseText.setCharacterSize(35);
                loseText.setString("RETRY");
                loseText.setFillColor(hoveredOption == 1 ? Color::Yellow : Color(180, 180, 180));
                FloatRect b2 = loseText.getGlobalBounds();
                loseText.setPosition({ (800.f - b2.size.x) / 2.f, 320.f });
                window.draw(loseText);

                loseText.setString("MAIN MENU");
                loseText.setFillColor(hoveredOption == 2 ? Color::Yellow : Color(180, 180, 180));
                FloatRect b3 = loseText.getGlobalBounds();
                loseText.setPosition({ (800.f - b3.size.x) / 2.f, 390.f });
                window.draw(loseText);

                window.display();
            }
            break;
        }
    }

    delete[] tilt;
    tilt = nullptr;
    return 0;
}

void LoadLevel(int levelNo, Level& level, Texture& bgTex, Sprite& background, Tiles*& tilt, int& tileCount) {
    if (levelNo == 10) Mogera::bossHp = 900;
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
    background.setTexture(bgTex, true);
    if (bgTex.getSize().x > 0) {
        background.setScale({ 800.f / bgTex.getSize().x, 600.f / bgTex.getSize().y });
    }

    if (tilt) delete[] tilt;
    tileCount = 0;
    for (int r = 0; r < Level::ROWS; r++)
        for (int c = 0; c < Level::COLS; c++)
            if (level.grid[r][c] != 0) tileCount++;

    tilt = new Tiles[tileCount];
    int idx = 0;
    for (int r = 0; r < Level::ROWS; r++)
        for (int c = 0; c < Level::COLS; c++)
            if (level.grid[r][c] != 0) {
                tilt[idx].setpost(c * Level::TILE_W, r * Level::TILE_H);
                idx++;
            }
}


void mover(int n, Botom* other, Tiles* tiles, int tileCount) {
    for (int i = 0; i < n; i++) {
        if (other[i].isAlive()) other[i].movement(tiles, tileCount);
    }
}

void Draw(int n, Botom* other, RenderWindow& window) {
    for (int i = 0; i < n; i++) {
        if (other[i].isAlive() || other[i].isDying()) {
            window.draw(other[i].getSprite());
            if (other[i].isFrozen()) {
                window.draw(other[i].getFrozenOverlay());
            }
        }
    }
}

void mover(int n, Fooga* other, Tiles* tiles, int tileCount) {
    for (int i = 0; i < n; i++) {
        if (other[i].isAlive()) other[i].movement(tiles, tileCount);
    }
}

void Draw(int n, Fooga* other, RenderWindow& window) {
    for (int i = 0; i < n; i++) {
        if (other[i].isAlive() || other[i].isDying()) {
            window.draw(other[i].getSprite());
            if (other[i].isFrozen()) {
                window.draw(other[i].getFrozenOverlay());
            }
        }
    }
}

void Draw(int n, Tornado* other, RenderWindow& window) {
    for (int i = 0; i < n; i++) {
        if (other[i].isAlive() || other[i].isDying()) {
            window.draw(other[i].getSprite());
            if (other[i].isFrozen()) {
                window.draw(other[i].getFrozenOverlay());
            }
        }
    }
}

void mover(int n, Tornado* other, Tiles* tiles, int tileCount) {
    for (int i = 0; i < n; i++) {
        if (other[i].isAlive()) other[i].movement_spin(tiles, tileCount);
    }
}

void Gravity(int n, Botom* other, Tiles* tiles, int c) {
    for (int i = 0; i < n; i++) {
        if (other[i].isAlive()) other[i].applyGravity(tiles, c);
    }
}

void Gravity(int n, Fooga* other, Tiles* tiles, int c) {
    for (int i = 0; i < n; i++) {
        if (other[i].isAlive()) other[i].applyGravity(tiles, c);
    }
}

void Gravity(Tornado* other, int n, Tiles* tiles, int c) {
    for (int i = 0; i < n; i++) {
        if (other[i].isAlive()) other[i].applyGravity(tiles, c);
    }
}


