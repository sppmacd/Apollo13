#pragma once

#include <ege/gui/GUIScreen.h>
#include <ege/scene/CameraObject2D.h>
#include <ege/scene/Scene2D.h>
#include <ege/tilemap/FixedTileMap2D.h>

// TileMap for Builder
template<class _PartType2, EGE::Size SX, EGE::Size SY>
class TileMapObject : public EGE::SceneObject2D
{
public:
    template<class _PartType3>
    struct ObjectState
    {
        // Pointer to object saved in m_objects, or nullptr (no object).
        EGE::FlatPtr<_PartType3> obj = nullptr;

        // Distance from corner. (0,0 - left top)
        EGE::Vec2u cornerPos;
    };

    TileMapObject(EGE::SharedPtr<EGE::Scene> scene, std::string atlasTextureName);

    void placePart(sf::Vector2f pos);
    void removePart(sf::Vector2f pos);

    bool canPartBePlacedHere(sf::Vector2f pos) const;
    EGE::Vec2u getPartSize() const { if(!m_currentPart) return {}; return m_currentPart->getSize(); }
    void render(sf::RenderTarget& target, const EGE::RenderStates& states) const;
    void setTile(EGE::Vec2i offset);
    void setCurrentPart(_PartType2* part) { m_currentPart = part; }

    EGE::SharedPtr<EGE::FixedTileMap2D<ObjectState<_PartType2>, SX, SY>> m_tilemap;

    std::string m_builderPartAtlasTextureName;
private:

    EGE::Map<EGE::Vec2i, _PartType2*> m_objects;
    _PartType2* m_currentPart = nullptr;
    sf::Vector2f highlightPos;
};

// Widget for Part Selection
template<class _PartType2, EGE::Size SX, EGE::Size SY>
class PartSelectWidget : public EGE::Widget
{
public:
    PartSelectWidget(EGE::Widget* parent, TileMapObject<_PartType2, SX, SY>* tmObj)
    : EGE::Widget(parent), m_tmObject(tmObj)
    {
    }

    void onLoad();
    void onMouseButtonRelease(sf::Event::MouseButtonEvent& event);
    void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states = {});

    std::string m_partAtlasTextureName;

private:
    EGE::Vector<_PartType2*> m_parts;
    sf::Texture* m_atlas = nullptr;
    TileMapObject<_PartType2, SX, SY>* m_tmObject;
    int m_index = -1;
};

// PartType must be derived and implement A13BuilderPart!
template<class _PartType, EGE::Size SX, EGE::Size SY>
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

    A13GUIAbstractBuilder(EGE::GUIGameLoop* loop)
    : EGE::GUIScreen(loop) {}

    virtual void onLoad();
    virtual void onResize(sf::Event::SizeEvent& event);
    virtual void onMouseButtonPress(sf::Event::MouseButtonEvent& event);
    virtual void onMouseButtonRelease(sf::Event::MouseButtonEvent& event);
    virtual void onMouseMove(sf::Event::MouseMoveEvent& event);
    virtual void onMouseWheelScroll(sf::Event::MouseWheelScrollEvent& event);

    void setBuilderBackground(std::string name) { m_backgroundTN = name; }
    void setBuilderAtlas(std::string name) { m_builderAtlasTN = name; }
    void setSelectorAtlas(std::string name) { m_selectorAtlasTN = name; }

private:
    EGE::SharedPtr<EGE::Scene2D> m_scene;
    EGE::SharedPtr<EGE::CameraObject2D> m_camera;
    EGE::SharedPtr<TileMapObject<_PartType, SX, SY>> m_tileMapObject;
    EGE::SharedPtr<PartSelectWidget<_PartType, SX, SY>> m_partSelector;

    std::string m_builderAtlasTN;
    std::string m_selectorAtlasTN;
    std::string m_backgroundTN;

    bool m_dragging = false;
    bool m_mousePressed = false;
    sf::Vector2i m_dragStartPos;
    sf::Vector2f m_dragStartCamPos;
    float m_zoom = 4.f;
};

#include "A13GUIAbstractBuilder.inl"
