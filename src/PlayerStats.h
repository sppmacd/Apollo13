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

    static PlayerStats& instance();

    void initialize();

    void registerContainer(Container* container);
    void unregisterContainer(Container* container);

    void addResourceItemRequest(ResourceItemStack stack, Container* requester);

    void update();

    int getQueueSize(ResourceItem* item) { return m_requestQueues[item->getId()].size(); }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() override;
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap> obj) override;

    // TODO: make it private
    struct ResourceRequest
    {
        ResourceItemStack stack;
        Container* requester;
    };

    // TODO: serialize queues
    EGE::Map<std::string, std::deque<ResourceRequest>> m_requestQueues;
private:

    bool process(ResourceRequest& request);

    EGE::Set<Container*> m_registeredContainers;
};

}
