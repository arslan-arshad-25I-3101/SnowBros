#include "src/Game.h"
#include <cstdlib>
#include <ctime>


int main()
{
    RenderWindow window(VideoMode({800u,600u}), "HOPE");
    float r = 45.0f;
    int c = 0;
    CircleShape shape(r);
    shape.setPosition({250.0f, 125.0f});
    window.setFramerateLimit(60.0f);
    float x = shape.getPosition().x;
    float y = shape.getPosition().y;
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
                    shape.setRadius(r);
                }
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
            shape.move({+0.0f, -2.25f});
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
                shape.move({ +0.0f, +2.25f });
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
                shape.move({ -2.25f, -0.0f });
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
                shape.move({ +2.25f, -0.0f });
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::Space)) {
               
                shape.move({+0.0f, -5.5f});
                if(c >= 60 && x != shape.getPosition().x && y != shape.getPosition().y)
                shape.move({+0.0, +5.5});
            }
        }
        window.clear(Color::Black);
        window.draw(shape);
        window.display();
        c++;
        if(c >= 60 && c%60 == 0)
        cout << "Fps = 60\n";
    }
    return 0;
}
