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
    // Tiles
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingRocketFactory>());
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingStartPlatform>());
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingFactory>());
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingMine>(0));
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingMine>(1));
    registerFactoryBuilding(std::make_unique<A13FactoryBuildingMine>(2));

    // Items
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(0, TERRAIN_FANCY_GRASS)); // grass
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(1, TERRAIN_ASPHALT_ROAD)); // dark asphalt
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(2, TERRAIN_ASPHALT_ROAD_LIGHT)); // light asphalt
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(3, TERRAIN_TRAMPLED_GRASS)); // trampled grass
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRoad>(4, TERRAIN_GRAVEL)); // gravel

    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemRobotSpawner>()); // robot spawner

    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemLogistic>(0)); // logistic planner - any
    registerFactoryBuildingItem(std::make_unique<A13FactoryBuildingItemLogistic>(1)); // logistic planner - belt mode
}

A13GameplayObjectManager::_ResourceItems A13GameplayObjectManager::items;

void A13GameplayObjectManager::registerResourceItems()
{
    // TODO: Finally add EGE::TextureAtlas !!!

    // Ore resources
    EGE::UniquePtr<ResourceItem> item;

    // Coal
    item = std::make_unique<ResourceItem>("a13:coal:ore");
    items.coal_ore = item.get();
    registerResourceItem(std::move(item));

    // Iron
    item = std::make_unique<ResourceItem>("a13:iron:ore");
    items.iron_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:iron");
    items.iron = item.get();
    registerResourceItem(std::move(item));

    // Copper
    item = std::make_unique<ResourceItem>("a13:copper:ore");
    items.copper_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:copper");
    items.copper = item.get();
    registerResourceItem(std::move(item));

    // Titanium
    item = std::make_unique<ResourceItem>("a13:titanium:ore");
    items.titanium_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:titanium");
    items.titanium = item.get();
    registerResourceItem(std::move(item));

    // Sand & Silicon
    item = std::make_unique<ResourceItem>("a13:silicon:sand");
    items.silicon_sand = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:silicon");
    items.silicon = item.get();
    registerResourceItem(std::move(item));

    // Aluminum
    item = std::make_unique<ResourceItem>("a13:aluminum:ore");
    items.aluminum_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:aluminum");
    items.aluminum = item.get();
    registerResourceItem(std::move(item));

    // Diamond
    item = std::make_unique<ResourceItem>("a13:diamond:ore");
    items.diamond_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:diamond");
    items.diamond = item.get();
    registerResourceItem(std::move(item));

    // Gold
    item = std::make_unique<ResourceItem>("a13:gold:ore");
    items.gold_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:gold");
    items.gold = item.get();
    registerResourceItem(std::move(item));

    // Silver
    item = std::make_unique<ResourceItem>("a13:silver:ore");
    items.silver_ore = item.get();
    registerResourceItem(std::move(item));
    item = std::make_unique<ResourceItem>("a13:silver");
    items.silver = item.get();
    registerResourceItem(std::move(item));
}
