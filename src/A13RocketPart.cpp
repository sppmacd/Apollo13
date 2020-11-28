#include "A13RocketPart.h"
#include "A13GameplayObjectManager.h"

#include "Apollo13.h"
#include "PlayerStats.h"
#include "Save.h"

EGE::SharedPtr<EGE::ObjectMap> A13RocketPartPart::serialize()
{
    // Empty for now
    return make<EGE::ObjectMap>();
}

bool A13RocketPartPart::deserialize(EGE::SharedPtr<EGE::ObjectMap>)
{
    // Empty for now
    return true;
}

EGE::SharedPtr<EGE::ObjectMap> A13ProjectTilemap::serialize()
{
    log() << "Saving part info for project builder";
    EGE::SharedPtr<EGE::ObjectMap> obj = make<EGE::ObjectMap>();
    EGE::SharedPtr<EGE::ObjectList> _parts = make<EGE::ObjectList>();

    for(auto& obj: m_parts)
    {
        auto _part = make<EGE::ObjectMap>();
        _part->addObject("coords", EGE::Serializers::fromVector2(obj.first));
        _part->addObject("data", obj.second->serialize());

        if(obj.second->part)
            _part->addString("id", obj.second->part->getId());

        _parts->addObject(_part);
    }

    obj->addObject("parts", _parts);
    obj->addInt("status", m_currentProjectTime);
    obj->addObject("inventory", m_items.serialize());

    // rocket data
    auto _rocket = make<EGE::ObjectMap>();
    _rocket->addInt("speed", m_rocketSpeed);
    _rocket->addFloat("height", m_rocketHeight);
    _rocket->addFloat("fuel", m_rocketFuel);
    _rocket->addFloat("maxHeight", m_rocketMaxHeight);
    _rocket->addFloat("thrust", m_thrust);
    obj->addObject("rocket", _rocket);
    return obj;
}

bool A13ProjectTilemap::deserialize(EGE::SharedPtr<EGE::ObjectMap> obj)
{
    log() << "Loading part info for project builder";
    EGE::WeakPtr<EGE::Object> _parts = obj->getObject("parts");
    if(_parts.expired())
        PRINT_ERROR_AND_RETURN("Not found valid Project/parts key!");

    for(auto& _obj : *((EGE::ObjectList*)_parts.lock().get()))
    {
        if(!_obj->isMap())
            return false;

        auto objMap = _obj->asMap();

        auto _part_id = objMap["id"];
        if(!_part_id)
            return false;
        auto _part_coords = objMap["coords"];
        if(!_part_coords)
            return false;
        auto _part_data = objMap["data"];
        if(!_part_data)
            return false;

        A13RocketPart* bld = A13GameplayObjectManager::instance().rocketParts.findById(_part_id->asString());
        if(!bld)
            return false;

        auto ipart = make<A13RocketPartPart>(bld);
        if(!_part_data->isMap())
            return false;
        ipart->deserialize(std::dynamic_pointer_cast<EGE::ObjectMap>(_part_data));
        if(!_part_coords->isMap())
            return false;
        auto coords = EGE::Serializers::toVector2(std::dynamic_pointer_cast<EGE::ObjectMap>(_part_coords));
        m_parts[coords] = ipart;

        for(EGE::Size x = 0; x < ipart->getSize().x; x++)
        for(EGE::Size y = 0; y < ipart->getSize().y; y++)
        {
            StateType* tile = getTile({(int)x + coords.x, (int)y + coords.y});
            tile->cornerPos = {x, y};
            tile->part = ipart.get();
        }
    }

    // Project status
    auto _status = obj->getObject("status");
    if(_status.expired() || !_status.lock()->isFloat())
        PRINT_ERROR_AND_RETURN("Not found valid Project/status key!");
    m_currentProjectTime = _status.lock()->asInt();

    auto _inventory = obj->getObject("inventory");
    if(_inventory.expired() || !_inventory.lock()->isMap())
        PRINT_ERROR_AND_RETURN("Not found valid Project/inventory key!");
    if(!m_items.deserialize(std::dynamic_pointer_cast<EGE::ObjectMap>(_inventory.lock())))
        PRINT_ERROR_AND_RETURN("Invalid Project/inventory key!");

    // Calculate total cost
    for(auto it: getParts())
    {
        m_totalCostInv.tryAddItems(it.second->part->getCost());
    }

    // Calculate project time
    recalculateRocketProperties();

    // Rocket data
    auto _rocket = obj->getObject("rocket");
    if(_rocket.expired() || !_rocket.lock()->isMap())
        PRINT_ERROR_AND_RETURN("Invalid Project/rocket key!");
    {
        auto rocket = _rocket.lock()->asMap();
        auto _rocket_speed = rocket["speed"];
        auto _rocket_height = rocket["height"];
        auto _rocket_fuel = rocket["fuel"];
        auto _rocket_maxHeight = rocket["maxHeight"];
        auto _rocket_thrust = rocket["thrust"];

        if(!_rocket_speed ||
           !_rocket_height ||
           !_rocket_fuel ||
           !_rocket_maxHeight ||
           !_rocket_thrust)
            PRINT_ERROR_AND_RETURN("Not found Project/rocket keys!");

        if(!_rocket_speed->isFloat() ||
           !_rocket_height->isFloat() ||
           !_rocket_fuel->isFloat() ||
           !_rocket_maxHeight->isFloat() ||
           !_rocket_thrust->isFloat())
            PRINT_ERROR_AND_RETURN("Invalid format of Project/rocket keys!");

        m_rocketSpeed = _rocket_speed->asFloat();
        m_rocketHeight = _rocket_height->asFloat();
        m_rocketFuel = _rocket_fuel->asFloat();
        m_rocketMaxHeight = _rocket_maxHeight->asFloat();
        m_thrust = _rocket_thrust->asFloat();
    }

    return true;
}

