#pragma once

#include "A13GUIAbstractBuilder.h"
#include "Builder/BuilderItem.h"
#include "Builder/BuilderPart.h"
#include "ResourceItem.h"
#include "ResourceItems/Inventory.h"
#include "ResourceItems/Container.h"

#include <ege/gpo/GameplayObject.h>
#include <ege/util/Vector.h>

class A13RocketPart;
class A13ProjectTilemap;

struct A13RocketPartPart : public BuilderPart<A13ProjectTilemap>
{
    A13RocketPart* part;

    A13RocketPartPart(A13RocketPart* _part)
    : part(_part) {}

    virtual EGE::Vec2d getAtlasPosition(int meta) const;
    virtual EGE::Vec2u getSize() const;

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize();
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap>);
};

class A13RocketPart : public EGE::GameplayObject
{
public:
    A13RocketPart(EGE::String id)
    : EGE::GameplayObject(id) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 0}; }
    virtual EGE::Vec2u getSize() const { return {1, 1}; }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap>) { return true; }

    virtual Cost getCost() const { return {}; }
    virtual int getBuildTime() const { return 60 * 20; } // 20 s

    virtual double getThrust() const { return 0; } // [N]
    virtual double getFuelUsage() const { return 0.0; } // [kg/s]
    virtual double getFuelStorage() const { return 0.0; } // [kg]

    // Rocket mass (without fuel)
    virtual double getMass() const { return 1.0; } // [kg]

    virtual std::string getDescription() { return "(No description provided)"; }
};

typedef Aliases::A13FixedTilemapForPart<A13RocketPartPart, 32, 64> A13GUIProjectBuilder_Tilemap;

class A13ProjectTilemap : public A13GUIProjectBuilder_Tilemap, public A13BuilderTilemap<A13RocketPartPart, 0>
{
public:
    void onActivate(EGE::Vec2i, const StateType&) {}

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize();
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap>);

    A13::Inventory& getTotalCostInv() { return m_totalCostInv; }
    A13::Inventory& getCurrentCostInv() { return m_items.getInventory(); }

    void onCloseProjectBuilder();

    int getTotalProjectTime() { return m_totalProjectTime; }
    int getCurrentProjectTime() { return m_currentProjectTime; }

    void recalculateRocketProperties();

    A13::Inventory getMissingItems()
    {
        A13::Inventory msItems;
        for(auto& stack: m_totalCostInv)
        {
            int diff = stack.second - m_items.getInventory()[stack.first];
            if(diff > 0)
            {
                msItems.tryAddItems({stack.first, diff});
            }
        }
        return msItems;
    }

    void startWorkingOnProject()
    {
        m_currentProjectTime = 0;
        m_items.getInventory().clear();
    }

    void progress(int i)
    {
        if(m_currentProjectTime >= 0 && m_currentProjectTime + i <= m_totalProjectTime)
            m_currentProjectTime += i;
    }

    bool finished() { return m_currentProjectTime == m_totalProjectTime; }
    void launchRocket()
    {
        m_currentProjectTime = -3;
        m_rocketTick = 0;
        m_rocketSpeed = 0;
        m_rocketHeight = 0;
        m_rocketMaxHeight = 0;
        m_rocketFuel = m_fuelTotal;
        m_thrust = m_maxThrust;
    }
    void winGame() { m_currentProjectTime = -2; }
    void loseGame() { m_currentProjectTime = -2; }

    void update();
    double calculatePointsForWin();

    int m_rocketTick = -1;

    // [m/s]
    double m_rocketSpeed = 0;

    // [m]
    double m_rocketHeight = 0;
    double m_rocketMaxHeight = 0;

    // [kg]
    double m_rocketFuel = 0;
    double m_fuelTotal = 0;

    // [kg/s]
    double m_fuelUsage = 0;

    // [N]
    double m_thrust = 1;
    double m_maxThrust = 1;

    // [kg]
    double m_mass = 0;

