#pragma once

#include "../ResourceItems/Container.h"
#include "../FactoryTilemap.h"

#include <ege/scene/SceneObject2D.h>
#include <ege/scene/TexturedRenderer2D.h>

namespace A13
{

class Entity : public EGE::SceneObject2D
{
public:
    Entity(std::shared_ptr<EGE::Scene> owner, A13::FactoryTilemap* tilemap, std::string id);

    virtual void preRenderUpdate(sf::RenderTarget& target);

    virtual EGE::Vec2d getAtlasPosition() { return {0, 0}; }

protected:
    A13::FactoryTilemap* const m_tilemap;
    EGE::TexturedRenderer2D* m_texRenderer = nullptr;
};

class EntityItemCarrier : public Entity
{
public:
    EntityItemCarrier(std::shared_ptr<EGE::Scene> owner, A13::FactoryTilemap* tilemap)
    : Entity(owner, tilemap, "item_carrier") {}

    virtual void onUpdate(long long tickCounter);

    void setDestination(EGE::Vec2i d);

    virtual EGE::Vec2d getAtlasPosition() { return {1, 0}; }

private:
    A13::Container m_internalStorage;
    EGE::Vec2d m_destination {0, 0};
};

}
