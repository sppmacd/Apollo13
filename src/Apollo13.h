#pragma once

#include <ege/gui/GUIGameLoop.h>

#include "Save.h"

class Apollo13 : public EGE::GUIGameLoop
{
public:
    EGE_SINGLETON(Apollo13);

    virtual EGE::EventResult load() override;
    virtual EGE::EventResult onFinish(int exitCode) override;

    virtual void logicTick(long long tickCount);

    EGE::MaxInt getSeed() { return m_seed; }

    A13::Save save;

private:
    EGE::MaxInt m_seed = 0;
};
