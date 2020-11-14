#include "A13FactoryBuilding.h"

EGE::Vec2d A13FactoryBuildingTile::getAtlasPosition() const
{
    return part ? part->getAtlasPosition() : EGE::Vec2d(0, 0);
}

EGE::Vec2u A13FactoryBuildingTile::getSize() const
{
    return part ? part->getSize() : EGE::Vec2u(0, 0);
}

void A13FactoryTilemap::onActivate(EGE::Vec2i pos, const StateType& state)
{
    if(state.obj)
        state.obj->part->onActivate(this, pos - EGE::Vec2i(state.cornerPos), state);
}

std::string A13FactoryTilemap::getTooltip(EGE::Vec2i pos, const A13FactoryTilemap::StateType& state)
{
    // TODO: use localization!
    std::string terrain = "Terrain: " + std::to_string(state.addObjs[0]);
    std::string tile = state.obj ? state.obj->part->getTooltip(this, pos - EGE::Vec2i(state.cornerPos), state) : "No object";
    return terrain + "\n" + tile;
}

void A13FactoryBuildingRocketFactory::onActivate(A13FactoryTilemap*, EGE::Vec2i, const A13FactoryTilemap::StateType&)
{
    log() << "Open ProjectBuilder!";
}
