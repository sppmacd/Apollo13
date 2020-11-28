#pragma once

#include <ege/gui/GUIGameLoop.h>
#include "MessageBox.h"

#include "Save.h"

class Apollo13 : public EGE::GUIGameLoop
{
public:
    EGE_SINGLETON(Apollo13);

    virtual EGE::EventResult load() override;
    virtual EGE::EventResult onFinish(int exitCode) override;

    virtual void logicTick(long long tickCount);

    EGE::MaxInt getSeed() { return m_seed; }

    void messageBox(std::string message, EGE::IdType id, A13::MessageBox::Type type = A13::MessageBox::Type::Ok);

    A13::Save save;

private:
    void messageBoxHelper(EGE::SharedPtr<EGE::GUIScreen> scr, std::string message, EGE::IdType id, A13::MessageBox::Type type);

    EGE::MaxInt m_seed = 0;
};
