#pragma once

#include "A13RocketPart.h"

#include <ege/gui/GUIScreen.h>
#include <ege/scene/CameraObject2D.h>
#include <ege/scene/Scene2D.h>
#include <ege/tilemap/FixedTileMap2D.h>

class A13GUIProjectBuilder : public EGE::GUIScreen
{
public:
    class TileMapObject;

    class PartSelectWidget : public EGE::Widget
    {
    public:
        PartSelectWidget(EGE::Widget* parent, TileMapObject* tmObj)
        : EGE::Widget(parent), m_tmObject(tmObj)
        {
        }

        void onLoad();
        void onMouseButtonRelease(sf::Event::MouseButtonEvent& event);
        void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states = {});

    private:
        EGE::Vector<A13RocketPart*> m_parts;
        sf::Texture* m_atlas = nullptr;
        TileMapObject* m_tmObject;
        int m_index = -1;
    };

    class BgWidget : public EGE::Widget
    {
    public:
        BgWidget(EGE::Widget* parent)
        : EGE::Widget(parent) {}

        void onLoad();
        void onResize(sf::Event::SizeEvent& event);
        void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states = {});

    private:
        sf::Texture* m_bg = nullptr;
    };

    class TileMapObject : public EGE::SceneObject2D
    {
    public:
        struct ObjectState
        {
            // Pointer to object saved in m_objects, or nullptr (no object).
            EGE::FlatPtr<A13RocketPart> obj = nullptr;

            // Distance from corner. (0,0 - left top)
            EGE::Vec2u cornerPos;
        };

        TileMapObject(EGE::SharedPtr<EGE::Scene> scene);

        void placePart(sf::Vector2f pos);
        void removePart(sf::Vector2f pos);

        bool canPartBePlacedHere(sf::Vector2f pos) const;
        EGE::Vec2u getPartSize() const { if(!m_currentPart) return {}; return m_currentPart->getSize(); }
        void render(sf::RenderTarget& target, const EGE::RenderStates& states) const;
        void setTile(EGE::Vec2i offset);
        void setCurrentPart(A13RocketPart* part) { m_currentPart = part; }

        EGE::SharedPtr<EGE::FixedTileMap2D<ObjectState, 18, 64>> m_tilemap;
    private:

        EGE::Map<EGE::Vec2i, A13RocketPart*> m_objects;
        A13RocketPart* m_currentPart = nullptr;
        sf::Vector2f highlightPos;
    };

    A13GUIProjectBuilder(EGE::GUIGameLoop* loop)
    : EGE::GUIScreen(loop) {}

    void onLoad();
    void onResize(sf::Event::SizeEvent& event);
    void onMouseButtonPress(sf::Event::MouseButtonEvent& event);
    void onMouseButtonRelease(sf::Event::MouseButtonEvent& event);
    void onMouseMove(sf::Event::MouseMoveEvent& event);
    void onMouseWheelScroll(sf::Event::MouseWheelScrollEvent& event);

private:
    EGE::SharedPtr<EGE::Scene2D> m_scene;
    EGE::SharedPtr<EGE::CameraObject2D> m_camera;
    EGE::SharedPtr<TileMapObject> m_tileMapObject;
    EGE::SharedPtr<PartSelectWidget> m_partSelector;

    bool m_dragging = false;
    bool m_mousePressed = false;
    sf::Vector2i m_dragStartPos;
    sf::Vector2f m_dragStartCamPos;
    float m_zoom = 4.f;
};
