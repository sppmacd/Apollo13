#include "ResourceItem.h"

#include "A13GameplayObjectManager.h"

ResourceItemStack::ResourceItemStack(std::string itemId, int count)
: m_item(A13GameplayObjectManager::instance().resourceItems.findById(itemId)), m_count(count)
{
    if(!m_item)
    {
        log(LogLevel::Error) << "[RIS] Couldn't find item " << itemId;
        CRASH();
    }
}

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

std::ostream& operator<<(std::ostream& os, const ResourceItemStack& stack)
{
    return os << stack.getItemCount() << "x " << (stack.getItem() ? stack.getItem()->getId() : "<null>");
}
