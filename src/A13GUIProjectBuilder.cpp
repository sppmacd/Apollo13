#include "A13GUIProjectBuilder.h"

#include "A13GameplayObjectManager.h"

A13GUIProjectBuilder::A13GUIProjectBuilder(EGE::GUIScreen* loop)
: A13GUIAbstractBuilder(loop, make<A13ProjectTilemap>())
{
    setBuilderBackground("gui/pb/background.png");
    setBuilderAtlas("gui/pb/rocket_parts.png");
    setSelectorAtlas("gui/pb/items.png");
    setGPO(&A13GameplayObjectManager::instance().rocketPartItems);
    m_tilemap->setTileSize(EGE::Vec2u(16, 16));
}

void A13GUIProjectBuilder::onKeyPress(sf::Event::KeyEvent& event)
{
    if(event.code == sf::Keyboard::Escape && !m_partSelector->getCurrentItem())
    {
        exitDialog(0);
    }

    A13GUIAbstractBuilder::onKeyPress(event);
}
