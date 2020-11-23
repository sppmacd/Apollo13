#include "PlayerStats.h"

namespace A13
{

void PlayerStats::initialize()
{
    getInventory().setMaxItemCount(-1);
    getInventory()[A13GameplayObjectManager::items.coal_ore] = 50;
    getInventory()[A13GameplayObjectManager::items.iron] = 100;
    getInventory()[A13GameplayObjectManager::items.copper] = 100;
}

void PlayerStats::registerContainer(Container* container)
{
    m_registeredContainers.insert(container);
}

void PlayerStats::unregisterContainer(Container* container)
{
    m_registeredContainers.erase(container);
}

void PlayerStats::addResourceItemRequest(ResourceItemStack stack, Container* requester)
{
    m_requests.push({stack, requester});
}

void PlayerStats::update()
{
    // Process half of the requests in the queue.
    // It allows later added processes also to be requested.
    if(m_requests.empty())
        return;

    EGE::Size halfSize = (m_requests.size() + 1) / 2;
    for(EGE::Size s = 0; s < halfSize; s++)
    {
        if(!process(m_requests.front()))
        {
            // Save a copy of request on the back of queue.
            // We will try process it later.
            m_requests.push(m_requests.front());
        }

        m_requests.pop();
    }
}

bool PlayerStats::process(ResourceRequest& request)
{
    if(m_registeredContainers.count(request.requester))
    {
        if(!getInventory().canRemoveItems(request.stack))
        {
            log() << "Failed to process resource request: Not enough resources in player inventory";
            log() << "   for stack " << request.stack;
            return false;
        }
        if(!request.requester->getInventory().canAddItems(request.stack))
        {
            log() << "Failed to process resource request: Not enough space in requester inventory";
            log() << "   for stack " << request.stack;
            return false;
        }

        getInventory().tryRemoveItems(request.stack);
        request.requester->getInventory().tryAddItems(request.stack);
    }
    else
    {
        log() << "Failed to process resource request: The container was removed";
        // We return true to force request be removed as it will always fail.
    }
    return true;
}

}
