#include "A13RocketPart.h"
#include "A13GameplayObjectManager.h"

EGE::Vec2d A13RocketPartPart::getAtlasPosition(int) const
{
    return part->getAtlasPosition();
}

EGE::Vec2u A13RocketPartPart::getSize() const
{
    return part->getSize();
}

EGE::SharedPtr<A13RocketPartPart> A13RocketPartItem::getPart() const
{
    return make<A13RocketPartPart>(m_tile);
}
