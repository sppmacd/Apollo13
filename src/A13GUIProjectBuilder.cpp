#include "A13GUIProjectBuilder.h"

#include "A13GameplayObjectManager.h"

A13GUIProjectBuilder::A13GUIProjectBuilder(EGE::GUIGameLoop* loop)
: A13GUIAbstractBuilder(loop, make<A13ProjectTilemap>())
{
    setBuilderBackground("gui/pb/background.png");
    setBuilderAtlas("gui/pb/rocket_parts.png");
    setSelectorAtlas("gui/pb/rocket_parts.png");
    setGPO(&A13GameplayObjectManager::instance().rocketPartItems);
    m_tilemap->setTileSize(EGE::Vec2u(16, 16));
}