private:
    A13::Container m_items;
    A13::Inventory m_totalCostInv; // Total cost of project (rocket)
    int m_totalProjectTime = 1;
    int m_currentProjectTime = -2; // -3 - ROCKET LAUNCHED, -2 - NOT STARTED, -1 - RESOURCES REQUESTED, >=0 - IN PROGRESS, TOTAL - FINISHED
};

class A13RocketPartItem : public EGE::GameplayObject, public BuilderItem<A13ProjectTilemap>
{
public:
    A13RocketPartItem(A13RocketPart* createdTile)
    : EGE::GameplayObject(createdTile->getId()), m_rocketPart(createdTile) {}

    virtual EGE::Vec2d getAtlasPosition(int) const
    {
        return m_rocketPart->getItemAtlasPosition();
    }

    virtual EGE::SharedPtr<A13RocketPartPart> getPart() const;

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i, const A13ProjectTilemap::StateType& tile)
    {
        return !tile.part ? CanPlaceHere::Yes : CanPlaceHere::No;
    }

    A13RocketPart* getRocketPart()
    {
        return m_rocketPart;
    }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap>) { return true; }

    virtual std::string getTooltip() { return m_rocketPart->getId(); }

    virtual std::string getDescription() { return m_rocketPart->getDescription(); }

private:
    A13RocketPart* m_rocketPart;
};

class A13RocketPartEngine : public A13RocketPart
{
public:
    A13RocketPartEngine()
    : A13RocketPart("a13:engine:generic") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 1}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 1}; }
    virtual EGE::Vec2u getSize() const { return {2, 2}; }

    virtual int getBuildTime() const { return 60 * 60 * 2; } // 120 s

    virtual double getThrust() const override { return 110000; } // [N]
    virtual double getFuelUsage() const override { return 30.0; } // [kg/s]

    // Rocket mass (without fuel)
    virtual double getMass() const override { return 500.0; } // [kg]

    virtual Cost getCost() const override;
};

class A13RocketPartFuelTank : public A13RocketPart
{
public:
    A13RocketPartFuelTank(EGE::Size index, EGE::Size size)
    : A13RocketPart("a13:fuel_tank:generic:" + std::to_string(size)), m_index(index), m_size(size) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {2, 1}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {m_index, 2}; }
    virtual EGE::Vec2u getSize() const { return {2, m_size}; }

    virtual int getBuildTime() const { return 60 * 20 * m_size; } // 20 * size s

    virtual Cost getCost() const override;

    virtual double getFuelStorage() const override { return 1400.0; } // [kg]

    // Rocket mass (without fuel)
    virtual double getMass() const override { return 250.0 * (m_size - 0.5); } // [kg]

private:
    EGE::Size m_size;
    EGE::Size m_index;
};

class A13RocketPartCone : public A13RocketPart
{
public:
    A13RocketPartCone()
    : A13RocketPart("a13:cone:generic") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 5}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 3}; }
    virtual EGE::Vec2u getSize() const { return {2, 2}; }

    virtual int getBuildTime() const { return 60 * 10; } // 10 s

    virtual Cost getCost() const override;

    // Rocket mass (without fuel)
    virtual double getMass() const override { return 400.0; } // [kg]

    virtual double getThrust() const override { return 5000; } // [N]

private:
    EGE::Size m_size;
};

class A13RocketPartCapsule : public A13RocketPart
{
public:
    A13RocketPartCapsule()
    : A13RocketPart("a13:capsule:generic") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 3}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 4}; }
    virtual EGE::Vec2u getSize() const { return {2, 2}; }

    virtual int getBuildTime() const { return 60 * 60 * 3; } // 180 s

    virtual Cost getCost() const override;

    virtual double getFuelUsage() const override { return 0.05; } // [kg/s]

    // Rocket mass (without fuel)
    virtual double getMass() const override { return 650.0; } // [kg]
};
