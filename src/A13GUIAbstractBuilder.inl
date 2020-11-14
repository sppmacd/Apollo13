// INLINE

#include <ege/debug/Logger.h>
#include <ege/scene/SceneWidget.h>
#include <ege/util/Random.h>

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
    m_tileMapObject->setCanPlaceHere([this](EGE::Vec2i pos) {
                                        return canPlaceHere(pos);
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
CanPlaceHere A13GUIAbstractBuilder<_Tilemap, _Item>::canPlaceHere(EGE::Vec2i tileRel)
{
    auto tile = (m_tileMapObject->m_tilemap->useEnsure() ? &m_tileMapObject->m_tilemap->ensureTile(tileRel) : m_tileMapObject->m_tilemap->getTile(tileRel));
    if(!tile)
        return CanPlaceHere::NotLoaded;
    else
        return canPlaceHere(tileRel, *tile, m_partSelector->getCurrentItem());
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::onMouseButtonPress(sf::Event::MouseButtonEvent& event)
{
    EGE::GUIScreen::onMouseButtonPress(event);

    if(m_dialog)
        return;

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

    if(m_dialog)
        return;

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

    if(m_dialog)
        return;

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

    if(m_dialog)
        return;

    if(event.delta > 0)
    {
        if(m_zoom < 16.0)
            m_zoom *= 1.1;
    }
    else
    {
        if(m_zoom > 0.87)
            m_zoom /= 1.1;
    }
    m_camera->setZoom(m_zoom);
    log() << m_zoom;
}

template<class _Tilemap, class _Item>
void A13GUIAbstractBuilder<_Tilemap, _Item>::onKeyPress(sf::Event::KeyEvent& event)
{
    EGE::GUIScreen::onKeyPress(event);

    if(event.code == sf::Keyboard::Escape)
    {
        m_partSelector->setCurrentItemIndex(-1);
        m_tileMapObject->setHighlight({0, 0}, {0, 0});
    }
}
