#include "Apollo13.h"

int main()
{
    // Create an instance of game by referring to it.
    Apollo13::instance();

    // Open window.
    Apollo13::instance().setWindow(make<EGE::SFMLSystemWindow>(sf::VideoMode::getFullscreenModes()[0], "Apollo 13", sf::Style::Fullscreen));

    // Set bg color
    Apollo13::instance().setBackgroundColor(sf::Color(192, 228, 237));

    // Run game.
    return Apollo13::instance().run();
}
