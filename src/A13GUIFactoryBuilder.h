#pragma once

#include "Apollo13.h"
#include "A13GameplayObjectManager.h"
#include "A13FactoryBuilding.h"
#include "PlayerStats.h"
#include "ResourceStatsWidget.h"

class A13GUIFactoryBuilder : public A13GUIAbstractBuilder<A13::FactoryTilemap, A13FactoryBuildingItem>
{
public:
    A13GUIFactoryBuilder(EGE::GUIGameLoop* loop)
    : A13GUIAbstractBuilder(loop, make<A13::FactoryTilemap>(Apollo13::instance().getSeed()))
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
            [this](EGE::Vec2i chunkPos, A13::FactoryTilemap::ChunkType& chunk)
            {
                EGE::Random random(chunkPos.x * 1024 * 1024 + chunkPos.y * 1024 + m_tilemap->seed);

                // TODO: replace it by better generator
                for(EGE::Size x = 0; x < 16; x++)
                for(EGE::Size y = 0; y < 16; y++)
                {
                    A13::FactoryTilemap::StateType& state = chunk.getTile({x, y});
                    state.addObjs[FACTORY_BUILDER_LAYER_TERRAIN] = random.nextInt(5) ? TERRAIN_GRASS : TERRAIN_WILD_GRASS;
                }

                // ores
                if(random.nextInt(21) == 1)
                {
                    int id = random.nextIntRanged(1, 10);

                    EGE::Vec2i pos(random.nextInt(16),
                        random.nextInt(16));

                    for(int s = random.nextIntRanged(20, 50); s >= 0; s--)
                    {
                        switch(random.nextInt(4))
                        {
                            case 0: pos.x++; break;
                            case 1: pos.x--; break;
                            case 2: pos.y++; break;
                            case 3: pos.y--; break;
                        }

                        const A13::FactoryTilemap::StateType& state = m_tilemap->ensureTile({chunkPos.x * 16 + pos.x, chunkPos.y * 16 + pos.y});
                        A13::Ore* ore = (A13::Ore*)&state.addObjs[FACTORY_BUILDER_LAYER_ORES];

                        if(ore->id != ORE_NONE)
                        {
                            s++;
                            continue;
                        }

                        ore->id = id;
                        ore->count = random.nextIntRanged(512, MAX_ORE_COUNT);
                    }
                }
            }
        );
    }

    virtual EGE::Vec2d atlasMapper(A13::FactoryTilemap* tilemap, const A13::FactoryTilemap::TileType& state, EGE::Size addLayer) override
    {
        if(addLayer == FACTORY_BUILDER_LAYER_TERRAIN)
        {
            EGE::Uint64 index = state.addObjs[addLayer];
            return EGE::Vec2d(0, index * 16);
        }
        else if(addLayer == FACTORY_BUILDER_LAYER_ORES)
        {
            A13::Ore* ore = (A13::Ore*)&state.addObjs[addLayer];
            return ore->id != ORE_NONE ? EGE::Vec2d(ore->id * 16, ((ore->count * 7 / MAX_ORE_COUNT) + 1) * 16) : EGE::Vec2d(0, 0);
        }
        return EGE::Vec2d(0, 0);
    }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13::FactoryTilemap::TileType& tile, A13FactoryBuildingItem* item)
    {
        return item->canPlaceHere(tileRel, tile);
    }

    virtual void onLoad()
    {
        A13GUIAbstractBuilder::onLoad();

        m_partSelector->setCustomRenderer([this](sf::RenderTarget& target, A13FactoryBuildingItem* item)
        {
            EGE::Renderer renderer(target);
            const unsigned HEIGHT = 40;

            // Background
            renderer.renderRectangle(m_partSelector->getSize().x, 0, 230, m_partSelector->getSize().y, sf::Color(0, 0, 0, 127));

            // Item name
            renderer.renderText(m_partSelector->getSize().x + 10, 20, *m_font, item->getId(), 16);

            // Cost
            Cost cost = item->getCost();

            EGE::Size s = 0;
            for(ResourceItemStack& stack: cost)
            {
                std::string str = stack.getItem()->getId() + " x" + std::to_string(stack.getItemCount());

                // Text
                sf::Color color = PlayerStats::instance().resourceItems[stack.getItem()->getId()] < stack.getItemCount() ? sf::Color::Red : sf::Color::White;
                renderer.renderText(m_partSelector->getSize().x + 47, HEIGHT * (s + 1) + 20, *m_font, str, 11, color);

                // Icon
                sf::IntRect texRect;
                // TODO: Name that '16' somehow
                texRect.left = stack.getItem()->getAtlasPosition().x * 16;
                texRect.top = stack.getItem()->getAtlasPosition().y * 16;
                texRect.width = 16;
                texRect.height = 16;
                renderer.renderTexturedRectangle(m_partSelector->getSize().x + 10, HEIGHT * (s + 1) + 10, 32, 32, *m_resourceStatsWidget->getAtlas(), texRect);

                s++;
            }

            // Description
            renderer.renderText(m_partSelector->getSize().x + 10, HEIGHT * (s + 1) + 20, *m_font, item->getDescription(), 10, sf::Color(200, 200, 200));
        });

        m_resourceStatsWidget = make<ResourceStatsWidget>(this);
        addWidget(m_resourceStatsWidget);
    }

    virtual void onResize(sf::Event::SizeEvent& event)
    {
        A13GUIAbstractBuilder::onResize(event);

        EGE::Size RESOURCE_STATS_SIZE = 90;
        m_resourceStatsWidget->setPosition({event.width - RESOURCE_STATS_SIZE, 0});
        m_resourceStatsWidget->setSize({RESOURCE_STATS_SIZE, event.height});
    }

private:
    EGE::SharedPtr<ResourceStatsWidget> m_resourceStatsWidget;
};
