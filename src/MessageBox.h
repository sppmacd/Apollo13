#pragma once

#include <ege/gui/GUIScreen.h>
#include <ege/gui/Button.h>

#define MSGBOX_RET_OK     0
#define MSGBOX_RET_YES    1
#define MSGBOX_RET_NO     2
#define MSGBOX_RET_CANCEL 3

#define MBID_DONT_CARE 0xFFFFFFFF

namespace A13
{

// modal dialog.
class MessageBox : public EGE::GUIScreen
{
public:
    enum class Type
    {
        Ok,
        OkCancel,
        YesNo,
        YesNoCancel
    };

    MessageBox(EGE::Widget* parent, EGE::IdType id, std::string message, Type type = Type::Ok)
    : EGE::GUIScreen(parent), m_id(id), m_message(message), m_type(type) {}

    virtual void onLoad() override;
    virtual void onResize(sf::Event::SizeEvent& event) override;
    virtual void onKeyPress(sf::Event::KeyEvent& event) override;
    virtual void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states = {}) override;

    EGE::IdType getId() { return m_id; }

private:
    struct MsgBoxButton
    {
        EGE::SharedPtr<EGE::Button> button;
        int exitCode;
    };

    void addMsgBoxButton(std::string label, int exitCode)
    {
        auto button = make<EGE::Button>(this);
        button->setLabel(label);
        button->setCallback([this, button, exitCode]() {
                            exitDialog(exitCode);
                        });
        m_buttons.push_back({button, exitCode});
        addWidget(button);
    }

    sf::Font* m_font = nullptr;
    EGE::IdType m_id;
    std::string m_message;
    Type m_type;
    EGE::Vector<MsgBoxButton> m_buttons;
};

}
