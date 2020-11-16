#include "A13FactoryBuilding.h"

#include "Apollo13.h"
#include "A13GUIProjectBuilder.h"
#include "A13GameplayObjectManager.h"
#include "PlayerStats.h"

EGE::Vec2d A13FactoryBuildingPart::getAtlasPosition() const
{
    return building ? building->getAtlasPosition() : EGE::Vec2d(0, 0);
}

EGE::Vec2u A13FactoryBuildingPart::getSize() const
{
    return building ? building->getSize() : EGE::Vec2u(0, 0);
}

std::string A13FactoryBuildingPart::getTooltip(const A13FactoryTilemap* tilemap, EGE::Vec2i pos, const A13FactoryBuildingPart::StateType& state) const
{
    return building ? building->getTooltip(tilemap, pos, state) + "\n" : "???\n";
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

bool A13FactoryBuildingItem::onPlace(A13FactoryTilemap* tilemap, EGE::Vec2i partPos)
{
    if(!m_building)
        return true;

    // Check resources in player inventory.
    Cost cost = getCost();

    for(ResourceItemStack& stack: cost)
    {
        if(PlayerStats::instance().resourceItems[stack.getItem()->getId()] < stack.getItemCount())
        {
            // We inform Builder that we placed object,
            // but we didn't do that.
            return true;
        }
    }

    // Remove resources from player inventory.
    for(ResourceItemStack& stack: cost)
    {
        PlayerStats::instance().resourceItems[stack.getItem()->getId()] -= stack.getItemCount();
    }

    return false;
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
                case ORE_IRON: id = "a13:iron"; break;
                case ORE_COPPER: id = "a13:copper"; break;
                case ORE_TITANIUM: id = "a13:titanium"; break;
                case ORE_SILICON: id = "a13:silicon:sand"; break;
                case ORE_ALUMINUM: id = "a13:aluminum"; break;
                case ORE_DIAMOND: id = "a13:diamond"; break;
                case ORE_GOLD: id = "a13:gold"; break;
                case ORE_SILVER: id = "a13:silver"; break;
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

            nextRandomTime = EGE::Random::fastRandom().nextIntRanged(120 / std::sqrt(orePos.size()), 180 / std::sqrt(orePos.size())) / multiplier;
        }
    }
}

// RECIPES
Cost A13FactoryBuildingRocketFactory::getCost() const
{
    return {
        { A13GameplayObjectManager::items.iron, 300 },
        { A13GameplayObjectManager::items.silicon, 150 },
        { A13GameplayObjectManager::items.aluminum, 100 }
    };
}

Cost A13FactoryBuildingStartPlatform::getCost() const
{
    return {
        { A13GameplayObjectManager::items.silicon_sand, 230 },
        { A13GameplayObjectManager::items.iron, 100 },
        { A13GameplayObjectManager::items.titanium, 50 }
    };
}

Cost A13FactoryBuildingMine::getCost() const
{
    switch(m_level)
    {
    case 0:
        return {
            { A13GameplayObjectManager::items.iron, 75 },
            { A13GameplayObjectManager::items.copper, 30 },
        };
    case 1:
        return {
            { A13GameplayObjectManager::items.iron, 100 },
            { A13GameplayObjectManager::items.copper, 75 },
            { A13GameplayObjectManager::items.titanium, 40 }
        };
    case 2:
        return {
            { A13GameplayObjectManager::items.iron, 150 },
            { A13GameplayObjectManager::items.copper, 100 },
            { A13GameplayObjectManager::items.diamond, 50 }
        };
    default:
        return {};
    }
}

Cost A13FactoryBuildingItemRoad::getCost() const
{
    return {
        { A13GameplayObjectManager::items.silicon_sand, 6 }
    };
}
