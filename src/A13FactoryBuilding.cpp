#include "A13FactoryBuilding.h"

EGE::Vec2d A13FactoryBuildingTile::getAtlasPosition() const
{
    return part ? part->getAtlasPosition() : EGE::Vec2d(0, 0);
}

EGE::Vec2u A13FactoryBuildingTile::getSize() const
{
    return part ? part->getSize() : EGE::Vec2u(0, 0);
}

void A13FactoryTilemap::onActivate(EGE::Vec2i pos, const StateType& tile)
{
    if(tile.obj)
        tile.obj->part->onActivate(this, pos - EGE::Vec2i(tile.cornerPos), tile);
}
