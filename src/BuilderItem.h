#pragma once

// A part that can be **placed** in Builder GUI.
template<class _Tilemap>
class BuilderItem
{
public:
    // returns position of item icon in part selector atlas (for left panel)
    virtual EGE::Vec2d getAtlasPosition() const = 0;

    // left-click on builder
    // returns false if object should be placed by default handler
    // args: tilemap, tilePos
    virtual bool onPlace(_Tilemap*, EGE::Vec2i) const { return false; };

    // returns part that corresponds to that item.
    virtual EGE::SharedPtr<typename _Tilemap::TileType::PartType> getPart() const = 0;
};
