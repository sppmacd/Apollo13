#include "ResourceStatsWidget.h"

#include "PlayerStats.h"

#include <sstream>
#include <iomanip>

void ResourceStatsWidget::onLoad()
{
    auto& gpo = A13GameplayObjectManager::instance().resourceItems;
    for(auto& g: gpo)
    {
        m_items[g.second->getId()] = g.second.get();
    }

    auto resMan = getLoop()->getResourceManager().lock();
    m_atlas = resMan->getTexture("game/resource_items.png").get();
    m_font = resMan->getDefaultFont().get();
}

std::string toStringPrefixed(int n)
{
    std::ostringstream oss;
    if(n > 1000000000)
        oss << std::fixed << std::setprecision(2) << n / 1000000000.0 << "G";
    else if(n > 100000000)
        oss << std::fixed << std::setprecision(0) << n / 1000000.0 << "M";
    else if(n > 10000000)
        oss << std::fixed << std::setprecision(1) << n / 1000000.0 << "M";
    else if(n > 1000000)
        oss << std::fixed << std::setprecision(2) << n / 1000000.0 << "M";
    else if(n > 100000)
        oss << std::fixed << std::setprecision(0) << n / 1000.0 << "k";
    else if(n > 10000)
        oss << std::fixed << std::setprecision(1) << n / 1000.0 << "k";
    else if(n > 1000)
        oss << std::fixed << std::setprecision(2) << n / 1000.0 << "k";
    else
        oss << n;

    return oss.str();
}

void ResourceStatsWidget::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    EGE::Renderer renderer(target);

    // Background
    renderer.renderRectangle(0, 0, getSize().x, getSize().y, sf::Color(0, 0, 0, 128));

    // Items
    ASSERT(m_atlas);
    ASSERT(m_font);

    sf::RenderWindow* wnd = (sf::RenderWindow*)&target;

    EGE::Size s = 0;
    for(auto pr: m_items)
    {
        // Count
        int count = m_inventory[pr.first];

        if(count == 0)
            continue;

        const int ITEM_SIZE = 48;
        float rpos = (getSize().x - ITEM_SIZE) / 2;

        renderer.renderText(rpos + ITEM_SIZE - 20, s * ITEM_SIZE + rpos + ITEM_SIZE / 2 - 6, *m_font, toStringPrefixed(count), 14);

        // Texture
        auto ac = pr.second->getAtlasPosition();
        renderer.renderTexturedRectangle(rpos - 20, s * ITEM_SIZE + rpos, ITEM_SIZE, ITEM_SIZE,
                                         *m_atlas, {ac.x * 16, ac.y * 16, 16, 16});

        // Tooltip (if applicable)

        if(wnd)
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*wnd);
            if(mousePos.x > getPosition().x && mousePos.x < getPosition().x + getSize().x &&
               mousePos.y > s * ITEM_SIZE + rpos && mousePos.y < s * ITEM_SIZE + rpos + ITEM_SIZE)
            {
                std::string tooltip = pr.second->getId();

                // TODO: EGE::Widget::enterGlobalView()
                m_parent->setViewForWidget(target);

                // Tooltip
                EGE::Renderer::TextWithBackgroundSettings settings;
                settings.background_color = sf::Color(0, 0, 0, 127);
                settings.font_size = 12;
                settings.text_align = EGE::Renderer::TextAlign::Right;
                renderer.renderTextWithBackground(mousePos.x - 10, mousePos.y - 10, *m_font, tooltip, settings);

                // we don't really need it
                setViewForWidget(target);
            }
        }

        s++;
    }
}
