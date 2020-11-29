#pragma once

#include <ege/gui/Button.h>
#include <ege/gui/GUIScreen.h>
#include <ege/gui/Label.h>

namespace A13
{

class GUIMainMenu : public EGE::GUIScreen
{
public:
    GUIMainMenu(EGE::GUIGameLoop* parent)
    : EGE::GUIScreen(parent) {}

    virtual void onLoad() override;
    virtual void onResize(sf::Event::SizeEvent& event) override;
    virtual void onDialogExit(EGE::GUIScreen* dialog, int exitCode);

private:
    EGE::SharedPtr<EGE::Button> m_loadSave;
    EGE::SharedPtr<EGE::Button> m_deleteSave;
    EGE::SharedPtr<EGE::Button> m_exitGame;
    EGE::SharedPtr<EGE::Label> m_labelGameName;
    EGE::SharedPtr<EGE::Label> m_labelCredits;
    sf::Font* m_font;
};

}
