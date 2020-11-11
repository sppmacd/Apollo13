#include "Apollo13.h"

int main()
{
    // Create a new loop.
    Apollo13 game;

    // Open window.
    game.setWindow(make<EGE::SFMLSystemWindow>(sf::VideoMode(500, 500), "Apollo13"));

    // Set bg color
    game.setBackgroundColor(sf::Color(192, 228, 237));

    // Run game.
    return game.run();
}
