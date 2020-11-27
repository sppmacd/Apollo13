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

    bool finished()
    {
        return m_currentProjectTime == m_totalProjectTime;
    }

private:
    A13::Container m_items;
    A13::Inventory m_totalCostInv; // Total cost of project (rocket)
    int m_totalProjectTime = 1;
    int m_currentProjectTime = -2; // -2 - NOT STARTED, -1 - RESOURCES REQUESTED, >=0 - IN PROGRESS, TOTAL - FINISHED
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
};