void A13ProjectTilemap::onCloseProjectBuilder()
{
    log() << "onCloseProjectBuilder";
    if(m_currentProjectTime >= 0)
        CRASH(); // We should NOT be able to open Project Builder while building rocket!

    if(m_currentProjectTime == -1)
    {
        // Give back all resources
        A13::PlayerStats::instance().loadItemsFrom(&m_items);
    }

    // TODO: Cancel resource requests

    // Calculate project time, thrust etc.
    recalculateRocketProperties();

    // Request resources
    auto missing = getMissingItems();
    for(auto it: missing)
    {
        A13::PlayerStats::instance().addResourceItemRequest({it.first, it.second}, &m_items);
    }

    // Do nothing if rocket is empty
    if(m_totalProjectTime == 0)
    {
        m_currentProjectTime = -2;
        return;
    }

    // Set to RESOURCES REQUESTED state
    m_currentProjectTime = -1;
}

void A13ProjectTilemap::recalculateRocketProperties()
{
    m_totalProjectTime = 0;
    m_maxThrust = 0;
    m_fuelUsage = 0;
    m_rocketFuel = 0;
    m_fuelTotal = 0;
    m_mass = 0;
    m_totalCostInv.clear();

    for(auto& it: getParts())
    {
        int i = it.second->part->getBuildTime();
        log() << "Adding time to totalProjectTime: " << i;
        m_totalProjectTime += it.second->part->getBuildTime();
        m_maxThrust += it.second->part->getThrust();
        m_fuelUsage += it.second->part->getFuelUsage();
        m_fuelTotal += it.second->part->getFuelStorage();
        m_mass += it.second->part->getMass();
        m_totalCostInv.tryAddItems(it.second->part->getCost());
    }
}

void A13ProjectTilemap::update()
{
    if(getCurrentProjectTime() == -3)
    {
        const double GRAVITY = 160000000000;

        // Rocket physics (simple)
        m_rocketTick++;
        double dst = m_rocketHeight + 1000000;
        m_rocketSpeed += (m_thrust / 60.0 - GRAVITY * (m_mass + m_rocketFuel)
                               / (dst * dst)) / (m_mass + m_rocketFuel);
        m_rocketHeight += m_rocketSpeed / 60.0;

        if(m_rocketHeight <= 0)
        {
            m_rocketHeight = 0;
            m_rocketSpeed = 0;
        }

        if(m_rocketHeight > m_rocketMaxHeight)
            m_rocketMaxHeight = m_rocketHeight;

        if(m_rocketFuel > 0)
            m_rocketFuel -= m_fuelUsage * (m_thrust / m_maxThrust) / 60.0;
        else
        {
            m_rocketFuel = 0;
            m_thrust = 0;
        }

        // Win game
        if(m_rocketHeight == 0)
        {
            if(m_rocketMaxHeight > 100000)
            {
                // TODO: calculate point count, height, etc.
                Apollo13::instance().messageBox("You won the game!\n\nYour points: " + std::to_string(calculatePointsForWin()), MBID_DONT_CARE);
                m_rocketTick = -1;
                winGame();
                //Apollo13::instance().winGame();
            }
            else if(m_rocketMaxHeight > 0)
            {
                Apollo13::instance().messageBox("The rocket crashed!", MBID_DONT_CARE);
                m_rocketTick = -1;
                loseGame();
            }
        }
    }
}

double A13ProjectTilemap::calculatePointsForWin()
{
    return m_rocketMaxHeight / 100.0;
}

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
    return make<A13RocketPartPart>(m_rocketPart);
}


Cost A13RocketPartEngine::getCost() const
{
    return {
        { A13GameplayObjectManager::items.aluminum, 200 },
        { A13GameplayObjectManager::items.titanium, 100 },
        { A13GameplayObjectManager::items.gold, 50 },
        { A13GameplayObjectManager::items.silver, 50 }
    };
}

Cost A13RocketPartFuelTank::getCost() const
{
    return {
        { A13GameplayObjectManager::items.aluminum, 300 * (m_size - 0.1) },
        { A13GameplayObjectManager::items.titanium, 25 * m_size },
        { A13GameplayObjectManager::items.silicon, 10 * m_size }
    };
}

Cost A13RocketPartCone::getCost() const
{
    return {
        { A13GameplayObjectManager::items.aluminum, 240 },
        { A13GameplayObjectManager::items.titanium, 100 },
        { A13GameplayObjectManager::items.copper, 50 }
    };
}

Cost A13RocketPartCapsule::getCost() const
{
    return {
        { A13GameplayObjectManager::items.aluminum, 300 },
        { A13GameplayObjectManager::items.titanium, 300 },
        { A13GameplayObjectManager::items.copper, 100 },
        { A13GameplayObjectManager::items.gold, 50 },
        { A13GameplayObjectManager::items.silver, 50 },
        { A13GameplayObjectManager::items.diamond, 25 }
    };
}
