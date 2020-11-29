#pragma once

#include <ege/util.h>

#include "A13RocketPart.h"
#include "FactoryTilemap.h"
#include "PlayerStats.h"

#define PRINT_ERROR_AND_RETURN(msg) {log(LogLevel::Error) << "Save load failed: " << msg; return false;}

namespace A13
{

class Save : public EGE::Serializable
{
public:
    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() override;
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap>) override;

    void initialize();
    bool load(std::string name);
    bool save(std::string name = "");
    void remove(std::string name = "main");
    void unload();

    bool loaded() { return m_loaded; }

    PlayerStats& playerStats() { return m_playerStats; }
    EGE::SharedPtr<A13::FactoryTilemap> tilemap() { return m_tilemap; }
    EGE::MaxInt seed() { return m_seed; }
    EGE::SharedPtr<A13ProjectTilemap> projectTilemap() { return m_projectTilemap; }

    std::string getFileName() { return m_fileName; }

private:
    bool loadFromFile(std::string name);
    bool saveToFile(std::string name);

    std::string m_fileName;
    PlayerStats m_playerStats;
    EGE::SharedPtr<A13::FactoryTilemap> m_tilemap;
    EGE::MaxInt m_seed = 0;
    EGE::SharedPtr<A13ProjectTilemap> m_projectTilemap;
    bool m_loaded = false;
};

}
