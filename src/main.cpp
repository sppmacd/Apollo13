#include <ege/gui/GUIGameLoop.h>

int main()
{
    EGE::GUIGameLoop loop;
    loop.setWindow(make<EGE::SFMLSystemWindow>(sf::VideoMode(500, 500), "Apollo13"));
    return loop.run();
}
