#include "Apollo13.h"

int main()
{
    // Create a new loop.
    Apollo13 game;

    // Open window.
    game.setWindow(make<EGE::SFMLSystemWindow>(sf::VideoMode(500, 500), "Apollo13"));

    // Run game.
    return game.run();
}
