#pragma once

#include <ege/gui/Widget.h>

namespace A13
{

class ProjectStatsWidget : public EGE::Widget
{
public:
    ProjectStatsWidget(EGE::Widget* parent)
    : EGE::Widget(parent) {}

    virtual void onLoad();

    virtual void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states);

private:
    sf::Font* m_font = nullptr;
    sf::Texture* m_atlas = nullptr;
};

}
