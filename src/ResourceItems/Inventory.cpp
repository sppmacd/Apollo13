#include "Inventory.h"

#include "../A13GameplayObjectManager.h"

namespace A13
{

int Inventory::getItemCount() const
{
    int ic = 0;
    for(auto& stack: m_stacks)
        ic += stack.second;
    return ic;
}

bool Inventory::canAddItems(ResourceItemStack stack) const
{
    return getMaxItemCount() == -1 || stack.getItemCount() + getItemCount() <= getMaxItemCount();
}

bool Inventory::tryAddItems(ResourceItemStack stack)
{
    if(!canAddItems(stack))
        return false;

    m_stacks[stack.getItem()] += stack.getItemCount();
    return true;
}

bool Inventory::canRemoveItems(ResourceItemStack stack) const
{
    auto it = m_stacks.find(stack.getItem());

    int count = 0;
    if(it != m_stacks.end())
        count = it->second;

    return count >= stack.getItemCount();
}

bool Inventory::tryRemoveItems(ResourceItemStack stack)
{
    if(!canRemoveItems(stack))
        return false;

    m_stacks[stack.getItem()] -= stack.getItemCount();
    return true;
}

int& Inventory::operator[](ResourceItem* type)
{
    return m_stacks[type];
}

int& Inventory::operator[](std::string itemId)
{
    return (*this)[A13GameplayObjectManager::instance().resourceItems.findById(itemId)];
}

}
