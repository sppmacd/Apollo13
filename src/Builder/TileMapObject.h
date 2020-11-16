#pragma once

#include <ege/scene/SceneObject2D.h>
#include <ege/scene/TilemapRenderer2D.h>
#include <ege/util.h>

#include <array>
#include <functional>
#include <string>

// TileMap for Builder
template<class _Tilemap>
class TileMapObject : public EGE::SceneObject2D
{
public:
    TileMapObject(EGE::SharedPtr<EGE::Scene> scene, std::array<std::string, _Tilemap::TileType::AdditionalLayerCount + 1>& atlasTextureNames, EGE::SharedPtr<_Tilemap> tilemap);

    void placePart(EGE::Vec2i pos, EGE::SharedPtr<typename _Tilemap::TileType::PartType> part);
    void removePart(EGE::Vec2i pos);

    void onActivate(EGE::Vec2i pos);
    std::string getTooltip(EGE::Vec2i pos);

    EGE::Vec2i mapTilePosition(sf::Vector2f scenePos) const;

    // Use ensureTile() instead of getTile(). The tilemap will be resized
    // if possible and necessary. DO NOT USE IT WITH FIXED TILEMAPS!
    void setUseEnsure(bool ensure = true) { m_useEnsure = ensure; ((EGE::TilemapRenderer2D<_Tilemap>*)m_renderer.get())->setUseEnsure(m_useEnsure); }

    // Check if any object is placed in rect [position] of size [range].
    bool isObjectInArea(EGE::Vec2i position, EGE::Vec2u range) const;

    bool canPartBePlacedHere(EGE::Vec2i pos, EGE::Vec2u partSize) const;
    void setHighlight(EGE::Vec2i offset, EGE::Vec2u size);

    // Function CanPlaceHere canPartBePlacedHere(EGE::Vec2i pos, const typename _Tilemap::TileType& tile);
    // Can be used to handle e.g additional layers.
    void setCanPlaceHere(std::function<CanPlaceHere(EGE::Vec2i)> predicate) { m_placePredicate = predicate; }

    EGE::Vec2d callCustomLayerAtlasMapper(const typename _Tilemap::TileType& tile, EGE::Size layer)
    {
        return m_atlasMapper(m_tilemap.get(), tile, layer);
    }

    void setAtlasMapper(std::function<EGE::Vec2d(_Tilemap*, const typename _Tilemap::TileType&, EGE::Size)> mapper) { m_atlasMapper = mapper; }

    void render(sf::RenderTarget& target, const EGE::RenderStates& states) const;
    void onUpdate(long long tickCounter);

    EGE::SharedPtr<_Tilemap> m_tilemap = nullptr;
    std::array<std::string, _Tilemap::TileType::AdditionalLayerCount + 1> m_builderPartAtlasTextureNames;

private:
    sf::Vector2f m_highlightPosScreen;
    EGE::Vec2i m_highlightPos;

    // TODO: change rendering of highlight to mark every tile differently
    EGE::Vec2u m_highlightSize;

    std::function<CanPlaceHere(EGE::Vec2i)> m_placePredicate;
    std::function<EGE::Vec2d(_Tilemap*, const typename _Tilemap::TileType&, EGE::Size)> m_atlasMapper;
    std::function<void(EGE::Vec2i, const typename _Tilemap::TileType&)> m_onActivate;
    EGE::Map<EGE::Vec2i, EGE::SharedPtr<typename _Tilemap::TileType::PartType>> m_objects;
    bool m_useEnsure = false;
};

#include "TileMapObject.inl"
