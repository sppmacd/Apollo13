#include "Entity.h"

#include "../A13FactoryBuilding.h"
#include "../FactoryTilemap.h"
#include "../Apollo13.h"
#include "../A13GUIFactoryBuilder.h"

#include <ege/util.h>

#include <signal.h>
#include <unistd.h>

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
    rect.left = getAtlasPosition().x * 16;
    rect.top = getAtlasPosition().y * 16;
    rect.width = 16;
    rect.height = 16;
    m_texRenderer->setTextureRect(rect);
}

EntityItemCarrier::EntityItemCarrier(std::shared_ptr<EGE::Scene> owner, A13::FactoryTilemap* tilemap)
: Entity(owner, tilemap, "item_carrier")
{
}

void EntityItemCarrier::render(sf::RenderTarget& target, const EGE::RenderStates& states) const
{
    Entity::render(target, states);
    EGE::Vec2d tilePos {getPosition().x / 16, getPosition().y / 16};
    auto gui = Apollo13::instance().getCurrentGUIScreen().get();
    ((A13GUIFactoryBuilder*)gui)->renderInventory(tilePos, m_inventory, target);
}

void EntityItemCarrier::doUpdate(EGE::TickCount tickCounter)
{
    EGE::Vec2i tilePos = {std::floor(getPosition().x / 16), std::floor(getPosition().y / 16)};
    auto tile = m_tilemap->ensureTile(tilePos);

    if(!m_initialized)
    {
        m_spawnTilePos = tilePos;

        if(tile.part && tile.part->container)
        {
            m_spawnPart = tile.part;
        }
        // Do not access *spawnPart after leaving constructor (UB)!

        m_sideCounter = EGE::Random::fastRandom().nextInt(4);
        m_initialized = true;
    }

    EGE::Uint8 logistic = tile.addObjs[FACTORY_BUILDER_LAYER_LOGISTIC];

    const int ONE_TILE_TIME = 60;

    float speed_ts = 16.0 / ONE_TILE_TIME;

    // It we are on part and it's not a spawn part, leave items here.
    if(tile.part && m_spawnPart && tile.part != m_spawnPart && tilePos != m_spawnTilePos)
    {
        log() << "Leave items to " << tile.part->building->getId();

        if(tile.part->container)
            tile.part->container->loadItemsFrom(this);

        if(m_inventory.getItemCount() == 0)
        {
            setDead();
            return;
        }
    }

    // TODO: This condition should be something like "is on center of tile"!
    if(tickCounter - lastDirCheck >= ONE_TILE_TIME)
    {
        // Get output directions that are available
        // 0-up, 1-right, 2-down, 3-left
        std::vector<int> dirs;

        if(tile.part && tile.part->container)
        {
            dirs = {0, 1, 2, 3};
        }
        else
        {
            if(logistic & 0b1) dirs.push_back(0);
            if(logistic & 0b10) dirs.push_back(1);
            if(logistic & 0b100) dirs.push_back(2);
            if(logistic & 0b1000) dirs.push_back(3);
        }

        // Get random direction that robot can go to
        int dir = 0;
        int xd = 0, yd = 0;
        EGE::Size lastSideCounter = m_sideCounter;

        std::vector<int> allowedDirs;
        std::vector<int> allowedDirsPart;
        for(EGE::Size s = 0; s < dirs.size(); s++)
        {
            dir = dirs[s];

            xd = 0; yd = 0;
            switch(dir)
            {
                case 0: yd = -1; break;
                case 1: xd = 1; break;
                case 2: yd = 1; break;
                case 3: xd = -1; break;
                default: CRASH();
            }

            log() << (int)getMotion().x << "," << (int)getMotion().y << "::" << (int)xd << "," << (int)yd;
            if(std::abs(m_currentDir - dir) == 2)
            {
                log() << "cannot run in opposite direction";
                continue;
            }

            auto neigh = m_tilemap->ensureTile({tilePos.x + xd, tilePos.y + yd});
            EGE::Uint8 logistic2 = neigh.addObjs[FACTORY_BUILDER_LAYER_LOGISTIC];

            // Input is opposite side to output.
            if  (  (dir == 2 && (logistic2 & 0b10000))
                || (dir == 3 && (logistic2 & 0b100000))
                || (dir == 0 && (logistic2 & 0b1000000))
                || (dir == 1 && (logistic2 & 0b10000000))
            )
            {
                allowedDirs.push_back(dir);
            }
            if(neigh.part && (neigh.part == tile.part || neigh.part->container))
            {
                if(neigh.part->building->getId() == "a13:factory")
                {
                    std::cout << "io";
                }
                allowedDirsPart.push_back(dir);
            }
        }

        if(allowedDirs.empty() && allowedDirsPart.empty())
        {
            setMotion({0, 0});
            m_currentDir = -1;
            return;
        }
        else if(allowedDirs.empty())
            allowedDirs.insert(allowedDirs.begin(), allowedDirsPart.begin(), allowedDirsPart.end());

        for(auto i: allowedDirs)
            std::cerr << i << ",";
        std::cerr << std::endl;

        // Here we are sure that the move will be done.
        lastDirCheck = tickCounter;

        int _dir = allowedDirs[tile.part ? allowedDirs.size() - 1 : EGE::Random::fastRandom().nextInt(allowedDirs.size())];
        int _xd = 0, _yd = 0;
        switch(_dir)
        {
            case 0: _yd = -1; break;
            case 1: _xd = 1; break;
            case 2: _yd = 1; break;
            case 3: _xd = -1; break;
            default: CRASH();
        }

        // Calculate and set motion.
        setMotion({_xd * speed_ts, _yd * speed_ts});
        m_currentDir = _dir;
    }
}

void EntityItemCarrier::onUpdate(long long tickCounter)
{
    doUpdate(tickCounter);
    Entity::onUpdate(tickCounter);
}

}
