#include "A13GUIProjectBuilder.h"

#include "A13GameplayObjectManager.h"

#include <ege/debug/Logger.h>
#include <ege/scene/SceneWidget.h>
#include <ege/scene/TilemapRenderer2D.h>
#include <ege/util/Random.h>

template<class _PartType, EGE::Size SX, EGE::Size SY>
void PartSelectWidget<_PartType, SX, SY>::onLoad()
{
    auto& gpo = A13GameplayObjectManager::instance().rocketParts;

    for(auto& part : gpo)
    {
        m_parts.push_back(part.second);
    }
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void PartSelectWidget<_PartType, SX, SY>::onMouseButtonRelease(sf::Event::MouseButtonEvent& event)
{
    if(m_leftClicked)
    {
        if(event.x > 10 && event.x < getSize().x - 10)
        {
            size_t index = (event.y) / getSize().x;
            log() << "partSelect " << index;
            if(index >= 0 && index < m_parts.size())
            {
                m_tmObject->setCurrentPart(m_parts[index]);
                m_index = index;
            }
        }
    }
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void PartSelectWidget<_PartType, SX, SY>::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    if(!m_atlas)
        m_atlas = getLoop()->getResourceManager().lock()->getTexture(m_partAtlasTextureName).get();

    // Background
    EGE::Renderer renderer(target);
    renderer.setStates(states);
    renderer.renderRectangle(0, 0, getSize().x, getSize().y, sf::Color(0, 0, 0, 127));

    // Highlight (if rendering on window)
    sf::RenderWindow* wnd = (sf::RenderWindow*)&target;
    if(wnd)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*wnd);
        if(mousePos.x < getSize().x)
        {
            size_t index = (mousePos.y) / getSize().x;
            if(index >= 0 && index < m_parts.size())
                renderer.renderRectangle(0, getSize().x * index, getSize().x, getSize().x, sf::Color(0, 0, 0, 127));
        }
    }

    // Permanent highlight (For active object)
    size_t index = m_index;
    renderer.renderRectangle(0, getSize().x * index, getSize().x, getSize().x, sf::Color(0, 255, 0, 127));

    // Objects
    size_t c = 0;
    for(auto part: m_parts)
    {
        sf::IntRect texRect;
        texRect.left = part->getAtlasPosition().x * 16;
        texRect.top = part->getAtlasPosition().y * 16;
        texRect.width = part->getSize().x * 16;
        texRect.height = part->getSize().y * 16;
        renderer.renderTexturedRectangle(10, getSize().x * c + 10, getSize().x - 20, getSize().x - 20, *m_atlas, texRect);
        c++;
    }
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void A13GUIAbstractBuilder<_PartType, SX, SY>::BgWidget::onLoad()
{
    m_bg = getLoop()->getResourceManager().lock()->getTexture(m_backgroundTextureName).get();
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void A13GUIAbstractBuilder<_PartType, SX, SY>::BgWidget::onResize(sf::Event::SizeEvent& event)
{
    setSize({(double)event.width, (double)event.height});
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void A13GUIAbstractBuilder<_PartType, SX, SY>::BgWidget::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    EGE::Renderer renderer(target);
    renderer.renderTexturedRectangle(0, 0, getSize().x, getSize().y, *m_bg);
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
TileMapObject<_PartType, SX, SY>::TileMapObject(EGE::SharedPtr<EGE::Scene> scene, std::string tex)
: EGE::SceneObject2D(scene, "a13:builder:tilemap"), m_builderPartAtlasTextureName(tex)
{
    m_tilemap = make<EGE::FixedTileMap2D<ObjectState<_PartType>, SX, SY>>();

    // Initialize tilemap
    m_tilemap->setTileSize({16, 16});
    m_tilemap->initialize({});

    // Set renderer
    auto renderer = make<EGE::TilemapRenderer2D<EGE::FixedTileMap2D<ObjectState<_PartType>, SX, SY>>>(scene, m_tilemap);
    renderer->setAtlasTextureName(m_builderPartAtlasTextureName);
    renderer->setTileAtlasMapper([](const ObjectState<_PartType>& state)->EGE::Vec2d {
                                    if(!state.obj)
                                        return EGE::Vec2d(0, 0);
                                    EGE::Vec2d atlasPos = state.obj->getAtlasPosition();
                                    return {atlasPos.x * 16 + state.cornerPos.x * 16, atlasPos.y * 16 + state.cornerPos.y * 16};
                                 });
    setRenderer(renderer);

    setPosition({0, 0});
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
bool TileMapObject<_PartType, SX, SY>::canPartBePlacedHere(sf::Vector2f pos) const
{
    if(!m_currentPart)
        return false;

    EGE::Vec2u partSize = m_currentPart->getSize();
    sf::Vector2f relPos = pos - getPosition();
    EGE::Vec2u tileRel = m_tilemap->getTileAlignedPos({(relPos.x), (relPos.y)});

    // Check if objects can be placed here
    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = m_tilemap->getTile({x + tileRel.x, y + tileRel.y});
        if(!tile)
            return false; // Out of bounds!
        if(tile->obj)
            return false; // Something is already here :(
    }
    return true;
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void TileMapObject<_PartType, SX, SY>::placePart(sf::Vector2f pos)
{
    if(!m_currentPart)
        return;

    // Calculate tile position
    sf::Vector2f relPos = pos - getPosition();
    EGE::Vec2u partSize = m_currentPart->getSize();
    EGE::Vec2u tileRel = m_tilemap->getTileAlignedPos({(relPos.x), (relPos.y)});

    // Check if objects can be placed here
    if(!canPartBePlacedHere(pos))
        return;

    auto it = m_objects.insert(std::make_pair(EGE::Vec2i(tileRel.x, tileRel.y), m_currentPart));

    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = m_tilemap->getTile({x + tileRel.x, y + tileRel.y});
        tile->obj = it.first->second;
        tile->cornerPos = {x, y};
    }
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void TileMapObject<_PartType, SX, SY>::removePart(sf::Vector2f pos)
{
    // Calculate tile position
    sf::Vector2f relPos = pos - getPosition();
    EGE::Vec2u tileRel = m_tilemap->getTileAlignedPos({(relPos.x), (relPos.y)});

    // Check which object is here
    auto tile = m_tilemap->getTile({tileRel.x, tileRel.y});
    if(!tile)
        return; // Out of bounds!

    EGE::FlatPtr<A13RocketPart> part = tile->obj;
    if(!part)
        return; // Nothing is here!

    // Calculate left-top coordinate
    EGE::Vec2u objectPos = tileRel - tile->cornerPos;

    // Get object iterator
    auto it = m_objects.find(objectPos);
    if(it == m_objects.end())
    {
        err() << "Can't find object at " << objectPos.x << "," << objectPos.y;
        return;
    }

    EGE::Vec2u partSize = tile->obj->getSize();

    // Remove that object
    m_objects.erase(it);

    // Clear tilemap
    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = m_tilemap->getTile({x + objectPos.x, y + objectPos.y});
        tile->obj = nullptr;
        tile->cornerPos = {0, 0};
    }
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void TileMapObject<_PartType, SX, SY>::render(sf::RenderTarget& target, const EGE::RenderStates& states) const
{
    EGE::Renderer renderer(target);

    // Tilemap
    EGE::SceneObject::render(target, states);

    // Highlight
    sf::Color bc = canPartBePlacedHere(highlightPos) ? sf::Color::Green : sf::Color::Red;

    if(getPartSize().x != 0 && highlightPos.x >= 0 && highlightPos.y >= 0)
    {
        sf::RectangleShape rs(sf::Vector2f(getPartSize().x * 16, getPartSize().y * 16));
        rs.setPosition(highlightPos.x, highlightPos.y);
        rs.setFillColor(sf::Color(bc.r, bc.g, bc.b, 64));
        rs.setOutlineColor(sf::Color(bc.r, bc.g, bc.b, 128));
        rs.setOutlineThickness(1.f);
        target.draw(rs);
    }

    /*
    std::string debugInfo = "Debug Info\n";
    for(auto obj: m_objects)
        debugInfo += std::to_string(obj.first.x) + "," + std::to_string(obj.first.y) + ": " + obj.second->getId() + "\n";

    renderer.renderText(0, 0, *getOwner()->getLoop()->getResourceManager().lock()->getDefaultFont().get(), debugInfo, 8);
    */
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void TileMapObject<_PartType, SX, SY>::setTile(EGE::Vec2i offset)
{
    highlightPos = sf::Vector2f((offset.x) * 16, (offset.y) * 16) + getPosition();
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void A13GUIAbstractBuilder<_PartType, SX, SY>::onLoad()
{
    EGE::GUIScreen::onLoad();

    // Allocate scene.
    m_scene = make<EGE::Scene2D>(getLoop());

    // Add tilemap to scene.
    m_tileMapObject = make<TileMapObject<_PartType, SX, SY>>(m_scene, m_builderAtlasTN);
    m_scene->addObject(m_tileMapObject);

    // Add camera
    m_camera = make<EGE::CameraObject2D>(m_scene);
    m_camera->setScalingMode(EGE::ScalingMode::Centered);
    m_camera->setZoom(4.f);
    m_zoom = 4.f;
    m_scene->addObject(m_camera);
    m_scene->setCamera(m_camera);

    // Add background
    auto bg = make<A13GUIAbstractBuilder::BgWidget>(this);
    bg->m_backgroundTextureName = m_backgroundTN;
    addWidget(bg);

    // Add SceneWidget.
    addWidget(make<EGE::SceneWidget>(this, m_scene));

    // Add left panel
    m_partSelector = make<PartSelectWidget<_PartType, SX, SY>>(this, m_tileMapObject.get());
    m_partSelector->m_partAtlasTextureName = m_selectorAtlasTN;
    addWidget(m_partSelector);
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void A13GUIAbstractBuilder<_PartType, SX, SY>::onMouseButtonPress(sf::Event::MouseButtonEvent& event)
{
    EGE::GUIScreen::onMouseButtonPress(event);
    if(event.button == sf::Mouse::Left)
    {
        m_dragStartPos = sf::Vector2i(event.x, event.y);
        m_dragStartCamPos = m_camera->getEyePosition();
        m_mousePressed = true;
    }
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void A13GUIAbstractBuilder<_PartType, SX, SY>::onResize(sf::Event::SizeEvent& event)
{
    EGE::GUIScreen::onResize(event);
    m_partSelector->setSize({80, (double)event.height});
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void A13GUIAbstractBuilder<_PartType, SX, SY>::onMouseButtonRelease(sf::Event::MouseButtonEvent& event)
{
    EGE::GUIScreen::onMouseButtonRelease(event);
    auto& wnd = *getWindow().lock().get();

    if(event.button == sf::Mouse::Left)
    {
        m_mousePressed = false;
        if(m_dragging)
            m_dragging = false;
        else
        {
            if(event.x < m_partSelector->getSize().x)
                return;
            m_tileMapObject->placePart(m_scene->mapScreenToScene(wnd, sf::Vector2i(event.x, event.y), getView(wnd)));
        }
    }
    else if(event.button == sf::Mouse::Right)
    {
        if(event.x < m_partSelector->getSize().x)
            return;
        m_tileMapObject->removePart(m_scene->mapScreenToScene(wnd, sf::Vector2i(event.x, event.y), getView(wnd)));
    }
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void A13GUIAbstractBuilder<_PartType, SX, SY>::onMouseMove(sf::Event::MouseMoveEvent& event)
{
    EGE::GUIScreen::onMouseMove(event);

    sf::Vector2f currentPos(event.x, event.y);
    auto diff = (currentPos - sf::Vector2f(m_dragStartPos));

    if(!m_dragging && m_mousePressed && EGE::VectorOperations::lengthSquared(EGE::Vec2d(diff.x, diff.y)) > 400)
    {
        m_dragging = true;
    }

    // Update highlight.
    auto& wnd = *getWindow().lock().get();
    sf::Vector2f mouseScenePos = m_scene->mapScreenToScene(wnd, sf::Vector2i(event.x, event.y), getView(wnd)) - m_tileMapObject->getPosition();
    EGE::Vec2i tileRel = m_tileMapObject->m_tilemap->getTileAlignedPos({mouseScenePos.x, mouseScenePos.y});
    m_tileMapObject->setTile(tileRel);

    if(m_dragging)
    {
        // Move camera.
        auto vec = m_dragStartCamPos - diff / m_zoom;
        m_camera->setEyePosition(vec);
    }
}

template<class _PartType, EGE::Size SX, EGE::Size SY>
void A13GUIAbstractBuilder<_PartType, SX, SY>::onMouseWheelScroll(sf::Event::MouseWheelScrollEvent& event)
{
    EGE::GUIScreen::onMouseWheelScroll(event);

    if(event.delta > 0)
    {
        m_zoom *= 1.1;
    }
    else
    {
        m_zoom /= 1.1;
    }
    m_camera->setZoom(m_zoom);
}
