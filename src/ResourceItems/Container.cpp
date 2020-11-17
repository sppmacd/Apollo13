#include "Container.h"

#include "../A13GameplayObjectManager.h"

namespace A13
{

int Container::getItemCount() const
{
    int ic = 0;
    for(auto& stack: m_stacks)
        ic += stack.second;
    return ic;
}

bool Container::canAddItems(ResourceItemStack stack) const
{
    return getMaxItemCount() == -1 || stack.getItemCount() + getItemCount() < getMaxItemCount();
}

bool Container::tryAddItems(ResourceItemStack stack)
{
    if(!canAddItems(stack))
        return false;

    m_stacks[stack.getItem()] += stack.getItemCount();
    return true;
}

bool Container::canRemoveItems(ResourceItemStack stack) const
{
    auto it = m_stacks.find(stack.getItem());

    int count = 0;
    if(it != m_stacks.end())
        count = it->second;

    return count >= stack.getItemCount();
}

bool Container::tryRemoveItems(ResourceItemStack stack)
{
    if(!canAddItems(stack))
        return false;

    m_stacks[stack.getItem()] -= stack.getItemCount();
    return true;
}

int& Container::operator[](ResourceItem* type)
{
    return m_stacks[type];
}

int& Container::operator[](std::string itemId)
{
    return (*this)[A13GameplayObjectManager::instance().resourceItems.findById(itemId)];
}

}
