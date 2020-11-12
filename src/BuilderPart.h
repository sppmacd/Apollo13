#pragma once

#include <ege/util/Vector.h>

// A part that can be used in Builder GUI.
class BuilderPart
{
public:
    virtual EGE::Vec2d getAtlasPosition() const = 0;
    virtual EGE::Vec2u getSize() const = 0;
};
