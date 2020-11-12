#pragma once

#include "A13GameplayObjectManager.h"
#include "A13GUIAbstractBuilder.h"
#include "A13RocketPart.h"

typedef Aliases::A13FixedTilemapForPart<A13RocketPart, 32, 64> A13GUIProjectBuilder_Tilemap;

class A13GUIProjectBuilder : public A13GUIAbstractBuilder<A13GUIProjectBuilder_Tilemap>
{
public:
    A13GUIProjectBuilder(EGE::GUIGameLoop* loop)
    : A13GUIAbstractBuilder(loop, make<A13GUIProjectBuilder_Tilemap>())
    {
        setBuilderBackground("gui/pb/background.png");
        setBuilderAtlas("gui/pb/rocket_parts.png");
        setSelectorAtlas("gui/pb/rocket_parts.png");
        setGPO(&A13GameplayObjectManager::instance().rocketParts);
        m_tilemap->setTileSize(EGE::Vec2u(16, 16));
    }
};
