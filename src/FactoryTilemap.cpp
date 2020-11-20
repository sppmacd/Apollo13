#include "FactoryTilemap.h"

#include "A13FactoryBuilding.h"
#include "PlayerStats.h"

namespace A13
{

EGE::Vec2d FactoryBuildingPart::getAtlasPosition(int) const
{
    return building ? building->getAtlasPosition() : EGE::Vec2d(0, 0);
}

EGE::Vec2u FactoryBuildingPart::getSize() const
{
    return building ? building->getSize() : EGE::Vec2u(0, 0);
}

std::string FactoryBuildingPart::getTooltip(const FactoryTilemap* tilemap, EGE::Vec2i pos, const FactoryBuildingPart::StateType& state) const
{
    return building ? building->getTooltip(tilemap, pos, state) + "\n" : "???\n";
}

bool FactoryBuildingPart::onRemove(FactoryTilemap*, EGE::Vec2i)
{
    if(building)
    {
        // Give back some of the resources to player.
        const double RESOURCE_GIVEBACK_FACTOR = 0.8;

        Cost cost = building->getCost();
        for(ResourceItemStack& stack: cost)
        {
            PlayerStats::instance().resourceItems[stack.getItem()->getId()] += stack.getItemCount() * RESOURCE_GIVEBACK_FACTOR;
        }
    }
    return true;
}

std::string FactoryTilemap::getTooltip(EGE::Vec2i pos, const FactoryTilemap::StateType& state) const
{
    // TODO: use localization!
    std::string terrain = "Terrain: " + std::to_string(state.addObjs[FACTORY_BUILDER_LAYER_TERRAIN]) + "\n";
    Ore* _ore = (Ore*)&state.addObjs[FACTORY_BUILDER_LAYER_ORES];
    std::string ore = _ore->id ? ("Ore: " + std::to_string(_ore->count) + "x #" + std::to_string(_ore->id) + "\n") : "";
    EGE::Uint8 _logistic = state.addObjs[FACTORY_BUILDER_LAYER_LOGISTIC];
    std::string logistic = _logistic ? ("Logistic: " + std::to_string(_logistic) + "\n") : "";
    std::string tile = (state.part ? state.part->getTooltip(this, pos - EGE::Vec2i(state.cornerPos), state) : "No object") + "\n";
    return terrain + ore + tile + logistic;
}

bool FactoryTilemap::onRemove(EGE::Vec2i partPos)
{
    auto& tile = ensureTile(partPos);
    if(tile.addObjs[FACTORY_BUILDER_LAYER_LOGISTIC])
    {
        tile.addObjs[FACTORY_BUILDER_LAYER_LOGISTIC] = 0;
        return true;
    }
    return false;
}

}
