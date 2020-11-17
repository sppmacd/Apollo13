#pragma once

#include "A13GUIAbstractBuilder.h"
#include "Builder/BuilderPart.h"
#include "Builder/TilemapObjectState.h"

#include <ege/util.h>

class A13FactoryBuilding;

namespace A13
{

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

class FactoryTilemap;

struct FactoryBuildingPart : public BuilderPart<FactoryTilemap>
{
    const A13FactoryBuilding* building;

    typedef TilemapObjectState<FactoryBuildingPart, 3> StateType;

    FactoryBuildingPart(const A13FactoryBuilding* _building)
    : building(_building) {}

    virtual EGE::Vec2d getAtlasPosition() const;
    virtual EGE::Vec2u getSize() const;

    virtual bool onRemove(A13::FactoryTilemap* tilemap, EGE::Vec2i partPos);
    virtual std::string getTooltip(const FactoryTilemap*, EGE::Vec2i, const StateType&) const;
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
typedef Aliases::A13ChunkedTilemapForPart<A13::FactoryBuildingPart, 16, 16, 3> GUIFactoryBuilder_Tilemap;

class FactoryTilemap : public GUIFactoryBuilder_Tilemap, public A13BuilderTilemap<FactoryBuildingPart, 3>
{
public:
    FactoryTilemap(EGE::MaxInt _seed)
    : seed(_seed) {}

    virtual std::string getTooltip(EGE::Vec2i pos, const StateType& state) const;

    EGE::MaxInt seed;
};

}
