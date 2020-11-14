// INLINE

#include <ege/debug/Logger.h>
#include <ege/scene/SceneWidget.h>
#include <ege/util/Random.h>

template<class _Tilemap, class _Item>
void PartSelectWidget<_Tilemap, _Item>::onLoad()
{
    ASSERT(m_gpo);
    for(auto& part : *m_gpo)
    {
        m_items.push_back(part.second);
    }
}

template<class _Tilemap, class _Item>
void PartSelectWidget<_Tilemap, _Item>::onMouseButtonRelease(sf::Event::MouseButtonEvent& event)
{
    if(m_leftClicked)
    {
        if(event.x > 10 && event.x < getSize().x - 10)
        {
            size_t index = (event.y) / getSize().x;
            if(index >= 0 && index < m_items.size())
            {
                setCurrentItemIndex(index);
            }
        }
    }
}

template<class _Tilemap, class _Item>
void PartSelectWidget<_Tilemap, _Item>::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
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
            if(index >= 0 && index < m_items.size())
                renderer.renderRectangle(0, getSize().x * index, getSize().x, getSize().x, sf::Color(0, 0, 0, 127));
        }
    }

    // Permanent highlight (For active object)
    size_t index = m_index;
    renderer.renderRectangle(0, getSize().x * index, getSize().x, getSize().x, sf::Color(0, 255, 0, 127));

    // Objects
    size_t c = 0;
    for(auto item: m_items)
    {
        sf::IntRect texRect;
        // TODO: Name that '64' somehow
        texRect.left = item->getAtlasPosition().x * 64;
        texRect.top = item->getAtlasPosition().y * 64;
        texRect.width = 64;
        texRect.height = 64;
        renderer.renderTexturedRectangle(10, getSize().x * c + 10, getSize().x - 20, getSize().x - 20, *m_atlas, texRect);
        c++;
    }
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::BgWidget::onLoad()
{
    m_bg = getLoop()->getResourceManager().lock()->getTexture(m_backgroundTextureName).get();
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::BgWidget::onResize(sf::Event::SizeEvent& event)
{
    setSize({(double)event.width, (double)event.height});
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::BgWidget::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    EGE::Renderer renderer(target);
    renderer.renderTexturedRectangle(0, 0, getSize().x, getSize().y, *m_bg);
}

template<class _Tilemap>
class AtlasMapper
{
public:
    AtlasMapper(TileMapObject<_Tilemap>* tmObj)
    : m_tileMapObj(tmObj) {}

    EGE::Vec2d operator()(const typename _Tilemap::TileType& state, EGE::Size layer)
    {
        switch(layer)
        {
        case _Tilemap::TileType::AdditionalLayerCount:
            {
                if(!state.obj)
                    return EGE::Vec2d(0, 0);

                EGE::Vec2d atlasPos = state.obj->getAtlasPosition();
                auto tileSize = m_tileMapObj->m_tilemap->getTileSize();
                return {atlasPos.x * tileSize.x + state.cornerPos.x * tileSize.x,
                        atlasPos.y * tileSize.y + state.cornerPos.y * tileSize.y};
            } break;
        default:
            return m_tileMapObj->callCustomLayerAtlasMapper(state, layer);
        }
    }

private:
    TileMapObject<_Tilemap>* m_tileMapObj;
};

template<class _Tilemap>
TileMapObject<_Tilemap>::TileMapObject(EGE::SharedPtr<EGE::Scene> scene, std::array<std::string, _Tilemap::TileType::AdditionalLayerCount + 1>& texs,  EGE::SharedPtr<_Tilemap> tilemap)
: EGE::SceneObject2D(scene, "a13:builder:tilemap"), m_builderPartAtlasTextureNames(texs), m_tilemap(tilemap)
{
    // Set tilemap
    m_tilemap = tilemap;

    // Set renderer
    auto renderer = make<EGE::TilemapRenderer2D<_Tilemap>>(scene, m_tilemap);

    renderer->setLayerCount(_Tilemap::TileType::AdditionalLayerCount + 1);
    for(EGE::Size s = 0; s < texs.size(); s++)
        renderer->setAtlasTextureName(m_builderPartAtlasTextureNames[s], s);

    renderer->setTileAtlasMapper(AtlasMapper<_Tilemap>(this));
    setRenderer(renderer);

    setPosition({0, 0});
}

template<class _Tilemap>
bool TileMapObject<_Tilemap>::isObjectInArea(EGE::Vec2i position, EGE::Vec2u range) const
{
    for(EGE::Uint32 x = 0; x < range.x; x++)
    for(EGE::Uint32 y = 0; y < range.y; y++)
    {
        auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({x + position.x, y + position.y}) : m_tilemap->getTile({x + position.x, y + position.y}));
        if(!tile)
            return true; // Out of bounds!
        if(tile->obj)
            return true; // Something is already here :(
    }
    return false;
}

