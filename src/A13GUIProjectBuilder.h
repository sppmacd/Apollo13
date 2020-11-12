#pragma once

#include "A13RocketPart.h"
#include "A13GUIAbstractBuilder.h"

#include <ege/gui/GUIScreen.h>
#include <ege/scene/CameraObject2D.h>
#include <ege/scene/Scene2D.h>
#include <ege/tilemap/FixedTileMap2D.h>

class A13GUIProjectBuilder : public A13GUIAbstractBuilder<A13RocketPart, 48, 64>
{
public:
    A13GUIProjectBuilder(EGE::GUIGameLoop* loop)
    : A13GUIAbstractBuilder(loop)
    {
        setBuilderBackground("gui/pb/background.png");
        setBuilderAtlas("gui/pb/rocket_parts.png");
        setSelectorAtlas("gui/pb/rocket_parts.png");
    }
};
