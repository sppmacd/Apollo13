#pragma once

#include "A13GameplayObjectManager.h"
#include "ResourceItem.h"
#include "ResourceItems/Container.h"

#include <ege/util.h>

namespace A13
{

class PlayerStats
{
public:
    EGE_SINGLETON(PlayerStats);

    void initialize();

    // maps ResourceItem ID to count
    Inventory resourceItems;
};

}
