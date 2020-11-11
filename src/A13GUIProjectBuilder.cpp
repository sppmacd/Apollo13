#include "A13GUIProjectBuilder.h"

#include <ege/debug/Logger.h>
#include <ege/scene/SceneWidget.h>
#include <ege/scene/TilemapRenderer2D.h>

A13GUIProjectBuilder::TileMapObject::TileMapObject(EGE::SharedPtr<EGE::Scene> scene)
: EGE::SceneObject2D(scene, "a13:project_builder:tilemap")
{

    m_tilemap = make<EGE::FixedTileMap2D<ObjectState, 7, 10>>();

    // Initialize tilemap
    m_tilemap->setTileSize({16, 16});
    m_tilemap->initialize({});

    // Set renderer
    auto renderer = make<EGE::TilemapRenderer2D<decltype(m_tilemap)::element_type>>(scene, m_tilemap);
    renderer->setAtlasTextureName("rocket_parts.png");
    renderer->setTileAtlasMapper([](const ObjectState& state)->EGE::Vec2d {
                                    if(!state.obj)
                                        return {0, 0};
                                    EGE::Vec2d atlasPos = state.obj->getAtlasPosition();
                                    return {atlasPos.x * 16 + state.cornerPos.x * 16, atlasPos.y * 16+ state.cornerPos.y * 16};
                                 });
    setRenderer(renderer);

    setPosition({0, 0});
}

void A13GUIProjectBuilder::TileMapObject::placePart(sf::Vector2f pos)
{
    // Calculate tile position
    sf::Vector2f relPos = pos - getPosition();
    EGE::Vec2u tileRel = m_tilemap->getTileAlignedPos({relPos.x, relPos.y});

    log() << "TileRel " << tileRel.x << "," << tileRel.y;

    // Check if objects can be placed here
    // TODO: center selection
    EGE::Vec2u partSize = m_currentPart->getSize();
    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = m_tilemap->getTile({x + tileRel.x, y + tileRel.y});
        if(!tile)
            return; // Out of bounds!
        if(tile->obj)
            return; // Something is already here :(
    }

    auto it = m_objects.insert(std::make_pair(EGE::Vec2i(tileRel.x, tileRel.y), make<A13RocketPart>("test")));

    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = m_tilemap->getTile({x + tileRel.x, y + tileRel.y});
        tile->obj = it.first->second.get();
        tile->cornerPos = {x, y};
    }
}

void A13GUIProjectBuilder::onLoad()
{
    EGE::GUIScreen::onLoad();

    // Allocate scene.
    m_scene = make<EGE::Scene2D>(getLoop());

    // Add tilemap to scene.
    m_tileMapObject = make<A13GUIProjectBuilder::TileMapObject>(m_scene);
    m_scene->addObject(m_tileMapObject);

    // Add camera
    m_camera = make<EGE::CameraObject2D>(m_scene);
    m_camera->setScalingMode(EGE::ScalingMode::Centered);
    m_camera->setZoom(4.f);
    m_zoom = 4.f;
    m_scene->addObject(m_camera);
    m_scene->setCamera(m_camera);

    // Add SceneWidget.
    addWidget(make<EGE::SceneWidget>(this, m_scene));
}

void A13GUIProjectBuilder::onMouseButtonPress(sf::Event::MouseButtonEvent& event)
{
    EGE::GUIScreen::onMouseButtonPress(event);
    if(event.button == sf::Mouse::Left)
    {
        m_dragStartPos = sf::Vector2i(event.x, event.y);
        m_dragStartCamPos = m_camera->getEyePosition();
        m_mousePressed = true;
    }
}

void A13GUIProjectBuilder::onMouseButtonRelease(sf::Event::MouseButtonEvent& event)
{
    EGE::GUIScreen::onMouseButtonRelease(event);
    if(event.button == sf::Mouse::Left)
    {
        m_mousePressed = false;
        if(m_dragging)
            m_dragging = false;
        else
        {
            log() << "place object";
            m_tileMapObject->placePart(m_scene->mapScreenToScene(*getWindow().lock().get(), sf::Vector2i(event.x, event.y)));
        }
    }
}

void A13GUIProjectBuilder::onMouseMove(sf::Event::MouseMoveEvent& event)
{
    EGE::GUIScreen::onMouseMove(event);

    sf::Vector2f currentPos(event.x, event.y);
    auto diff = (currentPos - sf::Vector2f(m_dragStartPos));

    if(!m_dragging && m_mousePressed && EGE::VectorOperations::lengthSquared(EGE::Vec2d(diff.x, diff.y)) > 16)
    {
        m_dragging = true;
    }

    if(m_dragging)
    {
        // Move camera.
        auto vec = m_dragStartCamPos - diff / m_zoom;
        m_camera->setEyePosition(vec);
    }
}
