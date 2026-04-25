#include "src/Game.h"
#include <cstdlib>
#include <ctime>





class Botom {
    protected:
    bool mv1 = false;
    Texture tex[4];
    Clock clocks;
    int frames = 0;
    float frametime = 0.15f;
    Sprite* enemy;
    FloatRect bounds;
    int pausef;
    public:
        Botom() {
            tex[0].loadFromFile("botom_orange/botom_orange_walk_11.png");
            tex[1].loadFromFile("botom_orange/botom_orange_walk_12.png");
            tex[2].loadFromFile("botom_orange/botom_orange_walk_13.png");
            tex[3].loadFromFile("botom_orange/Botom_bruh.png");
            enemy = new Sprite(tex[0]);
    }
        void movement() {
            if (pausef > 0) {
                enemy->setTexture(tex[3]);
                pausef--;
            }
           else{
            if ( mv1 == true) {
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
        
        
            if (enemy->getPosition().x >= 1150 && enemy->getPosition().x <= 1200){
                mv1 = true;
                enemy->setTexture(tex[3]);
                pausef = 60;
                }
            if (enemy->getPosition().x >= 0 && enemy->getPosition().x <= 50){
                mv1 = false;
                enemy->setTexture(tex[3]);
                pausef = 60;
                }
            }
            
            
    }
        Sprite getEnemy() {
        return *enemy;
    }
        void setPos(float i) {
            enemy->setPosition({ 350.f, 65.f + i * 65.f });
    }
        void setOrigin(float x, float y) {
            enemy->setOrigin({x,y});
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
        other[i].setOrigin(x,y);
        other[i].setPos(i);
        if(i % 2 == 1)
        other[i].setmv1(false);
        else
        other[i].setmv1(true);
    }
}

//class Tiles {
//    private:
//    Texture text;
//    Sprite* tile;
//    FloatRect bounds;
//    public:
//        Tiles() {
//            text.loadFromFile("tileset/tile_1.png");
//            tile = new Sprite(text);
//    }
//        void setpost(int i) {
//            tile->setPosition({0.f+i*74.6f, 540.f});
//    }
//        void setsca(float x, float y) {
//            tile->setScale({95.f/x, 90.f/y});
//    }
//        void Draw(RenderWindow& window) {
//            window.draw(*tile);
//    }
//        FloatRect boun()  {
//            return tile->getGlobalBounds();
//        }
//        ~Tiles() {
//            delete tile;
//            tile = nullptr;
//    }
//};



class Player : public Botom {
    protected:
    bool isJump;
    public:
        Player() {
            tex[0].loadFromFile("nick/nick_1.png");
            tex[1].loadFromFile("nick/nick_2.png");
            tex[2].loadFromFile("nick/nick_3.png");
            enemy = new Sprite(tex[0]);
    }
        void move(Keyboard::Key key) {
            if (key == Keyboard::Key::A) {
                enemy->move({ -2.25f, -0.0f });
                enemy->setScale({ 210.f / 620, 267.f / 550 });
                if (clocks.getElapsedTime().asSeconds() > frametime) {
                    frames++;
                    if (frames >= 3)
                        frames = 0;
                    enemy->setTexture(tex[frames]);
                    clocks.restart();
                }
            }
            if (key == Keyboard::Key::D ) {
                enemy->move({ +2.25f, -0.0f });
                enemy->setScale({ -210.f / 620, 267.f / 550 });
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
        void setsca(float x, float y) {
            enemy->setScale({ 210.f / x, 267.f / y });
        }
        void setpost() {
            enemy->setPosition({ 110.25f, 187.43f });
        }
        ~Player() {
            delete enemy;
            enemy = nullptr;
    }
};


int main()
{
    cout << "How many botoms do you want to create? ";
    int opt;
    cin >> opt;
    RenderWindow window(VideoMode({ 1200u,800u }), "HOPE");
    //------------- for background --------------
    Texture wintex;
    wintex.loadFromFile("Maps/Map_1.png");
    Sprite background(wintex);
    background.setScale({1200.f/wintex.getSize().x, 800.f/wintex.getSize().y});
    //------------ End of background ------------

    //---------------- For tiles ---------------
   /* Texture tiles;
    tiles.loadFromFile("tileset/tile_1.png");
    Tiles tilt[12];
    for(int i = 0; i < 12; i++)
    tilt[i].setsca(121.f, 121.f);
    for (int i = 0; i < 12; i++) {
        if (i < 4) {
            tilt[i].setpost(i);
        }
        else if(i >= 4 && i <= 7)
        tilt[i].setpost(i+2);
        else
        tilt[i].setpost(i+4);
    }*/
  
    //--------------- End of tiles -----------------
    Botom* botom = new Botom[opt];
    float x = 97.f/2.f;
    float y = 89.f/2.f;
    setPos(x, y, opt, botom);

    //--------------- Player -----------------
    Player play;
    x = 210.f/2.f;
    y = 267.f/2.f;
    play.setOrigin(x, y);
    play.setPos(4);
    play.setsca(620.f, 550.f);
    
    //------------- Player End----------------
    window.setFramerateLimit(60.0f);
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) {
            play.move(keyPressed->code);
                }
        }
        mover(opt, botom);
       
