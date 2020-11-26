#pragma once

#include "Apollo13.h"
#include "A13GameplayObjectManager.h"
#include "A13FactoryBuilding.h"
#include "PlayerStats.h"
#include "ResourceStatsWidget.h"

class A13GUIFactoryBuilder : public A13GUIAbstractBuilder<A13::FactoryTilemap, A13FactoryBuildingItem>
{
public:
    A13GUIFactoryBuilder(EGE::GUIGameLoop* loop);

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
        else if(addLayer == FACTORY_BUILDER_LAYER_LOGISTIC)
        {
            EGE::Uint8 flag = state.addObjs[addLayer];
            return {(flag & 0xF) << 4, (flag & 0xF0)};
        }
        return EGE::Vec2d(0, 0);
    }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13::FactoryTilemap::TileType& tile, A13FactoryBuildingItem* item)
    {
        return item->canPlaceHere(tileRel, tile);
    }

    virtual void onLoad();

    virtual void onResize(sf::Event::SizeEvent& event)
    {
        A13GUIAbstractBuilder::onResize(event);

        EGE::Size RESOURCE_STATS_SIZE = 90;
        m_resourceStatsWidget->setPosition({event.width - RESOURCE_STATS_SIZE, 0});
        m_resourceStatsWidget->setSize({RESOURCE_STATS_SIZE, event.height});
    }

    // Item is centered to tilePos.
    void renderItem(EGE::Vec2d tilePos, ResourceItem* item, sf::RenderTarget& target)
    {
        EGE::Renderer renderer(target);
        sf::Vector2f scenePos(tilePos.x * 16, tilePos.y * 16);

        sf::IntRect texRect;
        // TODO: Name that '16' somehow
        texRect.left = item->getAtlasPosition().x * 16;
        texRect.top = item->getAtlasPosition().y * 16;
        texRect.width = 16;
        texRect.height = 16;
        renderer.renderTexturedRectangle(scenePos.x - 4, scenePos.y - 4, 8, 8, *m_resourceStatsWidget->getAtlas(), texRect);
    }

    void renderInventory(EGE::Vec2d tilePos, const A13::Inventory& inventory, sf::RenderTarget& target)
    {
        if(inventory.getItemCount() == 0)
            return;

        const unsigned HEIGHT = 10;
        EGE::Size s = 0;

        EGE::Renderer renderer(target);
        sf::Vector2f scenePos(tilePos.x * 16, tilePos.y * 16);

        for(auto& stack: inventory)
        {
            if(stack.first.empty() || !stack.second)
                continue;

            std::string str = std::to_string(stack.second);

            // Text
            renderer.renderText(scenePos.x + 3, HEIGHT * s + scenePos.y - 6, *m_font, str, 6, sf::Color::White, 10);

            // Icon
            sf::IntRect texRect;
            // TODO: Name that '16' somehow
            auto item = A13GameplayObjectManager::instance().resourceItems.findById(stack.first);
            texRect.left = item->getAtlasPosition().x * 16;
            texRect.top = item->getAtlasPosition().y * 16;
            texRect.width = 16;
            texRect.height = 16;
            renderer.renderTexturedRectangle(scenePos.x - 4, HEIGHT * s + scenePos.y - 6, 8, 8, *m_resourceStatsWidget->getAtlas(), texRect);

            s++;
        }
    }

private:
    EGE::SharedPtr<ResourceStatsWidget> m_resourceStatsWidget;
};
