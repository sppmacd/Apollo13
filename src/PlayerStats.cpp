#include "PlayerStats.h"

void PlayerStats::initialize()
{
    for(auto& pr: A13GameplayObjectManager::instance().resourceItems)
    {
        resourceItems[pr.first.baseId] = 0;
    }
}