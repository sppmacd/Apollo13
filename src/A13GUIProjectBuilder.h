#pragma once

#include "A13GUIAbstractBuilder.h"
#include "A13RocketPart.h"

class A13GUIProjectBuilder : public A13GUIAbstractBuilder<A13ProjectTilemap, A13RocketPartItem>
{
public:
    A13GUIProjectBuilder(EGE::GUIGameLoop* loop);
};
