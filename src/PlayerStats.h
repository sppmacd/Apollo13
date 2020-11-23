#pragma once

#include "A13GameplayObjectManager.h"
#include "ResourceItem.h"
#include "ResourceItems/Container.h"

#include <ege/util.h>

namespace A13
{

class PlayerStats : public Container
{
public:
    PlayerStats()
    : Container(Container::NoRegisterInQueue) {}

    EGE_SINGLETON(PlayerStats);

    void initialize();

    void registerContainer(Container* container);
    void unregisterContainer(Container* container);

    void addResourceItemRequest(ResourceItemStack stack, Container* requester);

    void update();

private:
    struct ResourceRequest
    {
        ResourceItemStack stack;
        Container* requester;
    };

    bool process(ResourceRequest& request);

    EGE::Set<Container*> m_registeredContainers;
    std::queue<ResourceRequest> m_requests;
};

}
