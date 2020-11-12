#include "A13GameplayObjectManager.h"

void A13GameplayObjectManager::registerRocketPart(A13RocketPart* part)
{
    rocketParts.add(part->getId(), part);
}

void A13GameplayObjectManager::registerFactoryBuilding(A13FactoryBuilding* part)
{
    factoryBuildings.add(part->getId(), part);
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
    registerFactoryBuilding(new A13FactoryBuildingRocketFactory());
    registerFactoryBuilding(new A13FactoryBuildingStartPlatform());
    registerFactoryBuilding(new A13FactoryBuildingRoad(0)); // dirt
    registerFactoryBuilding(new A13FactoryBuildingRoad(1)); // light asphalt
    registerFactoryBuilding(new A13FactoryBuildingRoad(2)); // dark asphalt
    registerFactoryBuilding(new A13FactoryBuildingRoad(3)); // empty gray sth
}
