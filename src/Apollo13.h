#pragma once

#include <ege/gui/GUIGameLoop.h>

class Apollo13 : public EGE::GUIGameLoop
{
public:
    EGE_SINGLETON(Apollo13);

    virtual EGE::EventResult load() override;
};
