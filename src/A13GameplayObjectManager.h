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

    bool clear()
    {
        rocketParts.clear();
        factoryBuildings.clear();
        return true;
    }

    EGE::GameplayObjectRegistry<EGE::String, A13RocketPart> rocketParts;
    EGE::GameplayObjectRegistry<EGE::String, A13FactoryBuilding> factoryBuildings;
};
