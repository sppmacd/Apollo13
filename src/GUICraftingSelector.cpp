#include "GUICraftingSelector.h"

#include "A13GameplayObjectManager.h"

namespace A13
{

void GUICraftingSelector::CraftingSelectionWidget::onLoad()
{
    EGE::Widget::onLoad();

    log() << "Load CSW";
    auto resMan = getLoop()->getResourceManager().lock();
    m_atlas = resMan->getTexture("game/resource_items.png").get();
    m_font = resMan->getDefaultFont().get();

    EGE::Size s = 0;
    GUICraftingSelector* parent = (GUICraftingSelector*)m_parent;
    if(!parent)
        CRASH();

    m_recipes.push_back(nullptr);
    m_selectedRecipe = 0;
    for(auto& rc: A13GameplayObjectManager::instance().recipes)
    {
        s++;
        m_recipes.push_back(rc.second.get());
        if(parent->m_factory->getRecipe() == rc.second.get())
            m_selectedRecipe = s;
    }
}

void GUICraftingSelector::CraftingSelectionWidget::onResize(sf::Event::SizeEvent& event)
{
    EGE::Widget::onResize(event);

    setSize({event.width, event.height});
}

void GUICraftingSelector::CraftingSelectionWidget::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    EGE::Renderer renderer(target);

    ASSERT(m_atlas);
    ASSERT(m_font);

    sf::RenderWindow* wnd = (sf::RenderWindow*)&target;

    // Background
    renderer.renderRectangle(0, 0, getSize().x, getSize().y, sf::Color(60, 60, 60, 127));

    // Label
    renderer.renderText(200, 30, *m_font, "Select a crafting for Factory", 30, sf::Color::White);

    // Craftings
    EGE::Size s = 0;
    const int ITEM_SIZE = 60;
    const int ITEM_ICON_SIZE = 48;
    for(Recipe* recipe : m_recipes)
    {
        // Icon
        auto ap = recipe ? recipe->output.getItem()->getAtlasPosition() : EGE::Vec2d(15, 15);
        sf::IntRect textureRect;
        textureRect.left = ap.x * 16;
        textureRect.top = ap.y * 16;
        textureRect.width = 16;
        textureRect.height = 16;
        double x = (s % 16) * ITEM_SIZE + 200;
        double y = (s / 16) * ITEM_SIZE + 100;
        renderer.renderTexturedRectangle(x, y, ITEM_ICON_SIZE, ITEM_ICON_SIZE, *m_atlas, textureRect);

        // Highlight (if hovered)if(wnd)
        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*wnd);
            if(   mousePos.x > x && mousePos.x < x + ITEM_ICON_SIZE
               && mousePos.y > y && mousePos.y < y + ITEM_ICON_SIZE)
            {
                renderer.renderRectangle(x, y, ITEM_ICON_SIZE, ITEM_ICON_SIZE, sf::Color(255, 255, 255, 127));
            }
        }

        // Highlight (if active)
        if(s == m_selectedRecipe)
            renderer.renderRectangle(x, y, ITEM_ICON_SIZE, ITEM_ICON_SIZE, sf::Color(0, 255, 0, 127));

        s++;
    }
}

void GUICraftingSelector::CraftingSelectionWidget::onMouseButtonPress(sf::Event::MouseButtonEvent& event)
{
    EGE::Widget::onMouseButtonPress(event);

    // Check if any recipe is selected
    m_selectedRecipe = -1;
    EGE::Size s = 0;
    const int ITEM_SIZE = 60;
    const int ITEM_ICON_SIZE = 48;
    sf::RenderWindow* wnd = getWindow().lock().get();
    for(Recipe* recipe : m_recipes)
    {
        double x = (s % 16) * ITEM_SIZE + 200;
        double y = (s / 16) * ITEM_SIZE + 100;

        {
            sf::Vector2i mousePos = sf::Mouse::getPosition(*wnd);
            if(   mousePos.x > x && mousePos.x < x + ITEM_ICON_SIZE
               && mousePos.y > y && mousePos.y < y + ITEM_ICON_SIZE
               && m_leftClicked)
            {
                m_selectedRecipe = s;
            }
        }

        s++;
    }
}

/////////////////////////////////////////////////////////

void GUICraftingSelector::onLoad()
{
    EGE::GUIScreen::onLoad();

    m_selector = make<CraftingSelectionWidget>(this);
    addWidget(m_selector);
}

void GUICraftingSelector::onKeyPress(sf::Event::KeyEvent& event)
{
    EGE::GUIScreen::onKeyPress(event);

    if(event.code == sf::Keyboard::Escape)
    {
        exitDialog(0);
    }
}

void GUICraftingSelector::onUnload()
{
    EGE::GUIScreen::onUnload();

    m_factory->setRecipe(m_selector->m_selectedRecipe == -1 ? nullptr : m_selector->m_recipes[m_selector->m_selectedRecipe]);
}

}
