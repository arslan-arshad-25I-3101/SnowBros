#include <SFML/Graphics.hpp>

int main() {
    // 1. Create the game window (Width, Height, Title)
    sf::RenderWindow window(sf::VideoMode(800, 600), "Snow Bros - OOP Project");

    // This is the Game Loop! 
    while (window.isOpen()) {

        // 2. Handle Events (Input)
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 3. Update Game Logic (Movement, Collisions, etc.)
        // We'll put our Snowmen and Enemies logic here later!

        // 4. Render (Draw everything)
        window.clear(sf::Color::Black); // Clear screen with a color

        // Draw your game objects here:
        // window.draw(player);

        window.display(); // Swap buffers to show the new frame
    }

    return 0;
}