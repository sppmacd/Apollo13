#include "A13GameplayObjectManager.h"

void A13GameplayObjectManager::registerRocketPart(A13RocketPart* part)
{
    rocketParts.add(part->getId(), part);
    rocketPartItems.add(part->getId(), new A13RocketPartItem(part));
}

void A13GameplayObjectManager::registerFactoryBuilding(A13FactoryBuilding* part)
{
    factoryBuildings.add(part->getId(), part);
    factoryBuildingItems.add(part->getId(), new A13FactoryBuildingItem(part->getId(), part));
}

void A13GameplayObjectManager::registerFactoryBuildingItem(A13FactoryBuildingItem* part)
{
    factoryBuildingItems.add(part->getId(), part);
}

void A13GameplayObjectManager::registerRocketParts()
{
    registerRocketPart(new A13RocketPartEngine());
    registerRocketPart(new A13RocketPartFuelTank(1));
    registerRocketPart(new A13RocketPartFuelTank(2));
    registerRocketPart(new A13RocketPartFuelTank(4));
    registerRocketPart(new A13RocketPartCone());
    registerRocketPart(new A13RocketPartCapsule());
}

void A13GameplayObjectManager::registerFactoryBuildings()
{
    // Tiles
    registerFactoryBuilding(new A13FactoryBuildingRocketFactory());
    registerFactoryBuilding(new A13FactoryBuildingStartPlatform());

    // Items
    registerFactoryBuildingItem(new A13FactoryBuildingItemRoad(TERRAIN_FANCY_GRASS)); // grass
    registerFactoryBuildingItem(new A13FactoryBuildingItemRoad(TERRAIN_ASPHALT_ROAD)); // dark asphalt
    registerFactoryBuildingItem(new A13FactoryBuildingItemRoad(TERRAIN_ASPHALT_ROAD_LIGHT)); // light asphalt
    registerFactoryBuildingItem(new A13FactoryBuildingItemRoad(TERRAIN_TRAMPLED_GRASS)); // trampled grass
    registerFactoryBuildingItem(new A13FactoryBuildingItemRoad(TERRAIN_GRAVEL)); // trampled grass
}
