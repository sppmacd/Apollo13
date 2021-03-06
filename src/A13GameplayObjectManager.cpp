#include "A13GameplayObjectManager.h"

void A13GameplayObjectManager::registerRocketPart(EGE::UniquePtr<A13RocketPart> part)
{
    std::string id = part->getId();
    A13RocketPart* ptr = part.get();
    rocketParts.add(id, std::move(part));
    rocketPartItems.add(id, std::make_unique<A13RocketPartItem>(ptr));
}

void A13GameplayObjectManager::registerFactoryBuilding(EGE::UniquePtr<A13FactoryBuilding> part)
{
    std::string id = part->getId();
    A13FactoryBuilding* ptr = part.get();
    factoryBuildings.add(id, std::move(part));
    factoryBuildingItems.add(id, std::make_unique<A13FactoryBuildingItem>(id, ptr));
}

void A13GameplayObjectManager::registerFactoryBuildingItem(EGE::UniquePtr<A13FactoryBuildingItem> part)
{
    std::string id = part->getId();
    factoryBuildingItems.add(id, std::move(part));
}

void A13GameplayObjectManager::registerResourceItem(EGE::UniquePtr<ResourceItem> item)
{
    std::string id = item->getId();
    ResourceItem* ptr = item.get();
    EGE::IdType nid = (EGE::IdType)resourceItems.add(id, std::move(item));
    ptr->setNumericId(nid);
}

void A13GameplayObjectManager::registerRecipe(EGE::UniquePtr<A13::Recipe> recipe)
{
    auto item = recipe->output.getItem();
    recipes.add(item, std::move(recipe));
}

void A13GameplayObjectManager::registerRocketParts()
{
    registerRocketPart(std::make_unique<A13RocketPartEngine>());
    registerRocketPart(std::make_unique<A13RocketPartFuelTank>(0, 1));
    registerRocketPart(std::make_unique<A13RocketPartFuelTank>(1, 2));
    registerRocketPart(std::make_unique<A13RocketPartFuelTank>(2, 4));
    registerRocketPart(std::make_unique<A13RocketPartCone>());
    registerRocketPart(std::make_unique<A13RocketPartCapsule>());
}

void A13GameplayObjectManager::registerFactoryBuildings()
{
    // Basic buildings
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingQuickFactory>());
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingFactory>());
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingQuickMine>());
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingMine>(0));
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingMine>(1));
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingMine>(2));

    // Roads
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(0, TERRAIN_FANCY_GRASS)); // grass
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(1, TERRAIN_ASPHALT_ROAD)); // dark asphalt
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(2, TERRAIN_ASPHALT_ROAD_LIGHT)); // light asphalt
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(3, TERRAIN_TRAMPLED_GRASS)); // trampled grass
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(4, TERRAIN_GRAVEL)); // gravel

    // Buildings for rocket
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingRocketFactory>());
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingStartPlatform>());

    /*registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRobotSpawner>()); // robot spawner

    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemLogistic>(0)); // logistic planner - any
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemLogistic>(1)); // logistic planner - belt mode*/
}

A13GameplayObjectManager::_ResourceItems A13GameplayObjectManager::items;

void A13GameplayObjectManager::registerResourceItems()
{
    // TODO: Finally add EGE::TextureAtlas !!!

    // Ore resources
    EGE::UniquePtr<ResourceItem> item;

    // Coal
    item = std::make_unique<ResourceItem>("a13:coal:ore", "Coal Ore");
    items.coal_ore = item.get();
    registerResourceItem(std::move(item));

    // Iron
    item = std::make_unique<ResourceItem>("a13:iron:ore", "Iron Ore");
    items.iron_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:iron", "Iron");
    items.iron = item.get();
    registerResourceItem(std::move(item));

    // Copper
    item = std::make_unique<ResourceItem>("a13:copper:ore", "Copper Ore");
    items.copper_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:copper", "Copper");
    items.copper = item.get();
    registerResourceItem(std::move(item));

    // Titanium
    item = std::make_unique<ResourceItem>("a13:titanium:ore", "Titanium Ore");
    items.titanium_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:titanium", "Titanium");
    items.titanium = item.get();
    registerResourceItem(std::move(item));

    // Sand & Silicon
    item = std::make_unique<ResourceItem>("a13:silicon:sand", "Sand");
    items.silicon_sand = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:silicon", "Silicon");
    items.silicon = item.get();
    registerResourceItem(std::move(item));

    // Aluminum
    item = std::make_unique<ResourceItem>("a13:aluminum:ore", "Aluminum Ore");
    items.aluminum_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:aluminum", "Aluminum");
    items.aluminum = item.get();
    registerResourceItem(std::move(item));

    // Diamond
    item = std::make_unique<ResourceItem>("a13:diamond:ore", "Diamond Ore");
    items.diamond_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:diamond", "Diamond");
    items.diamond = item.get();
    registerResourceItem(std::move(item));

    // Gold
    item = std::make_unique<ResourceItem>("a13:gold:ore", "Gold Ore");
    items.gold_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:gold", "Gold");
    items.gold = item.get();
    registerResourceItem(std::move(item));

    // Silver
    item = std::make_unique<ResourceItem>("a13:silver:ore", "Silver Ore");
    items.silver_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:silver", "Silver");
    items.silver = item.get();
    registerResourceItem(std::move(item));
}

void A13GameplayObjectManager::registerRecipes()
{
    // Smelting
    registerRecipe(std::make_unique<A13::Recipe>( A13::Recipe( { {items.coal_ore}, {items.iron_ore, 8}     }, {items.iron, 7}      )));
    registerRecipe(std::make_unique<A13::Recipe>( A13::Recipe( { {items.coal_ore}, {items.copper_ore, 8}   }, {items.copper, 7}    )));
    registerRecipe(std::make_unique<A13::Recipe>( A13::Recipe( { {items.coal_ore}, {items.titanium_ore, 6} }, {items.titanium, 5}  )));
    registerRecipe(std::make_unique<A13::Recipe>( A13::Recipe( { {items.coal_ore}, {items.silicon_sand, 10}}, {items.silicon, 7}   )));
    registerRecipe(std::make_unique<A13::Recipe>( A13::Recipe( { {items.coal_ore}, {items.aluminum_ore, 8} }, {items.aluminum, 7}  )));
    registerRecipe(std::make_unique<A13::Recipe>( A13::Recipe( { {items.coal_ore}, {items.diamond_ore, 4}  }, {items.diamond, 3}   )));
    registerRecipe(std::make_unique<A13::Recipe>( A13::Recipe( { {items.coal_ore}, {items.gold_ore, 4}     }, {items.gold, 3}      )));
    registerRecipe(std::make_unique<A13::Recipe>( A13::Recipe( { {items.coal_ore}, {items.silver_ore, 4}   }, {items.silver, 3}    )));
}