template<class _Tilemap>
EGE::Vec2i TileMapObject<_Tilemap>::mapTilePosition(sf::Vector2f scenePos) const
{
    sf::Vector2f relPos = scenePos - getPosition();
    EGE::Vec2i tileRel = m_tilemap->getTileAlignedPos({(relPos.x), (relPos.y)});
    return tileRel;
}

template<class _Tilemap>
bool TileMapObject<_Tilemap>::canPartBePlacedHere(EGE::Vec2i tileRel, EGE::Vec2u partSize) const
{
    // Check if objects can be placed here
    if(m_placePredicate)
        return m_placePredicate(m_tilemap.get(), tileRel);

    // Default handler - just check if there are no objects here
    return !isObjectInArea(tileRel, partSize);
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::placePart(EGE::Vec2i tileRel, EGE::SharedPtr<typename _Tilemap::TileType::PartType> part)
{
    EGE::Vec2u partSize = part->getSize();

    // Check if objects can be placed here
    if(!canPartBePlacedHere(tileRel, partSize))
        return;

    auto it = m_objects.insert(std::make_pair(EGE::Vec2i(tileRel.x, tileRel.y), part));

    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({x + tileRel.x, y + tileRel.y}) : m_tilemap->getTile({x + tileRel.x, y + tileRel.y}));

        // Sanity check
        ASSERT(tile);

        tile->obj = it.first->second.get();
        tile->cornerPos = {x, y};
    }
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::removePart(EGE::Vec2i tileRel)
{
    // Check which object is here
    auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({tileRel.x, tileRel.y}) : m_tilemap->getTile({tileRel.x, tileRel.y}));
    if(!tile)
        return; // Out of bounds!

    EGE::FlatPtr<typename _Tilemap::TileType::PartType> part = tile->obj;
    if(!part)
        return; // Nothing is here!

    // Calculate left-top coordinate
    EGE::Vec2i objectPos = tileRel - EGE::Vec2i(tile->cornerPos);

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
        auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({x + objectPos.x, y + objectPos.y}) : m_tilemap->getTile({x + objectPos.x, y + objectPos.y}));
        if(!tile)
            return;

        tile->obj = nullptr;
        tile->cornerPos = {0, 0};
    }
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::onActivate(EGE::Vec2i pos)
{
    auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({pos.x, pos.y}) : m_tilemap->getTile({pos.x, pos.y}));
    if(tile)
        m_tilemap->onActivate(pos, *tile);
}

