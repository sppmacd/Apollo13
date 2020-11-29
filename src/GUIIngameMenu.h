#pragma once

#include <ege/gui/Button.h>
#include <ege/gui/GUIScreen.h>
#include <ege/gui/Label.h>

namespace A13
{

// modal dialog
class GUIIngameMenu : public EGE::GUIScreen
{
public:
    GUIIngameMenu(EGE::Widget* parent)
    : EGE::GUIScreen(parent) {}

    virtual void onLoad() override;
    virtual void onResize(sf::Event::SizeEvent& event) override;
    virtual void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states) override;

private:
    EGE::SharedPtr<EGE::Button> m_resumeGame;
    EGE::SharedPtr<EGE::Button> m_exitSave;
    EGE::SharedPtr<EGE::Button> m_exitGame;
    EGE::SharedPtr<EGE::Label> m_labelGameName;
    sf::Font* m_font;
};

}
