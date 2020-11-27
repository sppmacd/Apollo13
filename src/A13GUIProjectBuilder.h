#pragma once

#include "A13GUIAbstractBuilder.h"
#include "A13RocketPart.h"

#include "ResourceStatsWidget.h"
#include "ResourceItems/Inventory.h"

#include <iomanip>
#include <sstream>

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
        recalculate();
    }

    virtual void onRemove(EGE::Vec2i tilePos, A13RocketPartPart* part) override
    {
        if(part)
        {
            m_tilemap->getTotalCostInv().tryRemoveItems(part->part->getCost());
            recalculate();
        }
    }

    virtual void onResize(sf::Event::SizeEvent& event)
    {
        A13GUIAbstractBuilder::onResize(event);

        EGE::Size RESOURCE_STATS_SIZE = 90;
        m_resourceStatsWidgetProject->setPosition({event.width - RESOURCE_STATS_SIZE * 2, 0});
        m_resourceStatsWidgetProject->setSize({RESOURCE_STATS_SIZE, event.height});

        m_resourceStatsWidget->setPosition({event.width - RESOURCE_STATS_SIZE, 0});
        m_resourceStatsWidget->setSize({RESOURCE_STATS_SIZE, event.height});

        m_infoLabel->setPosition({event.width / 2, event.height - 40});
    }

    virtual void onLoad() override;

    virtual void onUnload()
    {
        m_tilemap->onCloseProjectBuilder();
    }

    bool checkValidity();

    void recalculate()
    {
        double time = 0;
        double thrust = 0;
        double fuel = 0;
        double fuelUsage = 0;
        double mass = 0;

        for(auto& it: m_tilemap->getParts())
        {
            time += it.second->part->getBuildTime();
            thrust += it.second->part->getThrust();
            fuelUsage += it.second->part->getFuelUsage();
            fuel += it.second->part->getFuelStorage();
            mass += it.second->part->getMass();
        }

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "Fuel: " << fuel << " kg";
        oss << " Fuel Usage: " << fuelUsage << " kg/t\n";
        oss << "Thrust: " << thrust << " N";
        oss << " Build time: " << time / 60.0 << " s)\n";
        oss << "Total mass: " << mass + fuel << " kg (" << mass << " kg without fuel)\n";
        oss << "Expected acceleration (for g = 9.81 m/s^2): " << thrust / (mass + fuel) - 9.81;

        m_infoLabel->setString(oss.str());
    }

    EGE::SharedPtr<ResourceStatsWidget> m_resourceStatsWidget;
    EGE::SharedPtr<ResourceStatsWidget> m_resourceStatsWidgetProject;
    EGE::SharedPtr<EGE::Label> m_infoLabel;
};
