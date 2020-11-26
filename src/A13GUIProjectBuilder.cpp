#include "A13GUIProjectBuilder.h"

#include "A13GameplayObjectManager.h"
#include "Apollo13.h"

A13GUIProjectBuilder::A13GUIProjectBuilder(EGE::GUIScreen* loop)
: A13GUIAbstractBuilder(loop, Apollo13::instance().save.projectTilemap())
{
    setBuilderBackground("gui/pb/background.png");
    setBuilderAtlas("gui/pb/rocket_parts.png");
    setSelectorAtlas("gui/pb/items.png");
    setGPO(&A13GameplayObjectManager::instance().rocketPartItems);
    m_tilemap->setTileSize(EGE::Vec2u(16, 16));

    for(auto it: m_tilemap->getParts())
    {
        m_totalItems.tryAddItems(it.second->part->getCost());
    }
}

void A13GUIProjectBuilder::onKeyPress(sf::Event::KeyEvent& event)
{
    if(event.code == sf::Keyboard::Escape && !m_partSelector->getCurrentItem())
    {
        exitDialog(0);
    }

    A13GUIAbstractBuilder::onKeyPress(event);
}

void A13GUIProjectBuilder::onLoad()
{
    A13GUIAbstractBuilder::onLoad();

    m_resourceStatsWidget = make<ResourceStatsWidget>(this, m_totalItems);
    addWidget(m_resourceStatsWidget);
}
