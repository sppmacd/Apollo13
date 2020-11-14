#pragma once

#include "A13FactoryBuilding.h"
#include "A13RocketPart.h"

#include <ege/gpo/GameplayObjectManager.h>
#include <ege/gpo/GameplayObjectRegistry.h>
#include <ege/util/Types.h>

class A13GameplayObjectManager : public EGE::GameplayObjectManager
{
public:
    EGE_SINGLETON(A13GameplayObjectManager);

    bool load()
    {
        registerRocketParts();
        registerFactoryBuildings();
        return true;
    }

    void registerRocketParts();
    void registerFactoryBuildings();

    void registerRocketPart(A13RocketPart* part);
    void registerFactoryBuilding(A13FactoryBuilding* part);

    void registerFactoryBuildingItem(A13FactoryBuildingItem* part);

    bool clear()
    {
        rocketParts.clear();
        factoryBuildings.clear();
        return true;
    }

    // Rocket parts (that are placed)
    EGE::GameplayObjectRegistry<EGE::String, A13RocketPart> rocketParts;

    // Items for Rocket Parts (that are in left panel)
    EGE::GameplayObjectRegistry<EGE::String, A13RocketPartItem> rocketPartItems;

    // Factory Building (that are placed)
    EGE::GameplayObjectRegistry<EGE::String, A13FactoryBuilding> factoryBuildings;

    // Items for Factory Buildings (that are in left panel)
    EGE::GameplayObjectRegistry<EGE::String, A13FactoryBuildingItem> factoryBuildingItems;
};
