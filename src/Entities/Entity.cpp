#include "Entity.h"

#include "../A13FactoryBuilding.h"

namespace A13
{

Entity::Entity(std::shared_ptr<EGE::Scene> owner, A13::FactoryTilemap* tilemap, std::string id)
: EGE::SceneObject2D(owner, "a13:ege:entity:" + id), m_tilemap(tilemap)
{
    auto texR = make<EGE::TexturedRenderer2D>(owner);
    m_texRenderer = texR.get();
    setRenderer(texR);
    m_texRenderer->center();
    m_texRenderer->setTextureName("game/entities.png");
}

void Entity::preRenderUpdate(sf::RenderTarget& target)
{
    EGE::SceneObject2D::preRenderUpdate(target);

    // Update atlas rectangle.
    sf::FloatRect rect;
    rect.left = getAtlasPosition().x;
    rect.top = getAtlasPosition().y;
    rect.width = 32;
    rect.height = 32;
    m_texRenderer->setTextureRect(rect);
}

void EntityItemCarrier::onUpdate(long long tickCounter)
{
    Entity::onUpdate(tickCounter);

    // Go to specified destination.
    if(EGE::VectorOperations::distanceTo({getPosition().x, getPosition().y}, m_destination) < 1.0)
    {
        // TODO: Find part that is there and check if it has a Container.
        EGE::Vec2i tilePos = m_tilemap->getTileAlignedPos({getPosition().x, getPosition().y});
        A13::FactoryTilemap::TileType& tile = m_tilemap->ensureTile(tilePos);
        if(tile.part->building && tile.part->building)
        {
            log() << "Item Carrier finished carrying " << m_internalStorage.getItemCount() << " items to " << tile.part->building->getId();
        }
        setDead();
    }
}

void EntityItemCarrier::setDestination(EGE::Vec2i d)
{
    m_destination = d;
    auto pos = getPosition();
    setRotation(EGE::VectorOperations::angleTo(EGE::Vec2d(pos.x, pos.y), {d.x, d.y}));
    EGE::Vec2d coord = EGE::VectorOperations::fromPolar(EGE::PolVec2d(getRotation(), 5.0));
    setMotion({coord.x, coord.y});
}

}
