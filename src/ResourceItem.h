#pragma once

#include <ege/gpo/GameplayObject.h>
#include <ege/util.h>

class ResourceItem : public EGE::GameplayObject
{
public:
    ResourceItem(std::string id)
    : EGE::GameplayObject(id) {}

    void setNumericId(EGE::IdType id) { if(!m_numericId) m_numericId = id; }
    EGE::IdType getNumericId() const { return m_numericId; }

    virtual EGE::Vec2d getAtlasPosition() const { int index = m_numericId - 1; return {(double)(index / 256 + index % 16), (double)(index / 16)}; }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {}

private:
    EGE::IdType m_numericId = 0;
};

class ResourceItemStack
{
public:
    ResourceItemStack(ResourceItem* item = nullptr, int count = 1)
    : m_item(item), m_count(count) {}

    ResourceItemStack(std::string itemId, int count = 1);

    ResourceItem* getItem() const { return m_item; }
    int getItemCount() const { return m_count; }

    // Returns false if decrement failed (there
    // will be negative number of items)
    bool tryDecrement(int count);

private:
    int m_count;
    ResourceItem* m_item;
};

std::ostream& operator<<(std::ostream& os, const ResourceItemStack& stack);
