#pragma once

#include <ege/scene/Scene.h>
#include <ege/util.h>

template <class _PartType, EGE::Size _AddLayerCount>
class A13BuilderTilemap
{
public:
    typedef TilemapObjectState<_PartType, _AddLayerCount> StateType;
    typedef EGE::Map<EGE::Vec2i, EGE::SharedPtr<_PartType>> ObjectMap;

    virtual bool onRemove(EGE::Vec2i) { return false; }

    virtual std::string getTooltip(EGE::Vec2i, const StateType&) { return ""; }

    virtual bool useEnsure() { return false; }

    // Returns a reference to object list.
    ObjectMap& getParts() { return m_parts; }

    EGE::SharedPtr<EGE::Scene> scene;

protected:
    ObjectMap m_parts;
};
