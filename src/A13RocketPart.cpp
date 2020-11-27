#include "A13RocketPart.h"
#include "A13GameplayObjectManager.h"

#include "PlayerStats.h"

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
    return obj;
}

bool A13ProjectTilemap::deserialize(EGE::SharedPtr<EGE::ObjectMap> obj)
{
    log() << "Loading part info for project builder";
    EGE::WeakPtr<EGE::Object> _parts = obj->getObject("parts");
    if(_parts.expired())
        return false;

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
        return false;
    m_currentProjectTime = _status.lock()->asInt();

    auto _inventory = obj->getObject("inventory");
    if(_inventory.expired() || !_inventory.lock()->isMap())
        return false;
    if(!m_items.deserialize(std::dynamic_pointer_cast<EGE::ObjectMap>(_inventory.lock())))
        return false;

    // Calculate total cost
    for(auto it: getParts())
    {
        m_totalCostInv.tryAddItems(it.second->part->getCost());
    }

    // Calculate project time
    m_totalProjectTime = 0;
    for(auto& it: getParts())
    {
        int i = it.second->part->getBuildTime();
        log() << "Adding time to totalProjectTime: " << i;
        m_totalProjectTime += it.second->part->getBuildTime();
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

    // Calculate project time
    m_totalProjectTime = 0;
    for(auto& it: getParts())
    {
        int i = it.second->part->getBuildTime();
        log() << "Adding time to totalProjectTime: " << i;
        m_totalProjectTime += it.second->part->getBuildTime();
    }

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
        { A13GameplayObjectManager::items.aluminum, 100 },
        { A13GameplayObjectManager::items.titanium, 50 },
        { A13GameplayObjectManager::items.gold, 25 }
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
