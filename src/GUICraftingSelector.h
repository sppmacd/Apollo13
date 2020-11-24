#pragma once

#include "A13FactoryBuilding.h"

#include <ege/gui/GUIScreen.h>

namespace A13
{

class Recipe;

class GUICraftingSelector : public EGE::GUIScreen
{
public:
    GUICraftingSelector(EGE::Widget* parent, A13FactoryBuildingFactory::Part* factory)
    : EGE::GUIScreen(parent), m_factory(factory) {}

    class CraftingSelectionWidget : public EGE::Widget
    {
    public:
        CraftingSelectionWidget(EGE::Widget* parent)
        : EGE::Widget(parent) {}

        virtual void onLoad() override;
        virtual void onResize(sf::Event::SizeEvent& event) override;
        virtual void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states = {}) override;
        virtual void onMouseButtonPress(sf::Event::MouseButtonEvent& event) override;

        EGE::Vector<A13::Recipe*> m_recipes;
        int m_selectedRecipe = -1;
        sf::Font* m_font = nullptr;
        sf::Texture* m_atlas = nullptr;
    };

    virtual void onLoad() override;
    virtual void onUnload() override;
    virtual void onKeyPress(sf::Event::KeyEvent& event) override;

private:
    A13FactoryBuildingFactory::Part* m_factory;
    EGE::SharedPtr<CraftingSelectionWidget> m_selector;
};

}
