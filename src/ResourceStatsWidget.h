#pragma once

#include <ege/gui/Widget.h>

#include "A13GameplayObjectManager.h"
//#include "A13PlayerStats.h"

class ResourceStatsWidget : public EGE::Widget
{
public:
    ResourceStatsWidget(EGE::Widget* parent)
    : EGE::Widget(parent) {}

    virtual void onLoad();
    virtual void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states = {});

    sf::Texture* getAtlas() { return m_atlas; }

private:
    sf::Texture* m_atlas = nullptr;
    sf::Font* m_font = nullptr;
    EGE::StringMap<ResourceItem*> m_items;
};
