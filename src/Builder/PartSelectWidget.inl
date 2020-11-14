// INLINE

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
