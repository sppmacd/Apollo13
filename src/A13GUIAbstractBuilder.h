#pragma once

#include <ege/gpo/GameplayObjectRegistry.h>
#include <ege/gui/GUIScreen.h>
#include <ege/gui/Label.h>
#include <ege/scene/CameraObject2D.h>
#include <ege/scene/Scene2D.h>
#include <ege/tilemap/ChunkedTileMap2D.h>
#include <ege/tilemap/FixedTileMap2D.h>

#include "Builder/TilemapObjectState.h"
#include "Builder/CanPlaceHere.h"
#include "Builder/A13BuilderTilemap.h"
#include "Builder/TileMapObject.h"
#include "Builder/PartSelectWidget.h"

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

    A13GUIAbstractBuilder(EGE::GUIScreen* screen, EGE::SharedPtr<_Tilemap> tilemap)
    : EGE::GUIScreen(screen), m_tilemap(tilemap) {}

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

    CanPlaceHere canPlaceHere(EGE::Vec2i);
    virtual CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const typename _Tilemap::TileType& tile, _Item* item) { return CanPlaceHere::NotLoaded; }

    // Returns atlas position for specific layer.
    // Args: tilemap, tileState, layer
    virtual EGE::Vec2d atlasMapper(_Tilemap*, const typename _Tilemap::TileType&, EGE::Size) { return EGE::Vec2d(0, 0); }

    void setGPO(EGE::GameplayObjectRegistry<EGE::String, _Item>* gpo) { m_gpo = gpo; }

    // TODO: Allow setting custom renderer / atlas mapper (for animations, shadows and more)

    EGE::SharedPtr<TileMapObject<_Tilemap>> m_tileMapObject;
    EGE::SharedPtr<PartSelectWidget<_Tilemap, _Item>> m_partSelector;
    EGE::SharedPtr<_Tilemap> m_tilemap = nullptr;

    void setMaxMeta(int meta) { m_maxMeta = meta; }

protected:
    sf::Vector2f screenToScene(sf::Vector2i pos)
    {
        auto& wnd = *getWindow().lock().get();
        return m_scene->mapScreenToScene(wnd, sf::Vector2i(pos.x, pos.y), getView(wnd));
    }

protected:
    sf::Font* m_font = nullptr;

private:
    void updateHighlight(sf::Vector2i mousePos)
    {
        auto& wnd = *getWindow().lock().get();
        sf::Vector2f mouseScenePos = m_scene->mapScreenToScene(wnd, sf::Vector2i(mousePos.x, mousePos.y), getView(wnd)) - m_tileMapObject->getPosition();
        if(m_partSelector->getCurrentItem())
        {
            EGE::Vec2i tileRel = m_tileMapObject->m_tilemap->getTileAlignedPos({mouseScenePos.x, mouseScenePos.y});
            auto item = m_partSelector->getCurrentItem();
            auto part = item->getPart();
            m_tileMapObject->setHighlight(tileRel, part->getSize(), part->getAtlasPosition(m_meta), part->getHighlightLayer());
        }
    }

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
    int m_meta = 0;
    int m_maxMeta = 1;
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
