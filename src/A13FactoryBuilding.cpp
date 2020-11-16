#include "A13FactoryBuilding.h"

#include "Apollo13.h"
#include "A13GUIProjectBuilder.h"

EGE::Vec2d A13FactoryBuildingPart::getAtlasPosition() const
{
    return building ? building->getAtlasPosition() : EGE::Vec2d(0, 0);
}

EGE::Vec2u A13FactoryBuildingPart::getSize() const
{
    return building ? building->getSize() : EGE::Vec2u(0, 0);
}

std::string A13FactoryBuildingPart::getTooltip(const A13FactoryTilemap*, EGE::Vec2i, const A13FactoryBuildingPart::StateType&) const
{
    return building->getId() + "\n";
}

std::string A13FactoryTilemap::getTooltip(EGE::Vec2i pos, const A13FactoryTilemap::StateType& state) const
{
    // TODO: use localization!
    std::string terrain = "Terrain: " + std::to_string(state.addObjs[FACTORY_BUILDER_LAYER_TERRAIN]) + "\n";
    Ore* _ore = (Ore*)&state.addObjs[FACTORY_BUILDER_LAYER_ORES];
    std::string ore = _ore->id ? ("Ore: " + std::to_string(_ore->count) + "x #" + std::to_string(_ore->id) + "\n") : "";
    std::string tile = state.part ? state.part->getTooltip(this, pos - EGE::Vec2i(state.cornerPos), state) : "No object";
    return terrain + ore + tile;
}

void A13FactoryBuildingRocketFactory::Part::onActivate(A13FactoryTilemap* tmap, EGE::Vec2i)
{
    log() << "Open ProjectBuilder!";
    auto gui = Apollo13::instance().getCurrentGUIScreen().get();
    gui->openDialog(make<A13GUIProjectBuilder>(gui));
}
