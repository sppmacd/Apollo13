#include "ResourceStatsWidget.h"

#include "PlayerStats.h"

void ResourceStatsWidget::onLoad()
{
    auto& gpo = A13GameplayObjectManager::instance().resourceItems;
    for(auto& g: gpo)
    {
        m_items[g.second->getNumericId()] = g.second.get();
    }

    auto resMan = getLoop()->getResourceManager().lock();
    m_atlas = resMan->getTexture("game/resource_items.png").get();
    m_font = resMan->getDefaultFont().get();
}

void ResourceStatsWidget::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    EGE::Renderer renderer(target);

    // Background
    renderer.renderRectangle(0, 0, getSize().x, getSize().y, sf::Color(0, 0, 0, 128));

    // Items
    ASSERT(m_atlas);
    ASSERT(m_font);

    EGE::Size s = 0;
    for(auto pr: m_items)
    {
        // Texture
        const int ITEM_SIZE = 48;
        float rpos = (getSize().x - ITEM_SIZE) / 2;
        auto ac = pr.second->getAtlasPosition();
        renderer.renderTexturedRectangle(rpos - 10, s * ITEM_SIZE + rpos, ITEM_SIZE, ITEM_SIZE,
                                         *m_atlas, {ac.x * 16, ac.y * 16, 16, 16});

        // Count
        int count = PlayerStats::instance().resourceItems[pr.first];
        renderer.renderText(rpos + ITEM_SIZE - 10, s * ITEM_SIZE + rpos + ITEM_SIZE / 2 - 6, *m_font, std::to_string(count), 16);

        s++;
    }
}
