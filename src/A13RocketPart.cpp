#include "A13RocketPart.h"
#include "A13GameplayObjectManager.h"

EGE::Vec2d A13RocketPartTile::getAtlasPosition() const
{
    return part->getAtlasPosition();
}

EGE::Vec2u A13RocketPartTile::getSize() const
{
    return part->getSize();
}

EGE::SharedPtr<A13RocketPartTile> A13RocketPartItem::getPart() const
{
    return make<A13RocketPartTile>(m_tile);
}
