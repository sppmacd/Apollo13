#pragma once

#include "A13GUIAbstractBuilder.h"
#include "Builder/BuilderItem.h"
#include "Builder/BuilderPart.h"

#include <ege/gpo/GameplayObject.h>
#include <ege/util/Vector.h>

class A13RocketPart;

struct A13RocketPartPart : public BuilderPart
{
    A13RocketPart* part;

    A13RocketPartPart(A13RocketPart* _part)
    : part(_part) {}

    virtual EGE::Vec2d getAtlasPosition() const;
    virtual EGE::Vec2u getSize() const;
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
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {}
};

typedef Aliases::A13FixedTilemapForPart<A13RocketPartPart, 32, 64> A13GUIProjectBuilder_Tilemap;

class A13ProjectTilemap : public A13GUIProjectBuilder_Tilemap, public A13BuilderTilemap<A13RocketPartPart, 0>
{
public:
    void onActivate(EGE::Vec2i, const StateType&) {}
};

class A13RocketPartItem : public EGE::GameplayObject, public BuilderItem<A13ProjectTilemap>
{
public:
    A13RocketPartItem(A13RocketPart* createdTile)
    : EGE::GameplayObject(createdTile->getId()), m_tile(createdTile) {}

    virtual EGE::Vec2d getAtlasPosition() const
    {
        return m_tile->getItemAtlasPosition();
    }

    virtual EGE::SharedPtr<A13RocketPartPart> getPart() const;

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i, const A13ProjectTilemap::StateType& tile)
    {
        return !tile.obj ? CanPlaceHere::Yes : CanPlaceHere::No;
    }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {}

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
};

class A13RocketPartFuelTank : public A13RocketPart
{
public:
    A13RocketPartFuelTank(EGE::Size index, EGE::Size size)
    : A13RocketPart("a13:fuel_tank:generic:" + std::to_string(size)), m_index(index), m_size(size) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {2, 1}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {m_index, 2}; }
    virtual EGE::Vec2u getSize() const { return {2, m_size}; }

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
};
