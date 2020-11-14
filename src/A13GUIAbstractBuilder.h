#pragma once

#include <ege/gpo/GameplayObjectRegistry.h>
#include <ege/gui/GUIScreen.h>
#include <ege/gui/Label.h>
#include <ege/scene/CameraObject2D.h>
#include <ege/scene/Scene2D.h>
#include <ege/scene/TilemapRenderer2D.h>
#include <ege/tilemap/ChunkedTileMap2D.h>
#include <ege/tilemap/FixedTileMap2D.h>

template<class _PartType, EGE::Size _AddLayerCount>
struct TilemapObjectState
{
    typedef _PartType PartType;

    static const EGE::Size AdditionalLayerCount = _AddLayerCount;

    // Pointer to object saved in m_objects, or nullptr (no object).
    EGE::FlatPtr<_PartType> obj = nullptr;

    // Additional layers (e.g shadows or base terrain) that can be only 1x1.
    EGE::Uint64 addObjs[AdditionalLayerCount] = {};

    // `obj` distance from corner. (0,0 - left top)
    EGE::Vec2u cornerPos;
};

template <class _PartType, EGE::Size _AddLayerCount>
class A13BuilderTilemap
{
public:
    typedef TilemapObjectState<_PartType, _AddLayerCount> StateType;

    virtual bool useEnsure() { return false; }

    virtual void onActivate(EGE::Vec2i pos, const StateType& state) {}
    virtual std::string getTooltip(EGE::Vec2i pos, const StateType& state) { return ""; }
};

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

    // Function bool canPartBePlacedHere(_Tilemap* tilemap, EGE::Vec2i pos);
    // Can be used to handle e.g additional layers.
    void setCanPartBePlacedHere(std::function<bool(_Tilemap*, EGE::Vec2i)> predicate) { m_placePredicate = predicate; }

    EGE::Vec2d callCustomLayerAtlasMapper(const typename _Tilemap::TileType& tile, EGE::Size layer)
    {
        return m_atlasMapper(m_tilemap.get(), tile, layer);
    }

    void setAtlasMapper(std::function<EGE::Vec2d(_Tilemap*, const typename _Tilemap::TileType&, EGE::Size)> mapper) { m_atlasMapper = mapper; }

    void render(sf::RenderTarget& target, const EGE::RenderStates& states) const;

    EGE::SharedPtr<_Tilemap> m_tilemap = nullptr;
    std::array<std::string, _Tilemap::TileType::AdditionalLayerCount + 1> m_builderPartAtlasTextureNames;

private:
    sf::Vector2f m_highlightPosScreen;
    EGE::Vec2i m_highlightPos;

    // TODO: change rendering of highlight to mark every tile differently
    EGE::Vec2u m_highlightSize;

    std::function<bool(_Tilemap*, EGE::Vec2i)> m_placePredicate;
    std::function<EGE::Vec2d(_Tilemap*, const typename _Tilemap::TileType&, EGE::Size)> m_atlasMapper;
    std::function<void(EGE::Vec2i, const typename _Tilemap::TileType&)> m_onActivate;
    EGE::Map<EGE::Vec2i, EGE::SharedPtr<typename _Tilemap::TileType::PartType>> m_objects;
    bool m_useEnsure = false;
};

// Widget for Part Selection (left panel)
template<class _Tilemap, class _Item>
class PartSelectWidget : public EGE::Widget
{
public:
    PartSelectWidget(EGE::Widget* parent, TileMapObject<_Tilemap>* tmObj)
    : EGE::Widget(parent), m_tmObject(tmObj)
    {
    }

    void onLoad();
    void onMouseButtonRelease(sf::Event::MouseButtonEvent& event);
    void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states = {});

    EGE::Vec2u getPartSize() const { if(m_index == -1) return {}; return m_items[m_index]->getSize(); }

    // -1 for no item
    void setCurrentItemIndex(int index) { m_index = index; }
    _Item* getCurrentItem() { return m_index == -1 ? nullptr : m_items[m_index]; }
    int* getCurrentItemIndex() { return m_index; }

    std::string m_partAtlasTextureName;
    EGE::GameplayObjectRegistry<EGE::String, _Item>* m_gpo = nullptr;

private:
    EGE::Vector<_Item*> m_items;
    sf::Texture* m_atlas = nullptr;
    TileMapObject<_Tilemap>* m_tmObject;
    int m_index = -1;
};

// _Tilemap must be derived from A13BuilderTilemap<BuilderPart, ...> and A13*TilemapForPart<BuilderPart, ...>
// _Item must be derived from BuilderItem
// Logic-level multilayer: any additional layer is an Uint64 which you can use to save any data.
// Renderer layers:
//  0 - Multi-size object layer
//  1..n - Additional layers 0..(n-1)
// TODO: Distinguish tiles (in tilemap) and "items" (in left panel)
//
// Item texture size: 64x64
// Part texture size: 16*size x 16*size
template<class _Tilemap, class _Item>
class A13GUIAbstractBuilder : public EGE::GUIScreen
{
public:
    class BgWidget : public EGE::Widget
    {
    public:
        BgWidget(EGE::Widget* parent)
        : EGE::Widget(parent) {}

