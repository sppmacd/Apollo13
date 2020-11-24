#include "A13FactoryBuilding.h"

#include "Apollo13.h"
#include "A13GUIProjectBuilder.h"
#include "GUICraftingSelector.h"
#include "A13GameplayObjectManager.h"
#include "PlayerStats.h"

bool A13FactoryBuildingItem::onPlace(A13::FactoryTilemap* tilemap, int meta, EGE::Vec2i partPos) const
{
    if(!m_building)
        return true;

    // Check resources in player inventory.
    Cost cost = getCost();

    for(ResourceItemStack& stack: cost)
    {
        if(!A13::PlayerStats::instance().getInventory().canRemoveItems(stack))
        {
            // We inform Builder that we placed object,
            // but we didn't do that.
            return true;
        }
    }

    // Remove resources from player inventory.
    for(ResourceItemStack& stack: cost)
    {
        A13::PlayerStats::instance().getInventory().tryRemoveItems(stack);
    }

    return false;
}

void A13FactoryBuildingRocketFactory::Part::onActivate(A13::FactoryTilemap* tmap, EGE::Vec2i)
{
    log() << "Open ProjectBuilder!";
    auto gui = Apollo13::instance().getCurrentGUIScreen().get();
    gui->openDialog(make<A13GUIProjectBuilder>(gui));
}

std::string A13FactoryBuildingFactory::Part::getTooltip(const A13::FactoryTilemap* tilemap, EGE::Vec2i pos, const A13::FactoryTilemap::StateType& state) const
{
    return A13::FactoryBuildingPart::getTooltip(tilemap, pos, state)
        + std::to_string(container->getInventory().getItemCount()) + " items in internal storage\n"
        + "Recipe: " + (m_recipe ? ("output: " + m_recipe->output.getItem()->getId()) : "none") + "\n";
}

void A13FactoryBuildingFactory::Part::onActivate(A13::FactoryTilemap*, EGE::Vec2i)
{
    log() << "Open Crafting Selector!";
    auto gui = Apollo13::instance().getCurrentGUIScreen().get();
    gui->openDialog(make<A13::GUICraftingSelector>(gui, this));
}

void A13FactoryBuildingFactory::Part::onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i partPos, EGE::TickCount tickCount)
{
    const int CRAFTING_TIME = 60;
    const int PREPARATION_TIME = 5;

    if(tickCount - lastCrafting > CRAFTING_TIME + PREPARATION_TIME + 1)
        lastCrafting = tickCount;

    if(m_recipe)
    {
        log(LogLevel::Verbose) << tickCount - lastCrafting;
        if(tickCount - lastCrafting == CRAFTING_TIME + PREPARATION_TIME)
        {
            // It's the tick after items were loaded. Try craft.
            m_error = false;

            // Check if every item from input can be removed (exists) from internal storage.
            for(auto item: m_recipe->input)
            {
                if(!container->getInventory().canRemoveItems(item))
                {
                    m_error = true;
                }
            }

            if(!A13::PlayerStats::instance().getInventory().canAddItems(m_recipe->output))
            {
                m_error = true;
            }

            if(m_error)
            {
                lastCrafting = tickCount - CRAFTING_TIME;
                return;
            }

            // We are sure that we have the items loaded. Let's remove them.
            for(auto item: m_recipe->input)
            {
                if(!container->getInventory().tryRemoveItems(item))
                {
                    CRASH();
                }
            }

            // We have successfully removed items. Add the result to player inventory.
            if(!A13::PlayerStats::instance().getInventory().tryAddItems(m_recipe->output))
            {
                CRASH();
            }

            lastCrafting = tickCount;
        }
        else if(tickCount - lastCrafting == CRAFTING_TIME)
        {
            // Add request for each item in input.
            // TODO: requests for multiple items at once
            if(!m_error)
            {
                for(auto item: m_recipe->input)
                    A13::PlayerStats::instance().addResourceItemRequest(item, container.get());
            }
        }
    }
}

void A13FactoryBuildingMine::Part::onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i partPos, EGE::TickCount tickCount)
{
    if(!orePos.empty() && tickCount >= lastOre + nextRandomTime)
    {
        // TODO: Optimize it a bit
        EGE::Size index = EGE::Random::fastRandom().nextInt(orePos.size());
        auto& tile = tilemap->ensureTile(orePos[index]);
        A13::Ore* ore = (A13::Ore*)&tile.addObjs[FACTORY_BUILDER_LAYER_ORES];
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

            // Remove a coal from player inventory (every 5 items)
            // TODO: add other fuels (oil) and depend FUEL_UNITS_PER
            const int FUEL_UNITS_PER_FUEL_ITEM = 5;
            const int FUEL_BUFFER_SIZE = 5;
            if(m_fuel == 0)
            {
                if(!fuelContainer.getInventory().tryRemoveItems({"a13:coal:ore", 1}))
                {
                    A13::PlayerStats::instance().addResourceItemRequest({"a13:coal:ore", FUEL_BUFFER_SIZE}, &fuelContainer);
                    return;
                }
                else
                {
                    m_fuel += FUEL_UNITS_PER_FUEL_ITEM;
                }
            }

            // Remove fuel
            m_fuel--;

            // Add an ore to player inventory
            if(!container->getInventory().tryAddItems({id, 1}))
            {
                log() << "container full";
                A13::PlayerStats::instance().loadItemsFrom(container.get());
                nextRandomTime = EGE::Random::fastRandom().nextIntRanged(240 / std::sqrt(orePos.size()), 360 / std::sqrt(orePos.size())) / multiplier;
                return;
            }

            // Mine an ore
            lastOre = tickCount;
            ore->count--;
            if(ore->count == 0)
            {
                ore->id = ORE_NONE;
                orePos.erase(orePos.begin() + index);
                return;
            }
            log() << "mine";

            nextRandomTime = EGE::Random::fastRandom().nextIntRanged(240 / std::sqrt(orePos.size()), 360 / std::sqrt(orePos.size())) / multiplier;
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

Cost A13FactoryBuildingFactory::getCost() const
{
    return {
        { A13GameplayObjectManager::items.copper, 50 },
        { A13GameplayObjectManager::items.iron, 100 }
    };
}

Cost A13FactoryBuildingMine::getCost() const
{
    switch(m_level)
    {
    case 0:
        return {
            { A13GameplayObjectManager::items.iron, 50 },
            { A13GameplayObjectManager::items.copper, 60 }
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

Cost A13FactoryBuildingQuickMine::getCost() const
{
    return {
        { A13GameplayObjectManager::items.iron, 10 }
    };
}

Cost A13FactoryBuildingItemRoad::getCost() const
{
    return {
        { A13GameplayObjectManager::items.silicon_sand, 6 }
    };
}
