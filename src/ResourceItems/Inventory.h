#pragma once

#include "../ResourceItem.h"

namespace A13
{

// The container can hold an user-specified
// item count of any types.
class Inventory
{
public:
    typedef EGE::Map<ResourceItem*, int> ArrayType;

    Inventory(const ArrayType& stacks = {})
    : m_stacks(stacks) {}

    // Set it to -1 for infinite storage.
    void setMaxItemCount(int n) { m_maxItems = n; }
    int getMaxItemCount() const { return m_maxItems; }
    int getItemCount() const;
    bool isFull() { return getMaxItemCount() != -1 && getItemCount() >= getMaxItemCount(); }
    int getRemainingSpace() { return getMaxItemCount() == -1 ? (1<<30) : getMaxItemCount() - getItemCount(); }

    // TODO: Add these for Cost
    bool canAddItems(ResourceItemStack stack) const;
    bool tryAddItems(ResourceItemStack stack);
    bool canRemoveItems(ResourceItemStack stack) const;
    bool tryRemoveItems(ResourceItemStack stack);

    const ArrayType::const_iterator begin() const { return m_stacks.begin(); }
    const ArrayType::const_iterator end() const { return m_stacks.end(); }

    int& operator[](ResourceItem* item);
    int& operator[](std::string itemId);

private:
    int m_maxItems = -1;
    ArrayType m_stacks;
};

}
