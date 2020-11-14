#pragma once

#include <ege/util.h>

template <class _PartType, EGE::Size _AddLayerCount>
class A13BuilderTilemap
{
public:
    typedef TilemapObjectState<_PartType, _AddLayerCount> StateType;

    virtual bool useEnsure() { return false; }

    // onPlace is called for PART, not for TILE!
    // return false if object shouldn't be placed here.
    // Args: partPos, part
    virtual bool onPlace(EGE::Vec2i, EGE::SharedPtr<_PartType>) { return true; }

    virtual void onActivate(EGE::Vec2i, const StateType&) {}
    virtual std::string getTooltip(EGE::Vec2i, const StateType&) { return ""; }
};
