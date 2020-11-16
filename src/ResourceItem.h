#pragma once

#include <ege/gpo/GameplayObject.h>
#include <ege/util.h>

class ResourceItem : public EGE::GameplayObject
{
public:
    ResourceItem(std::string id)
    : EGE::GameplayObject(id) {}

    void setNumericId(EGE::IdType id) { if(!m_numericId) m_numericId = id; }
    EGE::IdType getNumericId() { return m_numericId; }

    virtual EGE::Vec2d getAtlasPosition() { int index = m_numericId - 1; return {index / 256 + index % 16, index / 16}; }

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

    ResourceItem* getItem() { return m_item; }

    int getItemCount() { return m_count; }

    // Returns false if decrement failed (there
    // will be negative number of items)
    bool tryDecrement(int count);

private:
    int m_count;
    ResourceItem* m_item;
};
