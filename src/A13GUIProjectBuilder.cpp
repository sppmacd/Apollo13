#include "A13GUIProjectBuilder.h"

#include "A13GameplayObjectManager.h"
#include "Apollo13.h"

A13GUIProjectBuilder::A13GUIProjectBuilder(EGE::GUIScreen* loop)
: A13GUIAbstractBuilder(loop, Apollo13::instance().save.projectTilemap())
{
    setBuilderBackground("gui/pb/background.png");
    setBuilderAtlas("gui/pb/rocket_parts.png");
    setSelectorAtlas("gui/pb/items.png");
    setGPO(&A13GameplayObjectManager::instance().rocketPartItems);
    m_tilemap->setTileSize(EGE::Vec2u(16, 16));
}

void A13GUIProjectBuilder::onKeyPress(sf::Event::KeyEvent& event)
{
    if(event.code == sf::Keyboard::Escape && !m_partSelector->getCurrentItem())
    {
        if(checkValidity())
            exitDialog(0);
        else
            Apollo13::instance().messageBox("The rocket must have:\n* exactly 1 capsule\n* at least 4 fuel units\n* at least 1 engine", MBID_DONT_CARE);
    }

    A13GUIAbstractBuilder::onKeyPress(event);
}

void A13GUIProjectBuilder::onLoad()
{
    A13GUIAbstractBuilder::onLoad();

    // Custom part selector renderer (for description)
    m_partSelector->setCustomRenderer([this](sf::RenderTarget& target, A13RocketPartItem* item)
    {
        EGE::Renderer renderer(target);
        const unsigned HEIGHT = 40;

        // Background
        renderer.renderRectangle(m_partSelector->getSize().x, 0, 230, m_partSelector->getSize().y, sf::Color(0, 0, 0, 127));

        // Item name
        renderer.renderText(m_partSelector->getSize().x + 10, 20, *m_font, item->getLabel(), 16);

        // Cost
        Cost cost = item->getRocketPart()->getCost();

        EGE::Size s = 0;
        for(ResourceItemStack& stack: cost)
        {
            std::string str = stack.getItem()->getLabel() + " x" + std::to_string(stack.getItemCount());

            // Text
            sf::Color color = A13::PlayerStats::instance().getInventory()[stack.getItem()] < stack.getItemCount() ? sf::Color::Red : sf::Color::White;
            renderer.renderText(m_partSelector->getSize().x + 47, HEIGHT * (s + 1) + 20, *m_font, str, 11, color);

            // Icon
            sf::IntRect texRect;
            // TODO: Name that '16' somehow
            texRect.left = stack.getItem()->getAtlasPosition().x * 16;
            texRect.top = stack.getItem()->getAtlasPosition().y * 16;
            texRect.width = 16;
            texRect.height = 16;
            renderer.renderTexturedRectangle(m_partSelector->getSize().x + 10, HEIGHT * (s + 1) + 10, 32, 32, *m_resourceStatsWidget->getAtlas(), texRect);

            s++;
        }

        // Description
        //renderer.renderText(m_partSelector->getSize().x + 10, HEIGHT * (s + 1) + 20, *m_font, item->getDescription(), 10, sf::Color(200, 200, 200));
    });

    m_resourceStatsWidget = make<ResourceStatsWidget>(this, A13::PlayerStats::instance().getInventory());
    addWidget(m_resourceStatsWidget);
    m_resourceStatsWidgetProject = make<ResourceStatsWidget>(this, m_tilemap->getTotalCostInv());
    addWidget(m_resourceStatsWidgetProject);
    m_infoLabel = make<EGE::Label>(this);
    m_infoLabel->setTextAlign(EGE::Label::Align::Center);
    m_infoLabel->setString("Not built anything!");
    addWidget(m_infoLabel);
    recalculate();
}

bool A13GUIProjectBuilder::checkValidity()
{
    // Ensure that project time is properly calculated.
    recalculate();

    int capsules = 0, fuelTankUnits = 0, engines = 0;
    for(auto it : m_tilemap->getParts())
    {
        std::string id = it.second->part->getId();
        if(id == "a13:capsule:generic")
            capsules++;
        else if(id.find("a13:fuel_tank:generic:") == 0)
        {
            try
            {
                fuelTankUnits += stoi(id.substr(22));
            }
            catch(...)
            {
                CRASH();
            }
        }
        else if(id == "a13:engine:generic")
        {
            engines++;
        }
    }
    return projectTime == 0 || (capsules == 1 && fuelTankUnits >= 4 && engines >= 1);
}
