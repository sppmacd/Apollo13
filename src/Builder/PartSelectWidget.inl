// INLINE

template<class _Tilemap, class _Item>
void PartSelectWidget<_Tilemap, _Item>::onLoad()
{
    ASSERT(m_gpo);
    for(auto& part : *m_gpo)
    {
        m_items.push_back(part.second.get());
    }
    m_font = getLoop()->getResourceManager().lock()->getDefaultFont().get();
    ASSERT(m_font);
}

template<class _Tilemap, class _Item>
void PartSelectWidget<_Tilemap, _Item>::onMouseButtonRelease(sf::Event::MouseButtonEvent& event)
{
    if(m_leftClicked)
    {
        if(event.x && event.x < getSize().x)
        {
            // Clamp scroll position
            double ymax = -getSize().y + getSize().x * m_items.size();
            double scrollPos = std::max(0.0, std::min(m_scrollPos, ymax));

            size_t index = (event.y + (int)scrollPos) / getSize().x;
            if(index >= 0 && index < m_items.size())
            {
                setCurrentItemIndex(index);
            }
        }
    }
}

template<class _Tilemap, class _Item>
void PartSelectWidget<_Tilemap, _Item>::onMouseWheelScroll(sf::Event::MouseWheelScrollEvent& event)
{
    if(event.x < getSize().x)
    {
        m_scrollPos -= event.delta * getSize().x / 2;

        // Clamp scroll position
        double ymax = -getSize().y + getSize().x * m_items.size();
        m_scrollPos = std::max(0.0, std::min(m_scrollPos, ymax));
    }
}

template<class _Tilemap, class _Item>
void PartSelectWidget<_Tilemap, _Item>::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    if(!m_atlas)
        m_atlas = getLoop()->getResourceManager().lock()->getTexture(m_partAtlasTextureName).get();

    // Clamp scroll position
    double ymax = -getSize().y + getSize().x * m_items.size();
    double scrollPos = std::max(0.0, std::min(m_scrollPos, ymax));

    // Background
    EGE::Renderer renderer(target);
    renderer.setStates(states);
    renderer.renderRectangle(0, 0, getSize().x, getSize().y, sf::Color(0, 0, 0, 127));

    std::string tooltip;

    // Highlight and tooltip (if rendering on window)
    sf::RenderWindow* wnd = (sf::RenderWindow*)&target;
    sf::Vector2i mousePos;
    if(wnd)
    {
        mousePos = sf::Mouse::getPosition(*wnd);
        if(mousePos.x < getSize().x && mousePos.x > 0)
        {
            size_t index = (mousePos.y + (int)scrollPos) / (getSize().x);
            if(index >= 0 && index < m_items.size())
            {
                // Highlight
                renderer.renderRectangle(0, (getSize().x) * index - scrollPos, getSize().x, getSize().x, sf::Color(255, 255, 255, 127));

                // Tooltip
                tooltip = m_items[index]->getTooltip();

                m_parent->setViewForWidget(target);

                // Custom info
                if(m_customRenderer && !(sf::Mouse::isButtonPressed(sf::Mouse::Left) ^ m_leftClicked))
                {
                    m_customRenderer(target, m_items[index]);
                }

                setViewForWidget(target);
            }
        }
    }

    // Permanent highlight (For active object)
    size_t index = m_index;
    renderer.renderRectangle(0, (getSize().x) * index - scrollPos, getSize().x, getSize().x, sf::Color(0, 255, 0, 127));

    // Objects
    size_t c = 0;
    for(auto item: m_items)
    {
        sf::IntRect texRect;
        // TODO: Name that '64' somehow
        auto ap = item->getAtlasPosition(0);
        texRect.left = ap.x * 64;
        texRect.top = ap.y * 64;
        texRect.width = 64;
        texRect.height = 64;
        renderer.renderTexturedRectangle(10, (getSize().x) * c + 10 - scrollPos, getSize().x - 20, getSize().x - 20, *m_atlas, texRect);

        c++;
    }

    /*if(wnd && !tooltip.empty())
    {
        // TODO: EGE::Widget::enterGlobalView()
        m_parent->setViewForWidget(target);

        // Tooltip
        EGE::Renderer::TextWithBackgroundSettings settings;
        settings.background_color = sf::Color(0, 0, 0, 127);
        settings.font_size = 12;
        renderer.renderTextWithBackground(mousePos.x + 10, mousePos.y - 10, *m_font, tooltip, settings);

        // we don't really need it
        setViewForWidget(target);
    }*/
}
