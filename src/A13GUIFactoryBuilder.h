#pragma once

#include "A13GameplayObjectManager.h"
#include "A13FactoryBuilding.h"

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
                    chunk.getTile({x, y}).addObjs[FACTORY_BUILDER_LAYER_TERRAIN] = EGE::Random::fastRandom().nextInt(5) ? TERRAIN_GRASS : TERRAIN_WILD_GRASS;
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
            Ore* ore = &state.addObjs[addLayer];
            return EGE::Vec2d(ore->type * 16, (ore->count * 8 / MAX_ORE_COUNT) * 16);
        }
        return EGE::Vec2d(0, 0);
    }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13FactoryTilemap::TileType& tile, A13FactoryBuildingItem* item)
    {
        return item->canPlaceHere(tileRel, tile);
    }
};
