#include "FactoryTilemap.h"

#include "A13FactoryBuilding.h"
#include "PlayerStats.h"

#include <fstream>
#include <sstream>

namespace A13
{

EGE::Vec2d FactoryBuildingPart::getAtlasPosition(int) const
{
    return building ? building->getAtlasPosition() : EGE::Vec2d(0, 0);
}

EGE::Vec2u FactoryBuildingPart::getSize() const
{
    return building ? building->getSize() : EGE::Vec2u(0, 0);
}

std::string FactoryBuildingPart::getTooltip(const FactoryTilemap* tilemap, EGE::Vec2i pos, const FactoryBuildingPart::StateType& state) const
{
    return building ? building->getTooltip(tilemap, pos, state) + "\n" : "???\n";
}

bool FactoryBuildingPart::onRemove(FactoryTilemap*, EGE::Vec2i)
{
    if(building)
    {
        // Give back some of the resources to player.
        const double RESOURCE_GIVEBACK_FACTOR = 0.8;

        Cost cost = building->getCost();
        for(ResourceItemStack& stack: cost)
        {
            PlayerStats::instance().getInventory()[stack.getItem()->getId()] += stack.getItemCount() * RESOURCE_GIVEBACK_FACTOR;
        }
    }
    return true;
}

EGE::SharedPtr<EGE::ObjectMap> FactoryBuildingPart::serialize()
{
    auto obj = make<EGE::ObjectMap>();
    if(container)
    {
        auto _container = container->serialize();
        obj->addObject("container", _container);
    }
    return obj;
}

bool FactoryBuildingPart::deserialize(EGE::SharedPtr<EGE::ObjectMap> obj)
{
    auto _container = obj->getObject("container");
    if(container && !_container.expired())
    {
        return container->deserialize(std::static_pointer_cast<EGE::ObjectMap>(_container.lock()));
    }
    return true;
}

std::string FactoryTilemap::getTooltip(EGE::Vec2i pos, const FactoryTilemap::StateType& state) const
{
    // TODO: use localization!
    std::string terrain = "Terrain: " + std::to_string(state.addObjs[FACTORY_BUILDER_LAYER_TERRAIN]) + "\n";
    Ore* _ore = (Ore*)&state.addObjs[FACTORY_BUILDER_LAYER_ORES];
    std::string ore = _ore->id ? ("Ore: " + std::to_string(_ore->count) + "x #" + std::to_string(_ore->id) + "\n") : "";
    EGE::Uint8 _logistic = state.addObjs[FACTORY_BUILDER_LAYER_LOGISTIC];
    std::string logistic = _logistic ? ("Logistic: " + std::to_string(_logistic) + "\n") : "";
    std::string tile = (state.part ? state.part->getTooltip(this, pos - EGE::Vec2i(state.cornerPos), state) : "No object") + "\n";
    return terrain + ore + tile + logistic;
}

bool FactoryTilemap::onRemove(EGE::Vec2i partPos)
{
    auto& tile = ensureTile(partPos);
    if(tile.addObjs[FACTORY_BUILDER_LAYER_LOGISTIC])
    {
        tile.addObjs[FACTORY_BUILDER_LAYER_LOGISTIC] = 0;
        return true;
    }
    return false;
}

EGE::SharedPtr<EGE::ObjectMap> FactoryTilemap::serialize()
{
    log() << "Saving part info for factory tilemap";
    EGE::SharedPtr<EGE::ObjectMap> obj = make<EGE::ObjectMap>();
    EGE::SharedPtr<EGE::ObjectList> _parts = make<EGE::ObjectList>();

    for(auto& obj: m_parts)
    {
        auto _part = make<EGE::ObjectMap>();
        _part->addObject("coords", EGE::Serializers::fromVector2(obj.first));
        _part->addObject("data", obj.second->serialize());

        if(obj.second->building)
            _part->addString("id", obj.second->building->getId());

        _parts->addObject(_part);
    }

    obj->addObject("parts", _parts);
    return obj;
}

bool FactoryTilemap::deserialize(EGE::SharedPtr<EGE::ObjectMap> obj)
{
    log() << "Loading part info for factory tilemap";
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

        A13FactoryBuilding* bld = A13GameplayObjectManager::instance().factoryBuildings.findById(_part_id->asString());
        if(!bld)
            return false;

        auto ipart = bld->makePart();
        if(!_part_data->isMap())
            return false;
        ipart->deserialize(std::dynamic_pointer_cast<EGE::ObjectMap>(_part_data));
        if(!_part_coords->isMap())
            return false;
        m_parts[EGE::Serializers::toVector2(std::dynamic_pointer_cast<EGE::ObjectMap>(_part_coords))] = ipart;
    }
    return true;
}

