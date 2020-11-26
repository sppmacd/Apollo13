#pragma once

#include "A13GUIAbstractBuilder.h"
#include "Builder/BuilderItem.h"
#include "Builder/BuilderPart.h"
#include "ResourceItem.h"
#include "ResourceItems/Inventory.h"

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
};

typedef Aliases::A13FixedTilemapForPart<A13RocketPartPart, 32, 64> A13GUIProjectBuilder_Tilemap;

class A13ProjectTilemap : public A13GUIProjectBuilder_Tilemap, public A13BuilderTilemap<A13RocketPartPart, 0>
{
public:
    void onActivate(EGE::Vec2i, const StateType&) {}

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize();
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap>);

    void setTotalProjectCost(A13::Inventory& inv)
    {
        for(auto it: inv)
        {
            if(it.second > 0)
                m_cost.push_back({it.first, it.second});
        }
    }

    Cost m_cost;
};

class A13RocketPartItem : public EGE::GameplayObject, public BuilderItem<A13ProjectTilemap>
{
public:
    A13RocketPartItem(A13RocketPart* createdTile)
    : EGE::GameplayObject(createdTile->getId()), m_tile(createdTile) {}

    virtual EGE::Vec2d getAtlasPosition(int) const
    {
        return m_tile->getItemAtlasPosition();
    }

    virtual EGE::SharedPtr<A13RocketPartPart> getPart() const;

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i, const A13ProjectTilemap::StateType& tile)
    {
        return !tile.part ? CanPlaceHere::Yes : CanPlaceHere::No;
    }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap>) { return true; }

    virtual std::string getTooltip() { return m_tile->getId(); }

private:
    A13RocketPart* m_tile;
};

class A13RocketPartEngine : public A13RocketPart
{
public:
    A13RocketPartEngine()
    : A13RocketPart("a13:engine:generic") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 1}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 1}; }
    virtual EGE::Vec2u getSize() const { return {2, 2}; }

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

    virtual Cost getCost() const override;
};
