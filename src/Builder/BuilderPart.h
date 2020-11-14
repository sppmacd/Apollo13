#pragma once

#include <ege/util/Vector.h>

// A part that can be used in Builder GUI (on tilemap).
// TODO: Make it Serializable to allow world saving!
class BuilderPart
{
public:
    virtual EGE::Vec2d getAtlasPosition() const = 0;
    virtual EGE::Vec2u getSize() const = 0;
};
