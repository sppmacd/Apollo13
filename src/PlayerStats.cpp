#include "PlayerStats.h"

#include "Apollo13.h"

namespace A13
{

PlayerStats& PlayerStats::instance()
{
    return Apollo13::instance().save.playerStats();
}

void PlayerStats::initialize()
{
    getInventory().setMaxItemCount(-1);
    // items for 5 quick mines
    getInventory()[A13GameplayObjectManager::items.iron] = 50;
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
    m_requestQueues[stack.getItem()->getId()].push_back({stack, requester});
}

void PlayerStats::update()
{
    // Debug
    /*
    if(!m_requests.empty())
    {
        log() << "--------------------------------";
        log() << "Request queue size: " << m_requests.size();
        auto& i = m_requests.back();
        log() << "  FIRST: " << i.stack << " for " << i.requester;
        log() << "--------------------------------";
    }*/

    for(auto& q: m_requestQueues)
    {
        if(q.second.empty())
            continue;

        EGE::Size hsize = q.second.size();
        for(EGE::Size s = 0; s < hsize; s++)
        {
            if(!process(q.second.front()))
            {
                // Save a copy of request on the back of queue.
                // We will try process it later.
                q.second.push_back(q.second.front());
            }

            q.second.pop_front();
        }
    }
}

bool PlayerStats::process(ResourceRequest& request)
{
    if(m_registeredContainers.count(request.requester))
    {
        if(!getInventory().canRemoveItems(request.stack))
        {
            /*log() << "Failed to process resource request: Not enough resources in player inventory";
            log() << "   for stack " << request.stack;*/
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

EGE::SharedPtr<EGE::ObjectMap> PlayerStats::serialize()
{
    // TODO
    auto obj = Container::serialize();
    return obj;
}

bool PlayerStats::deserialize(EGE::SharedPtr<EGE::ObjectMap> obj)
{
    // TODO
    return Container::deserialize(obj);
}

}
