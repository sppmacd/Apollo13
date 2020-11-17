#include "ResourceItem.h"

#include "A13GameplayObjectManager.h"

ResourceItemStack::ResourceItemStack(std::string itemId, int count)
: m_item(A13GameplayObjectManager::instance().resourceItems.findById(itemId)), m_count(count) {}

bool ResourceItemStack::tryDecrement(int count)
{
    if(m_count - count < 0)
        return false;
    else if(m_count - count == 0)
    {
        m_item = nullptr;
        m_count = 0;
    }
    else
    {
        m_count -= count;
    }
    return true;
}
