#pragma once

#include <ege/util.h>

template<class _PartType, EGE::Size _AddLayerCount>
struct TilemapObjectState
{
    typedef _PartType PartType;

    static const EGE::Size AdditionalLayerCount = _AddLayerCount;

    // Pointer to object saved in m_parts, or nullptr (no object).
    EGE::FlatPtr<_PartType> part = nullptr;

    // Additional layers (e.g shadows or base terrain) that can be only 1x1.
    EGE::Uint64 addObjs[AdditionalLayerCount] = {};

    // `obj` distance from corner. (0,0 - left top)
    EGE::Vec2u cornerPos;

    // Metadata (e.g rotation)
    int meta = 0;
};
