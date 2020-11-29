#include "GUIIngameMenu.h"

#include "Apollo13.h"
#include "MessageBox.h"
#include "GUIMainMenu.h"

namespace A13
{

void GUIIngameMenu::onLoad()
{
    EGE::GUIScreen::onLoad();

    m_resumeGame = make<EGE::Button>(this);
    m_resumeGame->setLabel("Resume Game");
    m_resumeGame->setCallback( [this]{ exitDialog(0); } );
    addWidget(m_resumeGame);

    m_exitSave = make<EGE::Button>(this);
    m_exitSave->setLabel("Save and return to main menu");
    m_exitSave->setCallback( [this]{
        Apollo13::instance().save.unload();
        Apollo13::instance().setCurrentGUIScreen(make<A13::GUIMainMenu>(getLoop()), EGE::GUIGameLoop::GUIScreenImmediateInit::Yes);
    } );
    addWidget(m_exitSave);

    m_exitGame = make<EGE::Button>(this);
    m_exitGame->setLabel("Save and exit game");
    m_exitGame->setCallback( [this]{ getLoop()->exit(); } );
    addWidget(m_exitGame);

    m_labelGameName = make<EGE::Label>(this);
    m_labelGameName->setString("MOONSHOT TYCOON");
    m_labelGameName->setTextAlign(EGE::Label::Align::Center);
    m_labelGameName->setFontSize(60);
    addWidget(m_labelGameName);
}

void GUIIngameMenu::onResize(sf::Event::SizeEvent& event)
{
    EGE::GUIScreen::onResize(event);

    m_resumeGame->setPosition({event.width / 2 - 200, event.height / 2 - 100});
    m_resumeGame->setSize({400, 40});

    m_exitSave->setPosition({event.width / 2 - 200, event.height / 2});
    m_exitSave->setSize({400, 40});

    m_exitGame->setPosition({event.width / 2 - 200, event.height / 2 + 100});
    m_exitGame->setSize({400, 40});

    m_labelGameName->setPosition({event.width / 2, event.height / 2 - 200});
}

void GUIIngameMenu::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    EGE::Renderer renderer(target);

    // Background
    renderer.renderRectangle(getSize().x * 0.25, getSize().y * 0.25, getSize().x * 0.5, getSize().y * 0.5, sf::Color(0, 0, 0, 200));
}

}
