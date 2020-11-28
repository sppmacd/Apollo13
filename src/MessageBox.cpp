#include "MessageBox.h"

#include <ege/gfx/Renderer.h>
#include <ege/gui/GUIGameLoop.h>

namespace A13
{

void MessageBox::onLoad()
{
    EGE::GUIScreen::onLoad();

    // Load resources
    auto resMan = getLoop()->getResourceManager().lock();
    m_font = resMan->getDefaultFont().get();

    // Add buttons
    switch(m_type)
    {
    case Type::Ok:
        {
            addMsgBoxButton("Ok", MSGBOX_RET_OK);
            break;
        }
    case Type::OkCancel:
        {
            addMsgBoxButton("Ok", MSGBOX_RET_OK);
            addMsgBoxButton("Cancel", MSGBOX_RET_CANCEL);
            break;
        }
    case Type::YesNo:
        {
            addMsgBoxButton("Yes", MSGBOX_RET_YES);
            addMsgBoxButton("No", MSGBOX_RET_NO);
            break;
        }
    case Type::YesNoCancel:
        {
            addMsgBoxButton("Yes", MSGBOX_RET_YES);
            addMsgBoxButton("No", MSGBOX_RET_NO);
            addMsgBoxButton("Cancel", MSGBOX_RET_CANCEL);
            break;
        }
    default:
        CRASH();
    }
}

void MessageBox::onResize(sf::Event::SizeEvent& event)
{
    EGE::GUIScreen::onResize(event);

    const double BOX_MARGIN_S = 0.25;
    setSize({event.width * (1 - BOX_MARGIN_S * 2), event.height * (1 - BOX_MARGIN_S * 2)});
    setPosition({event.width * BOX_MARGIN_S, event.height * BOX_MARGIN_S});

    // Update buttons
    EGE::Size s = 0;
    EGE::Size maxs = m_buttons.size();
    const double BUTTON_MARGIN = 40;
    const double BUTTON_SIZE = (getSize().x - BUTTON_MARGIN * (maxs + 1)) / maxs;
    const double BUTTON_SIZE_Y = 30;

    for(auto& it: m_buttons)
    {
        const double BUTTON_POSITION = s * (BUTTON_SIZE + BUTTON_MARGIN) + BUTTON_MARGIN;
        it.button->setPosition({BUTTON_POSITION, getSize().y - BUTTON_SIZE_Y - BUTTON_MARGIN});
        it.button->setSize({BUTTON_SIZE, BUTTON_SIZE_Y});
        s++;
    }
}

void MessageBox::onKeyPress(sf::Event::KeyEvent& event)
{
    EGE::GUIScreen::onKeyPress(event);

    if(event.code == sf::Keyboard::Escape && !m_buttons.empty())
        exitDialog(m_buttons[m_buttons.size() - 1].exitCode);
}

void MessageBox::renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states)
{
    EGE::Renderer renderer(target);

    // Background
    renderer.renderRectangle(0, 0, getSize().x, getSize().y, sf::Color(0, 0, 0, 200));

    // Label
    EGE::Renderer::TextWithBackgroundSettings settings;
    settings.text_align = EGE::Renderer::Center;
    settings.background_color = sf::Color::Transparent;
    renderer.renderTextWithBackground(getSize().x / 2, getSize().y / 2 - 30, *m_font, m_message, settings);
}

}
