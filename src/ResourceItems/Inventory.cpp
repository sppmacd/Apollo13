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

    m_stacks[stack.getItem()->getId()] += stack.getItemCount();
    return true;
}

bool Inventory::canRemoveItems(ResourceItemStack stack) const
{
    auto it = m_stacks.find(stack.getItem()->getId());

    int count = 0;
    if(it != m_stacks.end())
        count = it->second;

    return count >= stack.getItemCount();
}

bool Inventory::tryRemoveItems(ResourceItemStack stack)
{
    if(!canRemoveItems(stack))
        return false;

    m_stacks[stack.getItem()->getId()] -= stack.getItemCount();
    return true;
}

bool Inventory::canAddItems(Cost cost) const
{
    for(auto& it: cost)
    {
        if(!canAddItems(it))
            return false;
    }
    return true;
}

bool Inventory::tryAddItems(Cost cost)
{
    if(!canAddItems(cost))
        return false;

    for(auto& it: cost)
    {
        if(!tryAddItems(it))
            return false;
    }
    return true;
}

bool Inventory::canRemoveItems(Cost cost) const
{
    for(auto& it: cost)
    {
        if(!canRemoveItems(it))
            return false;
    }
    return true;
}

bool Inventory::tryRemoveItems(Cost cost)
{
    if(!canRemoveItems(cost))
        return false;

    for(auto& it: cost)
    {
        if(!tryRemoveItems(it))
            return false;
    }
    return true;
}

int& Inventory::operator[](ResourceItem* type)
{
    return m_stacks[type->getId()];
}

int& Inventory::operator[](std::string itemId)
{
    return m_stacks[itemId];
}

EGE::SharedPtr<EGE::ObjectMap> Inventory::serialize()
{
    auto _map = make<EGE::ObjectMap>();
    _map->addInt("maxItems", m_maxItems);
    auto _items = make<EGE::ObjectMap>();

    for(auto it: *this)
    {
        _items->addUnsignedInt(it.first, it.second);
    }
    _map->addObject("items", _items);
    return _map;
}

bool Inventory::deserialize(EGE::SharedPtr<EGE::ObjectMap> obj)
{
    auto _maxItems = obj->getObject("maxItems");
    if(!_maxItems.expired() && _maxItems.lock()->isFloat())
    {
        m_maxItems = (int)_maxItems.lock()->asFloat();
        log() << m_maxItems;
    }

    auto _items = obj->getObject("items");
    if(!_items.expired() && _items.lock()->isMap())
    {
        for(auto& it: _items.lock()->asMap())
        {
            if(!it.second->isFloat())
                return false;
            m_stacks[it.first] = (int)it.second->asFloat();
            log() << it.second->asFloat();
        }
    }
    return true;
}

void Inventory::clear()
{
    m_stacks = {};
}

}
