#include "ResourceItem.h"

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
