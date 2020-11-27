#include "A13FactoryBuilding.h"

#include "Apollo13.h"
#include "A13GUIProjectBuilder.h"
#include "A13GUIFactoryBuilder.h"
#include "GUICraftingSelector.h"
#include "A13GameplayObjectManager.h"
#include "PlayerStats.h"

bool A13FactoryBuildingItem::onPlace(A13::FactoryTilemap* tilemap, int meta, EGE::Vec2i partPos) const
{
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
    int ptime = Apollo13::instance().save.projectTilemap()->getCurrentProjectTime();
    if(ptime >= 0)
    {
        log() << "Cannot open project builder while building rocket!";
        return;
    }
    if(ptime == -3)
    {
        log() << "Cannot open project builder when rocket is launched!";
        return;
    }
    log() << "Open ProjectBuilder!";
    auto gui = Apollo13::instance().getCurrentGUIScreen().get();
    gui->openDialog(make<A13GUIProjectBuilder>(gui));
}

void A13FactoryBuildingRocketFactory::Part::onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i, EGE::TickCount tickCount)
{
    auto ptm = Apollo13::instance().save.projectTilemap();
    int projTime = ptm->getCurrentProjectTime();

    if(projTime == -1)
    {
        auto missing = ptm->getMissingItems();
        if(missing.getItemCount() == 0)
        {
            ptm->startWorkingOnProject();
        }
    }
    else if(projTime >= 0 && projTime < ptm->getTotalProjectTime())
    {
        ptm->progress(1);
    }
}

void A13FactoryBuildingStartPlatform::Part::onActivate(A13::FactoryTilemap*, EGE::Vec2i)
{
    auto ptm = Apollo13::instance().save.projectTilemap();
    if(ptm->finished())
    {
        log(LogLevel::Crash) << "Finally launch rocket!";
        ptm->launchRocket();
        ptm->m_rocketTick = 0;
    }
}

void A13FactoryBuildingStartPlatform::Part::onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i, EGE::TickCount tickCount)
{
    auto ptm = Apollo13::instance().save.projectTilemap();
    if(ptm->getCurrentProjectTime() == -3)
    {
        const double GRAVITY = 160000000000;

        // Rocket physics (simple)
        ptm->m_rocketTick++;
        double dst = ptm->m_rocketHeight + 1000000;
        ptm->m_rocketSpeed += (ptm->m_thrust / 60.0 - GRAVITY * (ptm->m_mass + ptm->m_rocketFuel)
                               / (dst * dst)) / (ptm->m_mass + ptm->m_rocketFuel);
        ptm->m_rocketHeight += ptm->m_rocketSpeed / 60.0;

        if(ptm->m_rocketHeight <= 0)
        {
            ptm->m_rocketHeight = 0;
            ptm->m_rocketSpeed = 0;
        }

        if(ptm->m_rocketHeight > ptm->m_rocketMaxHeight)
            ptm->m_rocketMaxHeight = ptm->m_rocketHeight;

        if(ptm->m_rocketFuel > 0)
            ptm->m_rocketFuel -= ptm->m_fuelUsage * (ptm->m_thrust / ptm->m_maxThrust) / 60.0;
        else
        {
            ptm->m_rocketFuel = 0;
            ptm->m_thrust = 0;
        }

        // Win game
        if(ptm->m_rocketHeight == 0)
        {
            if(ptm->m_rocketMaxHeight > 100000)
            {
                // TODO: calculate point count, height, etc.
                log(LogLevel::Crash) << "!!!!!!! Win game !!!!!!!";
                ptm->m_rocketTick = -1;
                ptm->winGame();
                //Apollo13::instance().winGame();
            }
            else if(ptm->m_rocketMaxHeight > 0)
            {
                log(LogLevel::Crash) << "!!!!!!! Rocket crashed !!!!!!!";
                ptm->m_rocketTick = -1;
                ptm->loseGame();
            }
        }
    }
}