// Read/write chunks from/to stream
bool FactoryTilemap::writeTo(EGE::Vec2i chunkCoords, ChunkType& chunk, std::ostream& buf)
{
    // TODO: network ordering

    // Magic: version and chunk info
    buf << "A13" << (unsigned char)16 << (unsigned char)16 << (unsigned char)4;

    // Chunk data
    for(EGE::Size x = 0; x < 16; x++)
    for(EGE::Size y = 0; y < 16; y++)
    {
        A13::FactoryTilemap::StateType& state = chunk.getTile({x, y});

        // Part coords
        EGE::Int32 partx = chunkCoords.x * 16 + (int)x - (int)state.cornerPos.x;
        EGE::Int32 party = chunkCoords.y * 16 + (int)y - (int)state.cornerPos.y;
        //log() << "PART " << partx << "," << party;
        buf.write((char*)&partx, 4);
        buf.write((char*)&party, 4);

        // Additional layers
        for(EGE::Uint64 al: state.addObjs)
        {
            buf.write((char*)&al, 8);
            //log() << "AL " << al;
        }

        // Meta
        buf.write((char*)&state.meta, 4);
        //log() << "META " << state.meta;
    }
    return true;
}

bool FactoryTilemap::readFrom(EGE::Vec2i chunkCoords, ChunkType& chunk, std::istream& buf)
{
    // TODO: network ordering

    bool success = true;

    // Magic: version and chunk info
    char ver[4] = "   ";
    unsigned char cx, cy, alc; // chunk size x,y, additional layer count, new line
    success &= (bool)buf.read(ver, 3);
    success &= (bool)(buf >> cx >> cy >> alc);
    if(!success || std::string(ver) != "A13" || cx != 16 || cy != 16 || alc != 4)
    {
        log(LogLevel::Warning) << "Invalid chunk magic!";
        return false;
    }

    // Chunk data
    for(EGE::Size x = 0; x < 16; x++)
    for(EGE::Size y = 0; y < 16; y++)
    {
        A13::FactoryTilemap::StateType& state = chunk.getTile({x, y});

        // Part coords
        EGE::Int32 partx, ourx = chunkCoords.x * 16 + (int)x;
        EGE::Int32 party, oury = chunkCoords.y * 16 + (int)y;
        if(!buf.read((char*)&partx, 4)) return false;
        if(!buf.read((char*)&party, 4)) return false;
        //log() << "PART " << partx << "," << party;
        state.cornerPos.x = (unsigned)(ourx - partx);
        state.cornerPos.y = (unsigned)(oury - party);

        auto _part = m_parts.find({partx, party});
        if(_part != m_parts.end())
        {
            state.part = _part->second.get();
            //log() << "&PART " << state.part;
        }
        else
        {
            state.part = nullptr;
            //log() << "&PART nullptr";
        }

        // Additional layers
        for(EGE::Uint64& al: state.addObjs)
        {
            if(!buf.read((char*)&al, 8)) return false;
            //log() << "AL " << al;
        }

        // Meta
        if(!buf.read((char*)&state.meta, 4)) return false;
        //log() << "META " << state.meta;
    }

    return true;
}

