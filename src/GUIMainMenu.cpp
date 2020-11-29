#include "GUIMainMenu.h"

#include "Apollo13.h"
#include "MessageBox.h"

#define GMM_MB_RMSAVE 101

namespace A13
{

void GUIMainMenu::onLoad()
{
    EGE::GUIScreen::onLoad();

    m_loadSave = make<EGE::Button>(this);
    m_loadSave->setLabel("Load/Create Save");
    m_loadSave->setCallback( [this]{ Apollo13::instance().loadSave("main"); } );
    addWidget(m_loadSave);

    m_deleteSave = make<EGE::Button>(this);
    m_deleteSave->setLabel("Delete Current Save");
    m_deleteSave->setCallback( [this]{ Apollo13::instance().messageBox("Do you want to remove save?\nIt cannot be undone!", GMM_MB_RMSAVE, A13::MessageBox::Type::YesNo); } );
    addWidget(m_deleteSave);

    m_exitGame = make<EGE::Button>(this);
    m_exitGame->setLabel("Exit Game");
    m_exitGame->setCallback( [this]{ getLoop()->exit(); } );
    addWidget(m_exitGame);

    m_labelGameName = make<EGE::Label>(this);
    m_labelGameName->setString("MOONSHOT TYCOON");
    m_labelGameName->setTextAlign(EGE::Label::Align::Center);
    m_labelGameName->setFontSize(60);
    m_labelGameName->setColor(sf::Color::Black);
    addWidget(m_labelGameName);

    m_labelCredits = make<EGE::Label>(this);
    m_labelCredits->setString("v1.0 beta :: Sppmacd, PumpkinCell (c) 2020.");
    m_labelCredits->setColor(sf::Color::Black);
    addWidget(m_labelCredits);
}

void GUIMainMenu::onResize(sf::Event::SizeEvent& event)
{
    EGE::GUIScreen::onResize(event);

    m_loadSave->setPosition({event.width / 2 - 200, event.height / 2 - 100});
    m_loadSave->setSize({400, 40});

    m_deleteSave->setPosition({event.width / 2 - 200, event.height / 2});
    m_deleteSave->setSize({400, 40});

    m_exitGame->setPosition({event.width / 2 - 200, event.height / 2 + 100});
    m_exitGame->setSize({400, 40});

    m_labelGameName->setPosition({event.width / 2, event.height / 2 - 200});

    m_labelCredits->setPosition({10, event.height - 20});
}

void GUIMainMenu::onDialogExit(EGE::GUIScreen* dialog, int exitCode)
{
    MessageBox* mb = (MessageBox*)dialog;
    if(mb && exitCode)
    {
        if(mb->getId() == GMM_MB_RMSAVE && exitCode == MSGBOX_RET_YES)
        {
            Apollo13::instance().save.remove();
        }
    }
}

}
