#pragma once

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
        return true;
    }

    void registerRocketParts();

    void registerRocketPart(A13RocketPart* part);

    bool clear()
    {
        rocketParts.clear();
        return true;
    }

    EGE::GameplayObjectRegistry<EGE::String, A13RocketPart> rocketParts;
};
