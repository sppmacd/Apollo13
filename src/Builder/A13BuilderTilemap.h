#pragma once

#include <ege/util.h>

template <class _PartType, EGE::Size _AddLayerCount>
class A13BuilderTilemap
{
public:
    typedef TilemapObjectState<_PartType, _AddLayerCount> StateType;

    virtual std::string getTooltip(EGE::Vec2i, const StateType&) { return ""; }

    virtual bool useEnsure() { return false; }
};
