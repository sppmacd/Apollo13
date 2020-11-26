#pragma once

#include "A13GUIAbstractBuilder.h"
#include "A13RocketPart.h"

#include "ResourceStatsWidget.h"
#include "ResourceItems/Inventory.h"

// modal dialog
class A13GUIProjectBuilder : public A13GUIAbstractBuilder<A13ProjectTilemap, A13RocketPartItem>
{
public:
    A13GUIProjectBuilder(EGE::GUIScreen* loop);

    void onKeyPress(sf::Event::KeyEvent& event);

    CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13ProjectTilemap::StateType& tile, A13RocketPartItem* item)
    {
        return item->canPlaceHere(tileRel, tile);
    }

    virtual void onPlace(EGE::Vec2i tilePos, A13RocketPartPart& part) override
    {
        m_tilemap->getTotalCostInv().tryAddItems(part.part->getCost());
    }

    virtual void onRemove(EGE::Vec2i tilePos, A13RocketPartPart* part) override
    {
        if(part)
            m_tilemap->getTotalCostInv().tryRemoveItems(part->part->getCost());
    }

    virtual void onResize(sf::Event::SizeEvent& event)
    {
        A13GUIAbstractBuilder::onResize(event);

        EGE::Size RESOURCE_STATS_SIZE = 90;
        m_resourceStatsWidgetProject->setPosition({event.width - RESOURCE_STATS_SIZE * 2, 0});
        m_resourceStatsWidgetProject->setSize({RESOURCE_STATS_SIZE, event.height});

        m_resourceStatsWidget->setPosition({event.width - RESOURCE_STATS_SIZE, 0});
        m_resourceStatsWidget->setSize({RESOURCE_STATS_SIZE, event.height});
    }

    virtual void onLoad() override;

    virtual void onUnload()
    {
        m_tilemap->onCloseProjectBuilder();
    }

private:
    EGE::SharedPtr<ResourceStatsWidget> m_resourceStatsWidget;
    EGE::SharedPtr<ResourceStatsWidget> m_resourceStatsWidgetProject;
};
