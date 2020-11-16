#pragma once

#include "A13GameplayObjectManager.h"
#include "A13FactoryBuilding.h"
#include "ResourceStatsWidget.h"

class A13GUIFactoryBuilder : public A13GUIAbstractBuilder<A13FactoryTilemap, A13FactoryBuildingItem>
{
public:
    A13GUIFactoryBuilder(EGE::GUIGameLoop* loop)
    : A13GUIAbstractBuilder(loop, make<A13FactoryTilemap>())
    {
        setBuilderBackground("gui/factory_builder/background.png");

        // main layer
        setBuilderAtlas("gui/factory_builder/builder_atlas.png");
        // additional layers
        setBuilderAtlas("gui/factory_builder/terrain.png", FACTORY_BUILDER_LAYER_TERRAIN);
        setBuilderAtlas("gui/factory_builder/ores.png", FACTORY_BUILDER_LAYER_ORES);
        setBuilderAtlas("gui/factory_builder/shadows.png", FACTORY_BUILDER_LAYER_SHADOWS);

        setSelectorAtlas("gui/factory_builder/items.png");

        setGPO(&A13GameplayObjectManager::instance().factoryBuildingItems);
        setUseEnsure(true);
        m_tilemap->setTileSize(EGE::Vec2u(16, 16));
        m_tilemap->setGenerator(
            [](EGE::Vec2i chunkPos, A13FactoryTilemap::ChunkType& chunk)
            {
                // TODO: replace it by better generator
                for(EGE::Size x = 0; x < 16; x++)
                for(EGE::Size y = 0; y < 16; y++)
                {
                    A13FactoryTilemap::StateType& state = chunk.getTile({x, y});
                    state.addObjs[FACTORY_BUILDER_LAYER_TERRAIN] = EGE::Random::fastRandom().nextInt(5) ? TERRAIN_GRASS : TERRAIN_WILD_GRASS;

                    // ores
                    if(EGE::Random::fastRandom().nextInt(21) == 1)
                    {
                        Ore* ore = (Ore*)&state.addObjs[FACTORY_BUILDER_LAYER_ORES];
                        ore->id = EGE::Random::fastRandom().nextInt(40) / 4;
                        ore->count = EGE::Random::fastRandom().nextIntRanged(1024, MAX_ORE_COUNT);
                    }
                }
            }
        );
    }

    virtual EGE::Vec2d atlasMapper(A13FactoryTilemap* tilemap, const A13FactoryTilemap::TileType& state, EGE::Size addLayer) override
    {
        if(addLayer == FACTORY_BUILDER_LAYER_TERRAIN)
        {
            EGE::Uint64 index = state.addObjs[addLayer];
            return EGE::Vec2d(0, index * 16);
        }
        else if(addLayer == FACTORY_BUILDER_LAYER_ORES)
        {
            Ore* ore = (Ore*)&state.addObjs[addLayer];
            return ore->id != ORE_NONE ? EGE::Vec2d(ore->id * 16, ((ore->count * 7 / MAX_ORE_COUNT) + 1) * 16) : EGE::Vec2d(0, 0);
        }
        return EGE::Vec2d(0, 0);
    }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13FactoryTilemap::TileType& tile, A13FactoryBuildingItem* item)
    {
        return item->canPlaceHere(tileRel, tile);
    }

    virtual void onLoad()
    {
        A13GUIAbstractBuilder::onLoad();

        m_resourceStatsWidget = make<ResourceStatsWidget>(this);
        addWidget(m_resourceStatsWidget);
    }

    virtual void onResize(sf::Event::SizeEvent& event)
    {
        A13GUIAbstractBuilder::onResize(event);

        EGE::Size RESOURCE_STATS_SIZE = 80;
        m_resourceStatsWidget->setPosition({event.width - RESOURCE_STATS_SIZE, 0});
        m_resourceStatsWidget->setSize({RESOURCE_STATS_SIZE, event.height});
    }

private:
    EGE::SharedPtr<ResourceStatsWidget> m_resourceStatsWidget;
};
