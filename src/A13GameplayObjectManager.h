#pragma once

#include "A13FactoryBuilding.h"
#include "A13RocketPart.h"
#include "ResourceItem.h"

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
        registerResourceItems();
        return true;
    }

    void registerRocketParts();
    void registerFactoryBuildings();
    void registerResourceItems();

    void registerRocketPart(EGE::UniquePtr<A13RocketPart> part);
    void registerFactoryBuilding(EGE::UniquePtr<A13FactoryBuilding> part);

    void registerFactoryBuildingItem(EGE::UniquePtr<A13FactoryBuildingItem> part);

    void registerResourceItem(EGE::UniquePtr<ResourceItem> item);

    bool clear()
    {
        rocketParts.clear();
        rocketPartItems.clear();
        factoryBuildings.clear();
        factoryBuildingItems.clear();
        resourceItems.clear();
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

    // Resource Items (stone, coal, wood etc.)
    EGE::GameplayObjectRegistry<EGE::String, ResourceItem> resourceItems;
};
