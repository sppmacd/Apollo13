#include "Save.h"

#include "PlayerStats.h"

#include <fstream>

namespace A13
{

bool Save::load(std::string name)
{
    m_fileName = name;
    if(!loadFromFile(m_fileName))
    {
        log() << "Initializing and saving new world " << name;
        initialize();
        saveToFile(m_fileName);
        return false;
    }
    return true;
}

bool Save::save(std::string name)
{
    if(!name.empty())
        m_fileName = name;
    return saveToFile(m_fileName);
}

EGE::SharedPtr<EGE::ObjectMap> Save::serialize()
{
    log() << "Serializing save data";
    auto obj = make<EGE::ObjectMap>();

    // PlayerStats
    obj->addObject("PS", m_playerStats.serialize());

    // Seed
    obj->addInt("Seed", m_seed);

    // Tilemap
    obj->addObject("World", m_tilemap->serialize());

    // Project tilemap
    obj->addObject("Project", m_projectTilemap->serialize());

    return obj;
}

bool Save::deserialize(EGE::SharedPtr<EGE::ObjectMap> obj)
{
    log() << "Deserializing save data";

    // PlayerStats
    auto _PS = obj->getObject("PS");
    if(_PS.expired() || !_PS.lock()->isMap())
        PRINT_ERROR_AND_RETURN("Not found PS key in base.json");
    if(!m_playerStats.deserialize(std::dynamic_pointer_cast<EGE::ObjectMap>(_PS.lock())))
        PRINT_ERROR_AND_RETURN("Invalid PS key in base.json");

    // Seed
    auto _Seed = obj->getObject("Seed");
    if(_Seed.expired() || !_Seed.lock()->isInt())
        m_seed = 0;
    else
        m_seed = _Seed.lock()->asInt();

    // Tilemap
    auto _World = obj->getObject("World");
    if(_World.expired() || !_World.lock()->isMap())
        PRINT_ERROR_AND_RETURN("Not found World key in base.json");
    m_tilemap = make<A13::FactoryTilemap>(m_seed);
    if(!m_tilemap->deserialize(std::dynamic_pointer_cast<EGE::ObjectMap>(_World.lock())))
        PRINT_ERROR_AND_RETURN("Invalid world key in base.json");

    // Project tilemap
    auto _Project = obj->getObject("Project");
    if(_Project.expired() || !_Project.lock()->isMap())
        PRINT_ERROR_AND_RETURN("Not found Project key in base.json");
    m_projectTilemap = make<A13ProjectTilemap>();
    if(!m_projectTilemap->deserialize(std::dynamic_pointer_cast<EGE::ObjectMap>(_Project.lock())))
        PRINT_ERROR_AND_RETURN("Invalid Project key in base.json");
    return true;
}

void Save::initialize()
{
    m_playerStats.initialize();
    m_seed = EGE::System::unixTime();
    m_tilemap = make<A13::FactoryTilemap>(m_seed);
    m_projectTilemap = make<A13ProjectTilemap>();
}

bool Save::loadFromFile(std::string name)
{
    log() << "Loading world from file " << name;

    // Base file (JSON)
    std::ifstream file("saves/" + name + "/base.json");

    if(!file.good())
        return false;

    std::shared_ptr<EGE::Object> object;
    if(!(file >> EGE::objectIn(object, EGE::JSONConverter())))
    {
        log(LogLevel::Error) << "Failed to parse world base file!";
        return false;
    }

    if(!object->isMap())
    {
        log(LogLevel::Error) << "World base is not a map!";
        return false;
    }

    if(!deserialize(std::dynamic_pointer_cast<EGE::ObjectMap>(object)))
    {
        log(LogLevel::Error) << "Failed to deserialize world base file!";
        return false;
    }

    // Chunks are lazy-loaded in generator
    return true;
}

bool Save::saveToFile(std::string name)
{
    log() << "Saving world to file " << name;

    // Base file (JSON)
    std::shared_ptr<EGE::ObjectMap> object = serialize();
    if(!object)
    {
        log(LogLevel::Error) << "Failed to serialize world base file!";
        return false;
    }
    std::ofstream file("saves/" + name + "/base.json");
    if(!(file << EGE::objectOut(*object.get(), EGE::JSONConverter())))
    {
        log(LogLevel::Error) << "Failed to generate world base file!";
        return false;
    }

    // Chunks
    if(!m_tilemap->flush(name))
        return false;

    return true;
}

}