template<class _Tilemap>
std::string TileMapObject<_Tilemap>::getTooltip(EGE::Vec2i pos)
{
    auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({pos.x, pos.y}) : m_tilemap->getTile({pos.x, pos.y}));
    return tile ? m_tilemap->getTooltip(pos, *tile) : "Not loaded";
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::render(sf::RenderTarget& target, const EGE::RenderStates& states) const
{
    EGE::Renderer renderer(target);

    // Tilemap
    EGE::SceneObject::render(target, states);

    // Highlight

    // TODO: render separate rectangle for each tile
    sf::Color bc = canPartBePlacedHere(m_highlightPos, m_highlightSize) ? sf::Color::Green : sf::Color::Red;

    if(m_highlightSize.x != 0)
    {
        sf::RectangleShape rs(sf::Vector2f(m_highlightSize.x * 16, m_highlightSize.y * 16));
        rs.setPosition(m_highlightPosScreen.x, m_highlightPosScreen.y);
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

template<class _Tilemap>
void TileMapObject<_Tilemap>::setHighlight(EGE::Vec2i offset, EGE::Vec2u size)
{
    m_highlightPosScreen = sf::Vector2f((offset.x) * 16, (offset.y) * 16) + getPosition();
    m_highlightPos = offset;
    m_highlightSize = size;
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::onLoad()
{
    EGE::GUIScreen::onLoad();

    // Allocate scene.
    m_scene = make<EGE::Scene2D>(getLoop());

    // Add tilemap to scene.
    ASSERT(m_tilemap);
    m_tileMapObject = make<TileMapObject<_Tilemap>>(m_scene, m_builderAtlasTNs, m_tilemap);
    m_tileMapObject->setUseEnsure(m_useEnsure);
    m_tileMapObject->setAtlasMapper([this](_Tilemap* tilemap, const typename _Tilemap::TileType& tile, EGE::Size layer) {
                                        return atlasMapper(tilemap, tile, layer);
                                    });
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
    m_partSelector = make<PartSelectWidget<_Tilemap, _Item>>(this, m_tileMapObject.get());
    m_partSelector->m_partAtlasTextureName = m_selectorAtlasTN;
    m_partSelector->m_gpo = m_gpo;
    addWidget(m_partSelector);

    // Add tooltip label
    m_toolTipLabel = make<EGE::Label>(this);
    m_toolTipLabel->setPosition({90, 10});
    addWidget(m_toolTipLabel);
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::onMouseButtonPress(sf::Event::MouseButtonEvent& event)
{
    EGE::GUIScreen::onMouseButtonPress(event);
    if(event.button == sf::Mouse::Left)
    {
        m_dragStartPos = sf::Vector2i(event.x, event.y);
        m_dragStartCamPos = m_camera->getEyePosition();
        m_mousePressed = true;
    }
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::onResize(sf::Event::SizeEvent& event)
{
    EGE::GUIScreen::onResize(event);
    m_partSelector->setSize({80, (double)event.height});
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::onMouseButtonRelease(sf::Event::MouseButtonEvent& event)
{
    EGE::GUIScreen::onMouseButtonRelease(event);

    if(event.button == sf::Mouse::Left)
    {
        m_mousePressed = false;
        if(m_dragging)
            m_dragging = false;
        else
        {
            if(event.x < m_partSelector->getSize().x)
                return;
            auto tilePos = m_tileMapObject->mapTilePosition(screenToScene(sf::Vector2i(event.x, event.y)));

            if(m_partSelector->getCurrentItem())
            {
                if(!m_partSelector->getCurrentItem()->onPlace(m_tileMapObject->m_tilemap.get(), tilePos))
                    m_tileMapObject->placePart(tilePos, m_partSelector->getCurrentItem()->getPart());
            }
            else
            {
                // TODO: Try to "activate" currently hovered tile
                // TODO: BuilderPart::contextMenu() - right click
                // TODO: BuilderPart::tooltip() - when hovering instead of highlight
                m_tileMapObject->onActivate(tilePos);
            }
        }
    }
    else if(event.button == sf::Mouse::Right)
    {
        if(event.x < m_partSelector->getSize().x)
            return;
        auto tilePos = m_tileMapObject->mapTilePosition(screenToScene(sf::Vector2i(event.x, event.y)));
        m_tileMapObject->removePart(tilePos);
    }
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::onMouseMove(sf::Event::MouseMoveEvent& event)
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
    if(m_partSelector->getCurrentItem())
    {
        EGE::Vec2i tileRel = m_tileMapObject->m_tilemap->getTileAlignedPos({mouseScenePos.x, mouseScenePos.y});
        m_tileMapObject->setHighlight(tileRel, m_partSelector->getCurrentItem()->getPart()->getSize());
    }

    // Get tooltip and set it to current.
    EGE::Vec2i tileRel = m_tileMapObject->m_tilemap->getTileAlignedPos({mouseScenePos.x, mouseScenePos.y});
    m_tooltip = m_tileMapObject->getTooltip(tileRel);
    log() << "tooltip = " << m_tooltip;
    m_toolTipLabel->setString(m_tooltip);

    if(m_dragging)
    {
        // Move camera.
        auto vec = m_dragStartCamPos - diff / m_zoom;
        m_camera->setEyePosition(vec);
    }
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::onMouseWheelScroll(sf::Event::MouseWheelScrollEvent& event)
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
    log() << m_zoom;
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::onKeyPress(sf::Event::KeyEvent& event)
{
    if(event.code == sf::Keyboard::Escape)
    {
        m_partSelector->setCurrentItemIndex(-1);
        m_tileMapObject->setHighlight({0, 0}, {0, 0});
    }
}
