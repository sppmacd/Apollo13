#pragma once

#include "A13GUIAbstractBuilder.h"
#include "Builder/BuilderItem.h"
#include "Builder/BuilderPart.h"
#include "ResourceItem.h"

#include <ege/gpo/GameplayObject.h>
#include <ege/util/Vector.h>

#define FACTORY_BUILDER_LAYER_TERRAIN 0
#define FACTORY_BUILDER_LAYER_ORES 1
#define FACTORY_BUILDER_LAYER_SHADOWS 2

#define TERRAIN_GRASS 0
#define TERRAIN_ASPHALT_ROAD 1
#define TERRAIN_ASPHALT_ROAD_LIGHT 2
#define TERRAIN_TRAMPLED_GRASS 3
#define TERRAIN_GRAVEL 4
#define TERRAIN_WILD_GRASS 5
#define TERRAIN_FANCY_GRASS 6

#define ORE_NONE 0
#define ORE_COAL 1
#define ORE_IRON 2
#define ORE_COPPER 3
#define ORE_TITANIUM 4
#define ORE_SILICON 5
#define ORE_ALUMINUM 6
#define ORE_DIAMOND 7
#define ORE_GOLD 8
#define ORE_SILVER 9

#define MAX_ORE_COUNT 4096

class A13FactoryBuilding;
class A13FactoryTilemap;

struct A13FactoryBuildingPart : public BuilderPart<A13FactoryTilemap>
{
    const A13FactoryBuilding* building;

    typedef TilemapObjectState<A13FactoryBuildingPart, 3> StateType;

    A13FactoryBuildingPart(const A13FactoryBuilding* _building)
    : building(_building) {}

    virtual EGE::Vec2d getAtlasPosition() const;
    virtual EGE::Vec2u getSize() const;

    virtual std::string getTooltip(const A13FactoryTilemap*, EGE::Vec2i, const StateType&) const;
};

struct Terrain
{
    EGE::Uint32 terrain;
    EGE::Uint32 cornerMask; // NOT USED
};

struct Ore
{
    EGE::Uint32 id = ORE_NONE;
    EGE::Uint32 count = 0;
};

// Layers
// Multi-size object layer
//   - Buildings
// Additional layers
//   - 0 - Terrain (Terrain structure)
//   - 1 - Ores (Ore structure)
//   - 2 - Shadows (bitmap)
typedef Aliases::A13ChunkedTilemapForPart<A13FactoryBuildingPart, 16, 16, 3> A13GUIFactoryBuilder_Tilemap;

class A13FactoryTilemap : public A13GUIFactoryBuilder_Tilemap, public A13BuilderTilemap<A13FactoryBuildingPart, 3>
{
public:
    A13FactoryTilemap(EGE::MaxInt _seed)
    : seed(_seed) {}

    virtual std::string getTooltip(EGE::Vec2i pos, const StateType& state) const;

    EGE::MaxInt seed;
};

typedef EGE::Vector<ResourceItemStack> Cost;

class A13FactoryBuilding : public EGE::GameplayObject
{
public:
    A13FactoryBuilding(EGE::String id)
    : EGE::GameplayObject(id) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 0}; }
    virtual EGE::Vec2u getSize() const { return {1, 1}; }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {}

    virtual EGE::SharedPtr<A13FactoryBuildingPart> makePart() const
    {
        return make<A13FactoryBuildingPart>(this);
    }

    virtual std::string getTooltip(const A13FactoryTilemap*, EGE::Vec2i, const A13FactoryTilemap::StateType&) const
    {
        return getId() + "\n";
    }

    virtual std::string getDescription()
    {
        return "(No description)";
    }

    virtual Cost getCost() const
    {
        return {};
    }

    // Returns if object can be placed here.
    virtual CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13FactoryTilemap::TileType& tile) const
    {
        return !tile.part ? CanPlaceHere::Yes : CanPlaceHere::No;
    }
};

