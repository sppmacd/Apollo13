#pragma once

#include "A13GUIAbstractBuilder.h"
#include "BuilderItem.h"
#include "BuilderPart.h"

#include <ege/gpo/GameplayObject.h>
#include <ege/util/Vector.h>

#define FACTORY_BUILDER_LAYER_TERRAIN 0
#define FACTORY_BUILDER_LAYER_SHADOWS 1

#define TERRAIN_GRASS 0
#define TERRAIN_ASPHALT_ROAD 1
#define TERRAIN_ASPHALT_ROAD_LIGHT 2
#define TERRAIN_TRAMPLED_GRASS 3
#define TERRAIN_GRAVEL 4
#define TERRAIN_WILD_GRASS 5
#define TERRAIN_FANCY_GRASS 6

class A13FactoryBuilding;

struct A13FactoryBuildingTile : public BuilderPart
{
    A13FactoryBuilding* part;

    A13FactoryBuildingTile(A13FactoryBuilding* _part)
    : part(_part) {}

    virtual EGE::Vec2d getAtlasPosition() const;
    virtual EGE::Vec2u getSize() const;
};

// Layers
// Multi-size object layer
//   - Buildings
// Additional layers
//   - 0 - Terrain (as index in terrain.png) [TODO: make x coordinate working]
//   - 1 - Shadows
typedef Aliases::A13ChunkedTilemapForPart<A13FactoryBuildingTile, 16, 16, 2> A13GUIFactoryBuilder_Tilemap;

class A13FactoryTilemap : public A13GUIFactoryBuilder_Tilemap, public A13BuilderTilemap<A13FactoryBuildingTile, 2>
{
public:
    virtual void onActivate(EGE::Vec2i pos, const StateType& tile);
    virtual std::string getTooltip(EGE::Vec2i pos, const StateType& state);
};

class A13FactoryBuilding : public EGE::GameplayObject, public BuilderPart
{
public:
    A13FactoryBuilding(EGE::String id)
    : EGE::GameplayObject(id) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 0}; }
    virtual EGE::Vec2u getSize() const { return {1, 1}; }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {}

    // Should it be rendered as terrain layer (can place buildings over it?)
    virtual bool isTerrainLayer() { return false; }

    // Called when left-clicking on object.
    // Args: tilemap, partPos, tile
    virtual void onActivate(A13FactoryTilemap*, EGE::Vec2i, const A13FactoryTilemap::StateType&) { log() << "onActivate"; }
    virtual std::string getTooltip(A13FactoryTilemap*, EGE::Vec2i, const A13FactoryTilemap::StateType&) { return getId(); }
};

class A13FactoryBuildingItem : public EGE::GameplayObject, public BuilderItem<A13GUIFactoryBuilder_Tilemap>
{
public:
    A13FactoryBuildingItem(std::string id, A13FactoryBuilding* part = nullptr)
    : EGE::GameplayObject(id), m_part(part) {}

    virtual EGE::Vec2d getAtlasPosition() const
    {
        return m_part ? m_part->getItemAtlasPosition() : EGE::Vec2d(0, 0);
    }

    virtual EGE::SharedPtr<A13FactoryBuildingTile> getPart() const
    {
        return make<A13FactoryBuildingTile>(m_part);
    }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; };
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {};

private:
    A13FactoryBuilding* m_part;
};

class A13FactoryBuildingRocketFactory : public A13FactoryBuilding
{
public:
    A13FactoryBuildingRocketFactory()
    : A13FactoryBuilding("a13:rocket_factory") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {1, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 1}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }

    virtual void onActivate(A13FactoryTilemap*, EGE::Vec2i, const A13FactoryTilemap::StateType&);
    virtual std::string getTooltip(A13FactoryTilemap*, EGE::Vec2i pos, const A13FactoryTilemap::StateType& state)
        { return getId() + "\nClick to open Project Builder"; }
};

class A13FactoryBuildingStartPlatform : public A13FactoryBuilding
{
public:
    A13FactoryBuildingStartPlatform()
    : A13FactoryBuilding("a13:start_platform") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {5, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 2}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }
};

class A13FactoryBuildingItemRoad : public A13FactoryBuildingItem
{
public:
    A13FactoryBuildingItemRoad(EGE::Size index, EGE::Size placedTerrain)
    : A13FactoryBuildingItem("a13:building_road:" + std::to_string(index)), m_index(index), m_placed(placedTerrain) {}

    struct Tile : public A13FactoryBuildingTile
    {
        Tile()
        : A13FactoryBuildingTile(nullptr) {}

        virtual EGE::Vec2u getSize() const
        {
            return {1, 1};
        }
    };

    virtual EGE::Vec2d getAtlasPosition() const { return {m_index, 3}; }

    virtual bool isTerrainLayer() { return true; }

    virtual bool onPlace(A13GUIFactoryBuilder_Tilemap* tilemap, EGE::Vec2i tilePos) const
    {
        tilemap->ensureTile(tilePos).addObjs[FACTORY_BUILDER_LAYER_TERRAIN] = m_placed;
        return true;
    }

    virtual EGE::SharedPtr<A13FactoryBuildingTile> getPart() const
    {
        return make<Tile>();
    }

private:
    EGE::Size m_index;
    EGE::Size m_placed;
};
