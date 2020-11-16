#pragma once

#include "A13GameplayObjectManager.h"
#include "ResourceItem.h"

#include <ege/util.h>

class PlayerStats
{
public:
    EGE_SINGLETON(PlayerStats);

    void initialize();

    EGE::IdMap<int> resourceItems;
};
