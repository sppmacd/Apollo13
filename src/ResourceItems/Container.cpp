#include "Container.h"

#include <algorithm>

namespace A13
{

bool Container::loadItemsFrom(Container* other, int maxCount)
{
    // If maxCount is infinite, set it to size of other inventory
    if(maxCount == -1)
        maxCount = other->m_inventory.getItemCount();

    // Do nothing if other inventory is empty
    if(other->m_inventory.getItemCount() == 0)
        return false;

    // Make a copy of other inventory
    std::vector<ResourceItemStack> sortedInv;
    for(auto& pr: other->m_inventory)
        sortedInv.push_back({pr.first, pr.second});

    // Sort item stacks by item count.
    std::stable_sort(sortedInv.begin(), sortedInv.end(), [](const ResourceItemStack& l, const ResourceItemStack& r)
    {
        return l.getItemCount() > r.getItemCount();
    });

    // Transfer items until there are no place in our inventory (break)
    // or we run out of items in carrier (the loop finishes then)
    for(ResourceItemStack& stack: sortedInv)
    {
        // Update stack by needed stack
        stack = allowLoadItem(stack);

        if(stack.getItemCount() == 0)
            continue;

        if(m_inventory.isFull() || maxCount == 0)
            return false;

        int remaining = m_inventory.getRemainingSpace();
        if(remaining <= stack.getItemCount())
        {
            int maxCmin = std::min(remaining, maxCount);
            other->m_inventory.tryRemoveItems({stack.getItem(), maxCmin});
            m_inventory.tryAddItems({stack.getItem(), maxCmin});

            if(remaining < stack.getItemCount())
                return false;
        }
        else
        {
            int maxCmin = std::min(stack.getItemCount(), maxCount);
            ResourceItemStack is2 = {stack.getItem(), maxCmin};
            other->m_inventory.tryRemoveItems(is2);
            m_inventory.tryAddItems(is2);
            maxCount -= maxCmin;
        }
    }
    return true;
}

}
