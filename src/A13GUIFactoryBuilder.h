#pragma once

#include "A13GameplayObjectManager.h"
#include "A13FactoryBuilding.h"
#include "A13GUIAbstractBuilder.h"

typedef Aliases::A13ChunkedTilemapForPart<A13FactoryBuilding, 16, 16> A13GUIFactoryBuilder_Tilemap;

class A13GUIFactoryBuilder : public A13GUIAbstractBuilder<A13GUIFactoryBuilder_Tilemap>
{
public:
    A13GUIFactoryBuilder(EGE::GUIGameLoop* loop)
    : A13GUIAbstractBuilder(loop, make<A13GUIFactoryBuilder_Tilemap>())
    {
        setBuilderBackground("gui/factory_builder/background.png");
        setBuilderAtlas("gui/factory_builder/builder_atlas.png");
        setSelectorAtlas("gui/factory_builder/builder_atlas.png");
        setGPO(&A13GameplayObjectManager::instance().factoryBuildings);
        setUseEnsure(true);
        m_tilemap->setTileSize(EGE::Vec2u(16, 16));
    }
};
