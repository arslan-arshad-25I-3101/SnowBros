#include "src/Game.h"
#include <cstdlib>
#include <ctime>


int main()
{
    RenderWindow window(VideoMode({800u,600u}), "HOPE");
    float r = 45.0f;
    int c = 0;
    Texture tex[3];
    tex[0].loadFromFile("botom_orange/botom_orange_walk_11.png");
    tex[1].loadFromFile("botom_orange/botom_orange_walk_12.png");
    tex[2].loadFromFile("botom_orange/botom_orange_walk_13.png");
    Sprite player(tex[0]);
    player.setOrigin({97.f/2.f, 89.f/2.f});
    player.setPosition({350.f, 200.f});
    Clock clock;
    int frame = 0;
    float frametime = 0.15f;
    window.setFramerateLimit(60.0f);
   
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                if(r >= 0)
                r -= 3;
                else
                cout << "No thx\n";
                    
                }
            }
        }
       /* if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
            shape.move({ +0.0f, -2.25f });
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
            shape.move({ +0.0f, +2.25f });
        }*/
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
        }
        window.clear(Color::Black);
        window.draw(player);
        window.display();
        c++;
        if(c >= 60 && c%60 == 0)
        cout << "Fps = 60\n";
    }
    return 0;
}