        window.clear(Color::Black);
         Draw(opt, botom, window);
      window.draw(play.Draw());
        window.display();
        for (int i = 0; i < opt; i++) {
            if (play.boun().findIntersection(botom[i].boun())) {
                if (botom[i].getmv1() == false)
                    botom[i].setmv1(true);
                else if (botom[i].getmv1() == true)
                    botom[i].setmv1(false);
            }
        }
    }
    delete[] botom;
  /*  delete[] tile;
    tile = nullptr;*/
    botom = nullptr;
    return 0;
}

//------------ For drawing ugly mfs and tiles
/* for (int i = 0; i < 12; i++) {
           tilt[i].Draw(window);
       }*/
     

// ------------- For bounary checking-------------
/*for (int i = 0; i < opt; i++) {
            for (int j = 0; j < 12; j++) {
                if (botom[i].boun().findIntersection(tilt[j].boun())) {
                    if (botom[i].getmv1() == false)
                        botom[i].setmv1(true);
                    else if (botom[i].getmv1() == true)
                        botom[i].setmv1(false);
                }
            }
        }*/

//player movement
  /* if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
            player.move({ +0.0f, -2.25f });
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
            player.move({ +0.0f, +2.25f });
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
            player.move({ -2.25f, -0.0f });
            player.setScale({ 1.f,1.f });
            if (clock.getElapsedTime().asSeconds() > frametime) {
                frame++;
                if (frame >= 3)
                    frame = 0;
                player.setTexture(tex[frame]);
                clock.restart();
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
            player.move({ +2.25f, -0.0f });
            player.setScale({-1.f,1.f});
            if (clock.getElapsedTime().asSeconds() > frametime) {
            frame++;
            if(frame >= 3)
            frame = 0;
            player.setTexture(tex[frame]);
            clock.restart();
            }
        }*/

       /* c++;
        if (c >= 60 && c % 60 == 0)
            cout << "Fps = 60\n";*/
/*if (player.getPosition().x <= 800 && mv == true) {
    player.move({ -2.25f, -0.0f });
    player.setScale({ 1.f,1.f });
    if (clock.getElapsedTime().asSeconds() > frametime) {
        frame++;
        if (frame >= 3)
            frame = 0;
        player.setTexture(tex[frame]);
        clock.restart();

    }
}
else if (player.getPosition().x >= 0 && mv == false) {
    player.move({ +2.25f, -0.0f });
    player.setScale({ -1.f,1.f });
    if (clock.getElapsedTime().asSeconds() > frametime) {
        frame++;
        if (frame >= 3)
            frame = 0;
        player.setTexture(tex[frame]);
        clock.restart();

    }
}
if (player.getPosition().x == 800)
mv = true;
if (player.getPosition().x >= 0 && player.getPosition().x <= 15)
mv = false;*/