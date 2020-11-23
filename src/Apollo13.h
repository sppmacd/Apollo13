#pragma once

#include <ege/gui/GUIGameLoop.h>

class Apollo13 : public EGE::GUIGameLoop
{
public:
    EGE_SINGLETON(Apollo13);

    virtual EGE::EventResult load() override;

    virtual void logicTick(long long tickCount);

    EGE::MaxInt getSeed() { return m_seed; }

private:
    EGE::MaxInt m_seed = 0;
};
