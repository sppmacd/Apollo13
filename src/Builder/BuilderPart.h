#pragma once

#include <ege/util/Vector.h>

// A part that can be used in Builder GUI (on tilemap).
// TODO: Make it Serializable to allow world saving!
template<class _Tilemap>
class BuilderPart
{
public:
    virtual EGE::Vec2d getAtlasPosition() const = 0;
    virtual EGE::Vec2u getSize() const = 0;

    // Called before object is placed.
    // Return false if object shouldn't be placed here.
    // Can be used when you want to initialize part a bit.
    // e.g mine will search for ore coordinates
    // Args: tilemap, partPos
    virtual bool onPlace(_Tilemap*, EGE::Vec2i) { return true; }

    // Called before object is removed.
    // Return false if object shouldn't be removed.
    // Args: tilemap, partPos
    virtual bool onRemove(_Tilemap*, EGE::Vec2i) { return true; }

    // Called on right-click of part.
    // Args: tilemap, partPos
    virtual void onActivate(_Tilemap*, EGE::Vec2i) {}

    // Called every tick.
    virtual void onUpdate(_Tilemap*, EGE::Vec2i, EGE::TickCount) {}
};
