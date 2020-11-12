#pragma once

#include <ege/gui/GUIScreen.h>
#include <ege/scene/CameraObject2D.h>
#include <ege/scene/Scene2D.h>
#include <ege/scene/TilemapRenderer2D.h>
#include <ege/tilemap/ChunkedTileMap2D.h>
#include <ege/tilemap/FixedTileMap2D.h>

template<class _PartType3>
struct TilemapObjectState
{
    typedef _PartType3 PartType;

    // Pointer to object saved in m_objects, or nullptr (no object).
    EGE::FlatPtr<_PartType3> obj = nullptr;

    // Distance from corner. (0,0 - left top)
    EGE::Vec2u cornerPos;
};

// TileMap for Builder
template<class _Tilemap>
class TileMapObject : public EGE::SceneObject2D
{
public:
    TileMapObject(EGE::SharedPtr<EGE::Scene> scene, std::string atlasTextureName, EGE::SharedPtr<_Tilemap> tilemap);

    void placePart(sf::Vector2f pos);
    void removePart(sf::Vector2f pos);

    // Use ensureTile() instead of getTile(). The tilemap will be resized
    // if possible and necessary. DO NOT USE IT WITH FIXED TILEMAPS!
    void setUseEnsure(bool ensure = true) { m_useEnsure = ensure; ((EGE::TilemapRenderer2D<_Tilemap>*)m_renderer.get())->setUseEnsure(m_useEnsure); }

    bool canPartBePlacedHere(sf::Vector2f pos) const;
    EGE::Vec2u getPartSize() const { if(!m_currentPart) return {}; return m_currentPart->getSize(); }
    void render(sf::RenderTarget& target, const EGE::RenderStates& states) const;
    void setTile(EGE::Vec2i offset);
    void setCurrentPart(typename _Tilemap::TileType::PartType* part) { m_currentPart = part; }

    EGE::SharedPtr<_Tilemap> m_tilemap = nullptr;

    std::string m_builderPartAtlasTextureName;
private:

    EGE::Map<EGE::Vec2i, typename _Tilemap::TileType::PartType*> m_objects;
    typename _Tilemap::TileType::PartType* m_currentPart = nullptr;
    sf::Vector2f highlightPos;
    bool m_useEnsure = false;
};

// Widget for Part Selection
template<class _Tilemap>
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

    std::string m_partAtlasTextureName;
    EGE::GameplayObjectRegistry<EGE::String, typename _Tilemap::TileType::PartType>* m_gpo = nullptr;

private:
    EGE::Vector<typename _Tilemap::TileType::PartType*> m_parts;
    sf::Texture* m_atlas = nullptr;
    TileMapObject<_Tilemap>* m_tmObject;
    int m_index = -1;
};

// _Tilemap must be derived from EGE::TileMap2D<TilemapObjectState<BuilderPart>>
template<class _Tilemap>
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

    void setBuilderBackground(std::string name) { m_backgroundTN = name; }
    void setBuilderAtlas(std::string name) { m_builderAtlasTN = name; }
    void setSelectorAtlas(std::string name) { m_selectorAtlasTN = name; }
    void setGPO(EGE::GameplayObjectRegistry<EGE::String, typename _Tilemap::TileType::PartType>* gpo) { m_gpo = gpo; }
    void setUseEnsure(bool ensure = true) { m_useEnsure = ensure; }

    EGE::SharedPtr<TileMapObject<_Tilemap>> m_tileMapObject;
    EGE::SharedPtr<PartSelectWidget<_Tilemap>> m_partSelector;
    EGE::SharedPtr<_Tilemap> m_tilemap = nullptr;

private:
    EGE::SharedPtr<EGE::Scene2D> m_scene;
    EGE::SharedPtr<EGE::CameraObject2D> m_camera;

    std::string m_builderAtlasTN;
    std::string m_selectorAtlasTN;
    std::string m_backgroundTN;
    EGE::GameplayObjectRegistry<EGE::String, typename _Tilemap::TileType::PartType>* m_gpo = nullptr;
    bool m_useEnsure = false;

    bool m_dragging = false;
    bool m_mousePressed = false;
    sf::Vector2i m_dragStartPos;
    sf::Vector2f m_dragStartCamPos;
    float m_zoom = 4.f;
};

#include "A13GUIAbstractBuilder.inl"

namespace Aliases
{

// Aliases for tilemaps
template<class _PartType, EGE::Size SX, EGE::Size SY>
using A13FixedTilemapForPart = EGE::FixedTileMap2D<TilemapObjectState<_PartType>, SX, SY>;

template<class _PartType, EGE::Size SX = 16, EGE::Size SY = 16>
using A13ChunkedTilemapForPart = EGE::ChunkedTileMap2D<TilemapObjectState<_PartType>, SX, SY>;

}
