#include "A13GameplayObjectManager.h"

void A13GameplayObjectManager::registerRocketPart(A13RocketPart* part)
{
    rocketParts.add(part->getId(), part);
}

void A13GameplayObjectManager::registerRocketParts()
{
    registerRocketPart(new A13RocketPartEngine());
    registerRocketPart(new A13RocketPartFuelTank());
    registerRocketPart(new A13RocketPartCapsule());
}
