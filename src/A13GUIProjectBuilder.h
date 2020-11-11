#pragma once

#include "A13RocketPart.h"

#include <ege/gui/GUIScreen.h>
#include <ege/scene/CameraObject2D.h>
#include <ege/scene/Scene2D.h>
#include <ege/tilemap/FixedTileMap2D.h>

class A13GUIProjectBuilder : public EGE::GUIScreen
{
public:
    class TileMapObject : public EGE::SceneObject2D
    {
    public:
        TileMapObject(EGE::SharedPtr<EGE::Scene> scene);
        void placePart(sf::Vector2f pos);

    private:
        struct ObjectState
        {
            // Pointer to object saved in m_objects, or nullptr (no object).
            EGE::FlatPtr<A13RocketPart> obj = nullptr;

            // Distance from corner. (0,0 - left top)
            EGE::Vec2u cornerPos;
        };

        EGE::Map<EGE::Vec2i, EGE::SharedPtr<A13RocketPart>> m_objects;
        EGE::SharedPtr<EGE::FixedTileMap2D<ObjectState, 7, 10>> m_tilemap;
        EGE::SharedPtr<A13RocketPart> m_currentPart = make<A13RocketPart>("default");
    };

    A13GUIProjectBuilder(EGE::GUIGameLoop* loop)
    : EGE::GUIScreen(loop) {}

    void onLoad();
    void onMouseButtonPress(sf::Event::MouseButtonEvent& event);
    void onMouseButtonRelease(sf::Event::MouseButtonEvent& event);
    void onMouseMove(sf::Event::MouseMoveEvent& event);

private:
    EGE::SharedPtr<EGE::Scene2D> m_scene;
    EGE::SharedPtr<EGE::CameraObject2D> m_camera;
    EGE::SharedPtr<TileMapObject> m_tileMapObject;

    bool m_dragging = false;
    bool m_mousePressed = false;
    sf::Vector2i m_dragStartPos;
    sf::Vector2f m_dragStartCamPos;
    float m_zoom = 4.f;
};
