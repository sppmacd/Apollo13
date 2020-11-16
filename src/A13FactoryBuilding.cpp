#include "A13FactoryBuilding.h"

#include "Apollo13.h"
#include "A13GUIProjectBuilder.h"
#include "PlayerStats.h"

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

void A13FactoryBuildingMine::Part::onUpdate(A13FactoryTilemap* tilemap, EGE::Vec2i partPos, EGE::TickCount tickCount)
{
    if(!orePos.empty() && tickCount >= lastOre + nextRandomTime)
    {
        // TODO: Optimize it a bit
        EGE::Size index = EGE::Random::fastRandom().nextInt(orePos.size());
        auto& tile = tilemap->ensureTile(orePos[index]);
        Ore* ore = (Ore*)&tile.addObjs[FACTORY_BUILDER_LAYER_ORES];
        if(ore->id != ORE_NONE)
        {
            // TODO: Don't hardcode this!
            std::string id;
            switch(ore->id)
            {
                case ORE_COAL: id = "a13:coal:ore"; break;
                case ORE_IRON: id = "a13:iron:ore"; break;
                case ORE_COPPER: id = "a13:copper:ore"; break;
                case ORE_TITANIUM: id = "a13:titanium:ore"; break;
                case ORE_SILICON: id = "a13:silicon:sand"; break;
                case ORE_ALUMINUM: id = "a13:aluminum:ore"; break;
                case ORE_DIAMOND: id = "a13:diamond:ore"; break;
                case ORE_GOLD: id = "a13:gold:ore"; break;
                case ORE_SILVER: id = "a13:silver:ore"; break;
                default: break;
            }
            PlayerStats::instance().resourceItems[id]++;

            lastOre = tickCount;
            ore->count--;
            if(ore->count == 0)
            {
                ore->id = ORE_NONE;
                orePos.erase(orePos.begin() + index);
                return;
            }

            nextRandomTime = EGE::Random::fastRandom().nextIntRanged(60 / orePos.size(), 90 / orePos.size()) / multiplier;
        }
    }
}
