#include "ProjectStatsWidget.h"

#include "Apollo13.h"

#include <ege/gui/GUIGameLoop.h>
#include <ege/gfx/Renderer.h>

#include <iomanip>
#include <sstream>

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

    if(!Apollo13::instance().save.loaded())
        return;

    // Get data
    auto ptm = Apollo13::instance().save.projectTilemap();
    int ptime = ptm->getCurrentProjectTime();
    int ttime = ptm->getTotalProjectTime();

    if(ptime == -2)
        return;

    EGE::Renderer renderer(target);

    // Background
    renderer.renderRectangle(0, 0, getSize().x, getSize().y, sf::Color(0, 0, 0, 127));

    // Status
    std::string status;

    if(ptime == -1)
        status += "RESOURCES REQUESTED";
    else if(ptime == -3)
        status += "ROCKET LAUNCHED";
    else if(ptime < ttime)
        status += "BUILDING ROCKET... (" + std::to_string(int(double(ptime) / ttime * 100)) + "%)";
    else
        status += "FINISHED (Click Start Platform)";

    renderer.renderText(15, 10, *m_font, "MISSION: " + status, 20);

    if(ptime == -1)
    {
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
            renderer.renderTexturedRectangle(10 + s * WIDTH, 60, 32, 32, *m_atlas, texRect);

            s++;
        }
    }

    // Info
    std::ostringstream oss;
    if(ptime == -3)
    {
        oss << std::fixed << std::setprecision(2);
        oss << "Fuel: " << ptm->m_rocketFuel << " / " << ptm->m_fuelTotal << " kg (" << ptm->m_fuelUsage << " kg/t)";
        oss << " Thrust: " << ptm->m_thrust << " / " << ptm->m_maxThrust << " N\n";
        oss << "Speed: " << ptm->m_rocketSpeed << " m/s";
        oss << " Height: " << ptm->m_rocketHeight << " m (max: " << ptm->m_rocketMaxHeight << " m)\n";
        oss << "Total mass: " << ptm->m_mass + ptm->m_rocketFuel << " kg (" << ptm->m_mass << " kg without fuel)\n";
    }
    else if(ptime == -1)
    {
        oss << "Missing some resources. ";
    }
    oss << "Press P to cancel mission.";
    renderer.renderText(15, 40, *m_font, oss.str(), 10);

    // Progress
    if(ptime == -3)
        ptime = ttime;

    renderer.renderRectangle(15, 95, getSize().x - 30, 10, sf::Color(127, 0, 0));
    renderer.renderRectangle(15, 95, (getSize().x - 30) * ((double)ptime / ttime), 10, sf::Color(0, 127, 0));
}

}
