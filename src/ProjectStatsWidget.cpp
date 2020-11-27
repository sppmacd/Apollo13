#include "ProjectStatsWidget.h"

#include "Apollo13.h"

#include <ege/gui/GUIGameLoop.h>
#include <ege/gfx/Renderer.h>

namespace A13
{

void ProjectStatsWidget::onLoad()
{
    auto resMan = getLoop()->getResourceManager().lock();
    m_atlas = resMan->getTexture("game/resource_items.png").get();
    m_font = resMan->getDefaultFont().get();
}

void ProjectStatsWidget::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    ASSERT(m_atlas);
    ASSERT(m_font);

    EGE::Renderer renderer(target);

    // Background
    renderer.renderRectangle(0, 0, getSize().x, getSize().y, sf::Color(0, 0, 0, 127));

    // TODO: Status
    std::string status;
    int ptime = Apollo13::instance().save.projectTilemap()->getCurrentProjectTime();
    int ttime = Apollo13::instance().save.projectTilemap()->getTotalProjectTime();
    if(ptime == -2)
        status = "NOT STARTED";
    else if(ptime == -1)
        status = "RESOURCES REQUESTED";
    else if(ptime < ttime)
        status = "WORKING (" + std::to_string(int(double(ptime) / ttime * 100)) + "%)";
    else
        status = "FINISHED";

    renderer.renderText(10, 10, *m_font, "STATUS: " + status, 20);

    // Missing items
    Inventory missing = Apollo13::instance().save.projectTilemap()->getMissingItems();
    EGE::Size s = 0;
    for(auto& stack: missing)
    {
        if(stack.first.empty() || !stack.second)
            continue;

        std::string str = std::to_string(stack.second);

        const unsigned WIDTH = 42;

        // Icon
        sf::IntRect texRect;
        // TODO: Name that '16' somehow
        auto item = A13GameplayObjectManager::instance().resourceItems.findById(stack.first);
        texRect.left = item->getAtlasPosition().x * 16;
        texRect.top = item->getAtlasPosition().y * 16;
        texRect.width = 16;
        texRect.height = 16;
        renderer.renderTexturedRectangle(10 + s * WIDTH, 40, 32, 32, *m_atlas, texRect);

        s++;
    }

    // TODO: Progress

}

}