#define A13_CUSTOM_FACTORY_PART(clazz) \
    virtual EGE::SharedPtr<A13FactoryBuildingPart> makePart() const \
    { \
        return make<clazz>(this); \
    } \

class A13FactoryBuildingItem : public EGE::GameplayObject, public BuilderItem<A13GUIFactoryBuilder_Tilemap>
{
public:
    A13FactoryBuildingItem(std::string id, A13FactoryBuilding* building = nullptr)
    : EGE::GameplayObject(id), m_building(building) {}

    virtual EGE::Vec2d getAtlasPosition() const
    {
        return m_building ? m_building->getItemAtlasPosition() : EGE::Vec2d(0, 0);
    }

    virtual EGE::SharedPtr<A13FactoryBuildingPart> getPart() const
    {
        return m_building ? m_building->makePart() : nullptr;
    }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13FactoryTilemap::TileType& tile) const
    {
        return m_building ? m_building->canPlaceHere(tileRel, tile) : CanPlaceHere::NotLoaded;
    }

    virtual Cost getCost() const
    {
        return m_building ? m_building->getCost() : Cost();
    }

    virtual std::string getDescription()
    {
        return m_building ? m_building->getDescription() : "(No building assigned)";
    }

    virtual bool onPlace(A13FactoryTilemap* tilemap, EGE::Vec2i partPos);

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; };
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {};

private:
    A13FactoryBuilding* m_building;
};

class A13FactoryBuildingRocketFactory : public A13FactoryBuilding
{
public:
    A13FactoryBuildingRocketFactory()
    : A13FactoryBuilding("a13:rocket_factory") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {1, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 1}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }

    struct Part : public A13FactoryBuildingPart
    {
        Part(const A13FactoryBuilding* bld)
        : A13FactoryBuildingPart(bld) {}

        virtual void onActivate(A13FactoryTilemap*, EGE::Vec2i);

        virtual std::string getTooltip(const A13FactoryTilemap* tilemap, EGE::Vec2i pos, const A13FactoryTilemap::StateType& state) const
        {
            return A13FactoryBuildingPart::getTooltip(tilemap, pos, state) + "\nClick to open Project Builder";
        }
    };

    virtual std::string getDescription()
    {
        return
        "Rocket Factory is needed to\n"
        "create rocket project. Launching\n"
        "a Rocket gives you many coins,\n"
        "which you can use to buy resources.";
    }

    virtual Cost getCost() const;

    A13_CUSTOM_FACTORY_PART(Part);
};

class A13FactoryBuildingStartPlatform : public A13FactoryBuilding
{
public:
    A13FactoryBuildingStartPlatform()
    : A13FactoryBuilding("a13:start_platform") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {5, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 2}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }

    virtual Cost getCost() const;

    virtual std::string getDescription()
    {
        return
        "Use Start Platform to launch your\n"
        "rockets!";
    }
};

class A13FactoryBuildingFactory : public A13FactoryBuilding
{
public:
    A13FactoryBuildingFactory()
    : A13FactoryBuilding("a13:factory") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {1, 4}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {1, 1}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }

    virtual Cost getCost() const;

    virtual std::string getDescription()
    {
        return
        "Factory is a building used to\n"
        "create new items!";
    }
};

class A13FactoryBuildingMine : public A13FactoryBuilding
{
public:
    A13FactoryBuildingMine(EGE::Size level)
    : A13FactoryBuilding("a13:mine:" + std::to_string(level)), m_level(level) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {9 + m_level * 4, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {m_level, 4}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }

