#include "A13GameplayObjectManager.h"

void A13GameplayObjectManager::registerRocketPart(A13RocketPart* part)
{
    rocketParts.add(part->getId(), part);
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