void A13FactoryBuildingStartPlatform::Part::render(A13GUIFactoryBuilder* gui, EGE::Vec2i pos, sf::RenderTarget& target) const
{
    EGE::Renderer renderer(target);

    // Tick count for now
    auto ptm = Apollo13::instance().save.projectTilemap();
    renderer.renderText(pos.x * 16, pos.y * 16, *gui->m_font, "TC: " + std::to_string(ptm->m_rocketTick), 10, sf::Color::White, 100);
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

void A13FactoryBuildingFactory::Part::render(A13GUIFactoryBuilder* gui, EGE::Vec2i pos, sf::RenderTarget& target) const
{
    gui->renderItem({pos.x + getSize().x / 2.0, pos.y + getSize().y / 2.0}, m_recipe ? m_recipe->output.getItem() : nullptr, target);
}


void A13FactoryBuildingFactory::Part::onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i partPos, EGE::TickCount tickCount)
{
    const int CRAFTING_TIME = ((A13FactoryBuildingFactory*)building)->getCraftingDuration();
    const int PREPARATION_TIME = 5;

    if(tickCount - lastCrafting > CRAFTING_TIME + PREPARATION_TIME + 1)
        lastCrafting = tickCount;

    if(m_recipe)
    {
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

EGE::SharedPtr<EGE::ObjectMap> A13FactoryBuildingFactory::Part::serialize()
{
    auto obj = A13::FactoryBuildingPart::serialize();
    if(m_recipe)
        obj->addString("recipe", m_recipe->output.getItem()->getId());
    //obj->addInt("error", m_error);
    return obj;
}

bool A13FactoryBuildingFactory::Part::deserialize(EGE::SharedPtr<EGE::ObjectMap> obj)
{
    if(!A13::FactoryBuildingPart::deserialize(obj))
        return false;

    auto _recipe = obj->getObject("recipe");
    if(_recipe.expired() || !_recipe.lock()->isString())
        m_recipe = nullptr;
    else
        m_recipe = A13GameplayObjectManager::instance().recipes.findById(
                        A13GameplayObjectManager::instance().resourceItems.findById(_recipe.lock()->asString())
                                                                        );

    /*auto _error = obj->getObject("error");
    if(_error.expired() || !_error.lock()->isFloat())
        return false;
    m_error = _error.lock()->asInt();*/

    return true;
}

void A13FactoryBuildingMine::Part::render(A13GUIFactoryBuilder* gui, EGE::Vec2i pos, sf::RenderTarget& target) const
{
    // Inventory
    gui->renderInventory({pos.x + (int)getSize().x - 0.25, pos.y}, fuelContainer.getInventory(), target);

    EGE::Renderer renderer(target);

    // Fuel status
    renderer.renderRectangle(pos.x * 16, pos.y * 16 + 1, getSize().x * 16, 1, sf::Color(127, 0, 0));
    renderer.renderRectangle(pos.x * 16, pos.y * 16 + 1, getSize().x * 16 * (m_fuel / 5.0), 1, sf::Color(0, 127, 0));
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
                // Try remove coal from coal inventory and internal inventory (for coal mines)
                if(!fuelContainer.getInventory().tryRemoveItems({"a13:coal:ore", 1}) && !container->getInventory().tryRemoveItems({"a13:coal:ore", 1}) && !m_requestedCoal)
                {
                    A13::PlayerStats::instance().addResourceItemRequest({"a13:coal:ore", FUEL_BUFFER_SIZE}, &fuelContainer);
                    m_requestedCoal = true;
                    return;
                }
                else
                {
                    m_fuel += FUEL_UNITS_PER_FUEL_ITEM;
                    m_requestedCoal = false;
                }
            }

            // Remove fuel
            m_fuel--;

            // Add an ore to player inventory
            if(!container->getInventory().tryAddItems({id, 1}))
            {
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

            nextRandomTime = EGE::Random::fastRandom().nextIntRanged(240 / std::sqrt(orePos.size()), 360 / std::sqrt(orePos.size())) / multiplier;
        }
    }
}

EGE::SharedPtr<EGE::ObjectMap> A13FactoryBuildingMine::Part::serialize()
{
    auto obj = A13::FactoryBuildingPart::serialize();
    auto _orePos = make<EGE::ObjectList>();
    for(auto& it: orePos)
    {
        _orePos->addObject(EGE::Serializers::fromVector2(it));
    }
    obj->addObject("orePos", _orePos);
    obj->addFloat("multiplier", multiplier);
    obj->addInt("fuel", m_fuel);
    obj->addObject("fuelContainer", fuelContainer.serialize());
    //obj->addInt("requestedCoal", m_requestedCoal);
    return obj;
}

bool A13FactoryBuildingMine::Part::deserialize(EGE::SharedPtr<EGE::ObjectMap> obj)
{
    if(!A13::FactoryBuildingPart::deserialize(obj))
        return false;

    auto _orePos = obj->getObject("orePos");
    if(_orePos.expired() || !_orePos.lock()->isList())
        return false;

    for(auto& it : _orePos.lock()->asList())
    {
        if(!it || !it->isMap())
            return false;
        orePos.push_back(EGE::Serializers::toVector2(std::dynamic_pointer_cast<EGE::ObjectMap>(it)));
    }

    auto _multiplier = obj->getObject("multiplier");
    if(_multiplier.expired() || !_multiplier.lock()->isFloat())
        return false;
    multiplier = _multiplier.lock()->asFloat();

    auto _fuel = obj->getObject("fuel");
    if(_fuel.expired() || !_fuel.lock()->isFloat())
        return false;
    m_fuel = _fuel.lock()->asInt();

    auto _fuelContainer = obj->getObject("fuelContainer");
    if(_fuelContainer.expired() || !_fuelContainer.lock()->isMap())
        return false;
    if(!fuelContainer.deserialize(std::dynamic_pointer_cast<EGE::ObjectMap>(_fuelContainer.lock())))
        return false;

    /*auto _requestedCoal = obj->getObject("requestedCoal");
    if(_requestedCoal.expired() || !_requestedCoal.lock()->isInt())
        return false;
    m_requestedCoal = _requestedCoal.lock()->asInt();*/

    return true;
}

// RECIPES
Cost A13FactoryBuildingRocketFactory::getCost() const
{
    return {
        { A13GameplayObjectManager::items.iron, 600 },
        { A13GameplayObjectManager::items.silicon, 300 },
        { A13GameplayObjectManager::items.aluminum, 200 }
    };
}

Cost A13FactoryBuildingStartPlatform::getCost() const
{
    return {
        { A13GameplayObjectManager::items.silicon, 700 },
        { A13GameplayObjectManager::items.copper, 500 },
        { A13GameplayObjectManager::items.iron, 400 },
        { A13GameplayObjectManager::items.aluminum, 300 },
        { A13GameplayObjectManager::items.titanium, 100 },
        { A13GameplayObjectManager::items.gold, 100 },
        { A13GameplayObjectManager::items.silver, 100 }
    };
}

Cost A13FactoryBuildingFactory::getCost() const
{
    return {
        { A13GameplayObjectManager::items.iron, 100 },
        { A13GameplayObjectManager::items.copper, 50 }
    };
}

Cost A13FactoryBuildingQuickFactory::getCost() const
{
    return {
        { A13GameplayObjectManager::items.iron_ore, 10 },
        { A13GameplayObjectManager::items.copper_ore, 10 }
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
        { A13GameplayObjectManager::items.silicon, 2 }
    };
}
