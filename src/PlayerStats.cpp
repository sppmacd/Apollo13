#include "PlayerStats.h"

namespace A13
{

void PlayerStats::initialize()
{
    resourceItems.setMaxItemCount(-1);
    resourceItems[A13GameplayObjectManager::items.coal_ore->getId()] = 50;
    resourceItems[A13GameplayObjectManager::items.iron->getId()] = 100;
    resourceItems[A13GameplayObjectManager::items.copper->getId()] = 100;
}

}
