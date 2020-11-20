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

class EntityItemCarrier : public Entity, public Container
{
public:
    EntityItemCarrier(std::shared_ptr<EGE::Scene> owner, A13::FactoryTilemap* tilemap);

    virtual void onUpdate(long long tickCounter);
    void render(sf::RenderTarget& target, const EGE::RenderStates& states) const;

    virtual EGE::Vec2d getAtlasPosition() { return {2, 0}; }

private:
    void doUpdate(EGE::TickCount tickCounter);

    EGE::TickCount lastDirCheck = 0;
    EGE::Vec2i m_spawnTilePos;
    A13::FactoryBuildingPart* m_spawnPart = nullptr;
    EGE::Size m_sideCounter = 0;
    int m_currentDir = -1;
    bool m_initialized = false;
};

}