        void onLoad();
        void onResize(sf::Event::SizeEvent& event);
        void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states = {});

        std::string m_backgroundTextureName;

    private:
        sf::Texture* m_bg = nullptr;
    };

    A13GUIAbstractBuilder(EGE::GUIGameLoop* loop, EGE::SharedPtr<_Tilemap> tilemap)
    : EGE::GUIScreen(loop), m_tilemap(tilemap) {}

    virtual void onLoad();
    virtual void onResize(sf::Event::SizeEvent& event);
    virtual void onMouseButtonPress(sf::Event::MouseButtonEvent& event);
    virtual void onMouseButtonRelease(sf::Event::MouseButtonEvent& event);
    virtual void onMouseMove(sf::Event::MouseMoveEvent& event);
    virtual void onMouseWheelScroll(sf::Event::MouseWheelScrollEvent& event);
    virtual void onKeyPress(sf::Event::KeyEvent& event);

    void setBuilderBackground(std::string name) { m_backgroundTN = name; }

    // for multi-size object layer
    void setBuilderAtlas(std::string name) { m_builderAtlasTNs[_Tilemap::TileType::AdditionalLayerCount] = name; }

    // for additional layers
    void setBuilderAtlas(std::string name, EGE::Size layer) { ASSERT(layer < _Tilemap::TileType::AdditionalLayerCount); m_builderAtlasTNs[layer] = name; }

    // atlas for items
    void setSelectorAtlas(std::string name) { m_selectorAtlasTN = name; }

    void setUseEnsure(bool ensure = true) { m_useEnsure = ensure; }

    // Returns true if object was placed/removed by this user handler; false otherwise.
    // If it returns false, the default handler (that for multi-tile parts) will be called.
    // Args: tilemap, tilePos, part (part is from GPO)
    virtual bool placePart(_Tilemap*, EGE::Vec2i, typename _Tilemap::TileType::PartType*) { return false; }
    virtual bool removePart(_Tilemap*, EGE::Vec2i, typename _Tilemap::TileType::PartType*) { return false; }

    // Returns atlas position for specific layer.
    // Args: tilemap, tileState, layer
    virtual EGE::Vec2d atlasMapper(_Tilemap*, const typename _Tilemap::TileType&, EGE::Size) { return EGE::Vec2d(0, 0); }

    void setGPO(EGE::GameplayObjectRegistry<EGE::String, _Item>* gpo) { m_gpo = gpo; }

    // TODO: Allow setting custom renderer / atlas mapper (for animations, shadows and more)

    EGE::SharedPtr<TileMapObject<_Tilemap>> m_tileMapObject;
    EGE::SharedPtr<PartSelectWidget<_Tilemap, _Item>> m_partSelector;
    EGE::SharedPtr<_Tilemap> m_tilemap = nullptr;

protected:
    sf::Vector2f screenToScene(sf::Vector2i pos)
    {
        auto& wnd = *getWindow().lock().get();
        return m_scene->mapScreenToScene(wnd, sf::Vector2i(pos.x, pos.y), getView(wnd));
    }

private:
    EGE::SharedPtr<EGE::Scene2D> m_scene;
    EGE::SharedPtr<EGE::CameraObject2D> m_camera;
    EGE::SharedPtr<EGE::Label> m_toolTipLabel;

    std::array<std::string, _Tilemap::TileType::AdditionalLayerCount + 1> m_builderAtlasTNs;
    std::string m_selectorAtlasTN;
    std::string m_backgroundTN;
    std::string m_tooltip;
    EGE::GameplayObjectRegistry<EGE::String, _Item>* m_gpo = nullptr;
    bool m_useEnsure = false;

    bool m_dragging = false;
    bool m_mousePressed = false;
    sf::Vector2i m_dragStartPos;
    sf::Vector2f m_dragStartCamPos;
    float m_zoom = 4.f;
};

namespace Aliases
{

// Aliases for tilemaps
template<class _PartType, EGE::Size SX, EGE::Size SY, EGE::Size _AddLayers = 0>
using A13FixedTilemapForPart = EGE::FixedTileMap2D<TilemapObjectState<_PartType, _AddLayers>, SX, SY>;

template<class _PartType, EGE::Size SX = 16, EGE::Size SY = 16, EGE::Size _AddLayers = 0>
using A13ChunkedTilemapForPart = EGE::ChunkedTileMap2D<TilemapObjectState<_PartType, _AddLayers>, SX, SY>;

}

#include "A13GUIAbstractBuilder.inl"
