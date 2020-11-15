#pragma once

#include "A13GUIAbstractBuilder.h"
#include "A13RocketPart.h"

// modal dialog
class A13GUIProjectBuilder : public A13GUIAbstractBuilder<A13ProjectTilemap, A13RocketPartItem>
{
public:
    A13GUIProjectBuilder(EGE::GUIScreen* loop);

    void onKeyPress(sf::Event::KeyEvent& event);

    CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13ProjectTilemap::StateType& tile, A13RocketPartItem* item)
    {
        return item->canPlaceHere(tileRel, tile);
    }
};