    virtual std::string getTooltip(const A13FactoryTilemap*, EGE::Vec2i pos, const A13FactoryTilemap::StateType& state) const
        { return getId() + "\nLevel " + std::to_string(m_level); }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i pos, const A13FactoryTilemap::StateType& state) const
    {
        Ore* ore = (Ore*)&state.addObjs[FACTORY_BUILDER_LAYER_ORES];
        CanPlaceHere cph = A13FactoryBuilding::canPlaceHere(pos, state);
        if(ore->id != ORE_NONE)
            return cph == CanPlaceHere::Yes ? CanPlaceHere::Match : CanPlaceHere::No;
        return cph == CanPlaceHere::Yes ? CanPlaceHere::Restricted : CanPlaceHere::No;
    }

    virtual double getMultiplier()
    {
        return m_level + 1;
    }

    struct Part : public A13FactoryBuildingPart
    {
        Part(const A13FactoryBuilding* bld)
        : A13FactoryBuildingPart(bld) {}

        virtual std::string getTooltip(A13FactoryTilemap* tilemap, EGE::Vec2i pos, const A13FactoryTilemap::StateType& state) const
        {
            return A13FactoryBuildingPart::getTooltip(tilemap, pos, state)
                + "\nLevel " + std::to_string(((A13FactoryBuildingMine*)building)->m_level);
        }

        virtual bool onPlace(A13FactoryTilemap* tilemap, EGE::Vec2i partPos)
        {
            if(!A13FactoryBuildingPart::onPlace(tilemap, partPos))
                return false;

            for(EGE::Size x = 0; x < getSize().x; x++)
            for(EGE::Size y = 0; y < getSize().y; y++)
            {
                auto& tile = tilemap->ensureTile({x + partPos.x, y + partPos.y});
                Ore* ore = (Ore*)&tile.addObjs[FACTORY_BUILDER_LAYER_ORES];
                if(ore->id != ORE_NONE && ore->count > 0)
                    orePos.push_back({x + partPos.x, y + partPos.y});
            }

            multiplier = ((A13FactoryBuildingMine*)building)->getMultiplier();

            return true;
        }

        virtual void onUpdate(A13FactoryTilemap* tilemap, EGE::Vec2i, EGE::TickCount tickCount);

        EGE::Vector<EGE::Vec2i> orePos;
        int nextRandomTime = 0;
        EGE::TickCount lastOre = 0;
        double multiplier = 1;
    };

    A13_CUSTOM_FACTORY_PART(Part);

    virtual Cost getCost() const;

    virtual std::string getDescription()
    {
        return
        "Mines allows you to get resources.\n"
        "The higher is level, the more\n"
        "resources it mines.";
    }

private:
    EGE::Size m_level;
};

class A13FactoryBuildingItemRoad : public A13FactoryBuildingItem
{
public:
    A13FactoryBuildingItemRoad(EGE::Size index, EGE::Size placedTerrain)
    : A13FactoryBuildingItem("a13:building_road:" + std::to_string(index)), m_index(index), m_placed(placedTerrain) {}

    struct Part : public A13FactoryBuildingPart
    {
        Part()
        : A13FactoryBuildingPart(nullptr) {}

        virtual EGE::Vec2u getSize() const
        {
            return {1, 1};
        }
    };

    virtual EGE::Vec2d getAtlasPosition() const { return {m_index, 3}; }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i pos, const A13FactoryTilemap::StateType& state) const
    {
        if(state.addObjs[FACTORY_BUILDER_LAYER_TERRAIN] == m_placed)
            return CanPlaceHere::Restricted;
        return CanPlaceHere::Yes;
    }

    virtual bool onPlace(A13FactoryTilemap* tilemap, EGE::Vec2i tilePos) const
    {
        tilemap->ensureTile(tilePos).addObjs[FACTORY_BUILDER_LAYER_TERRAIN] = m_placed;
        return true;
    }

    virtual CanPlaceHere canPlaceHere(A13FactoryTilemap* tilemap, EGE::Vec2i pos, const A13FactoryTilemap::StateType& state) const
    {
        return CanPlaceHere::Yes;
    }

    virtual EGE::SharedPtr<A13FactoryBuildingPart> getPart() const
    {
        return make<Part>();
    }

    virtual Cost getCost() const;

    virtual std::string getDescription()
    {
        return
        "Roads are decorative items which\n"
        "you can place on terrain, below\n"
        "regular buildings.";
    }

private:
    EGE::Size m_index;
    EGE::Size m_placed;
};
