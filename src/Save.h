#pragma once

#include <ege/util.h>

#include "FactoryTilemap.h"
#include "PlayerStats.h"

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

    PlayerStats& playerStats() { return m_playerStats; }
    EGE::SharedPtr<A13::FactoryTilemap> tilemap() { return m_tilemap; }
    EGE::MaxInt seed() { return m_seed; }

    std::string getFileName() { return m_fileName; }

private:
    bool loadFromFile(std::string name);
    bool saveToFile(std::string name);

    std::string m_fileName;
    PlayerStats m_playerStats;
    EGE::SharedPtr<A13::FactoryTilemap> m_tilemap;
    EGE::MaxInt m_seed = 0;
};

}
