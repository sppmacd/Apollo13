#pragma once

#include <ege/gui/GUIGameLoop.h>

class Apollo13 : public EGE::GUIGameLoop
{
public:
    virtual EGE::EventResult load() override;
};
