#include "PlayerStats.h"

void PlayerStats::initialize()
{
    for(auto& pr: A13GameplayObjectManager::instance().resourceItems)
    {
        resourceItems[pr.first.baseId] = 0;
    }
    resourceItems[A13GameplayObjectManager::items.coal_ore->getId()] = 50;
    resourceItems[A13GameplayObjectManager::items.iron->getId()] = 100;
    resourceItems[A13GameplayObjectManager::items.copper->getId()] = 100;
}