bool FactoryTilemap::writeTo(EGE::Vec2i chunkCoord, FactoryTilemap::ChunkType& chunk, std::string worldName)
{
    //log(LogLevel::Verbose) << "[FT] Saving chunk " <<  chunkCoord.x << "," << chunkCoord.y;
    std::ostringstream oss;
    oss << "saves/" << worldName << "/chunk_" << chunkCoord.x << "_" << chunkCoord.y << ".a13c";
    std::ofstream file(oss.str(), std::ios::binary);
    if(!file.good())
        return false;
    return writeTo(chunkCoord, chunk, file);
}

bool FactoryTilemap::readFrom(EGE::Vec2i chunkCoord, FactoryTilemap::ChunkType& chunk, std::string worldName)
{
    //log(LogLevel::Verbose) << "[FT] Loading chunk " <<  chunkCoord.x << "," << chunkCoord.y;
    std::ostringstream oss;
    oss << "saves/" << worldName << "/chunk_" << chunkCoord.x << "_" << chunkCoord.y << ".a13c";
    std::ifstream file(oss.str(), std::ios::binary);
    if(!file.good() || !readFrom(chunkCoord, chunk, file))
    {
        generateChunk(chunkCoord, chunk);
        return writeTo(chunkCoord, chunk, worldName);
    }
    return true;
}

bool FactoryTilemap::flush(std::string worldName)
{
    log() << "Flushing tilemap for save " << worldName;
    for(auto& it: *this)
    {
        auto chunk = it.second.get();
        if(chunk)
        {
            if(!writeTo(it.first, *chunk, worldName))
                return false;
        }
    }
    return true;
}

void FactoryTilemap::generateChunk(EGE::Vec2i chunkPos, A13::FactoryTilemap::ChunkType& chunk)
{
    //log(LogLevel::Verbose) << "[FT] Generating chunk " <<  chunkPos.x << "," << chunkPos.y;
    EGE::Random random(chunkPos.x * 1024 * 1024 + chunkPos.y * 1024 + seed);

    // TODO: replace it by better generator
    for(EGE::Size x = 0; x < 16; x++)
    for(EGE::Size y = 0; y < 16; y++)
    {
        A13::FactoryTilemap::StateType& state = chunk.getTile({x, y});
        state.addObjs[FACTORY_BUILDER_LAYER_TERRAIN] = random.nextInt(5) ? TERRAIN_GRASS : TERRAIN_WILD_GRASS;
    }

    // ores
    if(random.nextInt(10) == 1)
    {
        EGE::Vector<int> ores;

        while(ores.empty())
        {
            // try each ore

            if(random.nextInt(5) == 0)
                ores.push_back(ORE_COAL);
            if(random.nextInt(6) == 0)
                ores.push_back(ORE_IRON);
            if(random.nextInt(6) == 0)
                ores.push_back(ORE_COPPER);
            if(random.nextInt(20) == 0)
                ores.push_back(ORE_TITANIUM);
            if(random.nextInt(10) == 0)
                ores.push_back(ORE_SILICON);
            if(random.nextInt(8) == 0)
                ores.push_back(ORE_ALUMINUM);
            if(random.nextInt(30) == 0)
                ores.push_back(ORE_DIAMOND);
            if(random.nextInt(35) == 0)
                ores.push_back(ORE_GOLD);
            if(random.nextInt(35) == 0)
                ores.push_back(ORE_SILVER);
        }

        int id = ores[random.nextInt(ores.size())];

        EGE::Vec2i pos(random.nextInt(16),
            random.nextInt(16));

        for(int s = random.nextIntRanged(20, 50); s >= 0; s--)
        {
            switch(random.nextInt(4))
            {
                case 0: pos.x++; break;
                case 1: pos.x--; break;
                case 2: pos.y++; break;
                case 3: pos.y--; break;
            }

            const A13::FactoryTilemap::StateType& state = ensureTile({chunkPos.x * 16 + pos.x, chunkPos.y * 16 + pos.y});
            A13::Ore* ore = (A13::Ore*)&state.addObjs[FACTORY_BUILDER_LAYER_ORES];

            if(ore->id != ORE_NONE)
            {
                s++;
                continue;
            }

            ore->id = id;
            ore->count = random.nextIntRanged(512, MAX_ORE_COUNT);
        }
    }
}

}
