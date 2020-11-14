#include "A13FactoryBuilding.h"

#include "Apollo13.h"
#include "A13GUIProjectBuilder.h"

EGE::Vec2d A13FactoryBuildingPart::getAtlasPosition() const
{
    return part ? part->getAtlasPosition() : EGE::Vec2d(0, 0);
}

EGE::Vec2u A13FactoryBuildingPart::getSize() const
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

bool A13FactoryTilemap::onPlace(EGE::Vec2i pos, EGE::SharedPtr<A13FactoryBuildingPart> part)
{
    // TODO: Check resources
    return part->part->onPlace(this, pos);
}

CanPlaceHere A13FactoryTilemap::canPlaceHere(EGE::Vec2i pos, const A13FactoryTilemap::StateType& state)
{
    if(state.obj)
        return state.obj->part->canPlaceHere(pos, state);
    return CanPlaceHere::Yes;
}

void A13FactoryBuildingRocketFactory::onActivate(A13FactoryTilemap* tmap, EGE::Vec2i, const A13FactoryTilemap::StateType&)
{
    log() << "Open ProjectBuilder!";
    auto gui = Apollo13::instance().getCurrentGUIScreen().get();
    gui->openDialog(make<A13GUIProjectBuilder>(gui));
}
