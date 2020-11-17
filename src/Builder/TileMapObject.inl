// INLINE

template<class _Tilemap>
TileMapObject<_Tilemap>::TileMapObject(EGE::SharedPtr<EGE::Scene> scene, std::array<std::string, _Tilemap::TileType::AdditionalLayerCount + 1>& texs,  EGE::SharedPtr<_Tilemap> tilemap)
: EGE::SceneObject2D(scene, "a13:builder:tilemap"), m_builderPartAtlasTextureNames(texs), m_tilemap(tilemap)
{
    // Set tilemap
    m_tilemap = tilemap;

    // Set renderer
    auto renderer = make<EGE::TilemapRenderer2D<_Tilemap>>(scene, m_tilemap);

    renderer->setLayerCount(_Tilemap::TileType::AdditionalLayerCount + 1);
    for(EGE::Size s = 0; s < texs.size(); s++)
        renderer->setAtlasTextureName(m_builderPartAtlasTextureNames[s], s);

    renderer->setTileAtlasMapper([this](const typename _Tilemap::TileType& state, EGE::Size layer)->EGE::Vec2d
    {
        switch(layer)
        {
        case _Tilemap::TileType::AdditionalLayerCount:
            {
                if(!state.part)
                    return EGE::Vec2d(0, 0);

                EGE::Vec2d atlasPos = state.part->getAtlasPosition();
                auto tileSize = m_tilemap->getTileSize();
                return EGE::Vec2d(atlasPos.x * tileSize.x + state.cornerPos.x * tileSize.x,
                        atlasPos.y * tileSize.y + state.cornerPos.y * tileSize.y);
            } break;
        default:
            return callCustomLayerAtlasMapper(state, layer);
        }
    });
    setRenderer(renderer);

    setPosition({0, 0});
}

template<class _Tilemap>
bool TileMapObject<_Tilemap>::isObjectInArea(EGE::Vec2i position, EGE::Vec2u range) const
{
    for(EGE::Uint32 x = 0; x < range.x; x++)
    for(EGE::Uint32 y = 0; y < range.y; y++)
    {
        auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({x + position.x, y + position.y}) : m_tilemap->getTile({x + position.x, y + position.y}));
        if(!tile)
            return true; // Out of bounds!
        if(tile->part)
            return true; // Something is already here :(
    }
    return false;
}

template<class _Tilemap>
EGE::Vec2i TileMapObject<_Tilemap>::mapTilePosition(sf::Vector2f scenePos) const
{
    sf::Vector2f relPos = scenePos - getPosition();
    EGE::Vec2i tileRel = m_tilemap->getTileAlignedPos({(relPos.x), (relPos.y)});
    return tileRel;
}

template<class _Tilemap>
bool TileMapObject<_Tilemap>::canPartBePlacedHere(EGE::Vec2i tileRel, EGE::Vec2u partSize) const
{
    // Check if objects can be placed here
    if(m_placePredicate)
    {
        // Don't allow placing on CanPlaceHere::No tiles
        bool isRed = false;

        // Don't allow placing if all tiles are CanPlaceHere::Restricted
        bool isAllYellow = true;

        for(EGE::Size x = 0; x < partSize.x; x++)
        for(EGE::Size y = 0; y < partSize.y; y++)
        {
            CanPlaceHere cph = m_placePredicate(tileRel + EGE::Vec2i(x, y));
            isRed |= (cph == CanPlaceHere::No || cph == CanPlaceHere::NotLoaded);

            if((cph == CanPlaceHere::Match || cph == CanPlaceHere::Yes) && isAllYellow)
                isAllYellow = false;
        }
        return !isRed && !isAllYellow;
    }

    // Default handler - just check if there are no objects here
    return !isObjectInArea(tileRel, partSize);
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::placePart(EGE::Vec2i tileRel, EGE::SharedPtr<typename _Tilemap::TileType::PartType> part)
{
    EGE::Vec2u partSize = part->getSize();

    // Check if part can be placed here
    if(!canPartBePlacedHere(tileRel, partSize) || !part->onPlace(m_tilemap.get(), tileRel))
        return;

    auto it = m_objects.insert(std::make_pair(EGE::Vec2i(tileRel.x, tileRel.y), part));

    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({x + tileRel.x, y + tileRel.y}) : m_tilemap->getTile({x + tileRel.x, y + tileRel.y}));

        // Sanity check
        ASSERT(tile);

        tile->part = it.first->second.get();
        tile->cornerPos = {x, y};
    }
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::removePart(EGE::Vec2i tileRel)
{
    // Check which object is here
    auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({tileRel.x, tileRel.y}) : m_tilemap->getTile({tileRel.x, tileRel.y}));
    if(!tile)
        return; // Out of bounds!

    EGE::FlatPtr<typename _Tilemap::TileType::PartType> part = tile->part;
    if(!part)
        return; // Nothing is here!

    // Calculate left-top coordinate
    EGE::Vec2i objectPos = tileRel - EGE::Vec2i(tile->cornerPos);

    // Get object iterator
    auto it = m_objects.find(objectPos);
    if(it == m_objects.end())
    {
        err() << "Can't find object at " << objectPos.x << "," << objectPos.y;
        return;
    }

    EGE::Vec2u partSize = tile->part->getSize();

    // Remove that object
    m_objects.erase(it);

    // Clear tilemap
    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({x + objectPos.x, y + objectPos.y}) : m_tilemap->getTile({x + objectPos.x, y + objectPos.y}));
        if(!tile)
            return;

        tile->part = nullptr;
        tile->cornerPos = {0, 0};
    }
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::onActivate(EGE::Vec2i pos)
{
    auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({pos.x, pos.y}) : m_tilemap->getTile({pos.x, pos.y}));
    if(tile && tile->part)
        tile->part->onActivate(m_tilemap.get(), pos);
}

template<class _Tilemap>
std::string TileMapObject<_Tilemap>::getTooltip(EGE::Vec2i pos)
{
    auto tile = (m_tilemap->useEnsure() ? &m_tilemap->ensureTile({pos.x, pos.y}) : m_tilemap->getTile({pos.x, pos.y}));
    return tile ? m_tilemap->getTooltip(pos, *tile) : "Not loaded";
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::render(sf::RenderTarget& target, const EGE::RenderStates& states) const
{
    EGE::Renderer renderer(target);

    // Tilemap
    EGE::SceneObject::render(target, states);

    // Highlight

    // TODO: optimize it
    for(int x = m_highlightPos.x; x < m_highlightPos.x + (int)m_highlightSize.x; x++)
    for(int y = m_highlightPos.y; y < m_highlightPos.y + (int)m_highlightSize.y; y++)
    {
        sf::Color bc;
        if(m_placePredicate)
        {
            CanPlaceHere cph = m_placePredicate({x, y});
            switch(cph)
            {
                case CanPlaceHere::Match: bc = sf::Color::Cyan; break;
                case CanPlaceHere::No: bc = sf::Color::Red; break;
                case CanPlaceHere::NotLoaded: bc = sf::Color::Black; break;
                case CanPlaceHere::Restricted: bc = sf::Color::Yellow; break;
                case CanPlaceHere::Yes: bc = sf::Color::Green; break;
                default: CRASH();
            }
        }
        else
            bc = canPartBePlacedHere({x, y}, m_highlightSize) ? sf::Color::White : sf::Color(128, 128, 128);

        sf::RectangleShape rs(sf::Vector2f(14, 14));
        sf::Vector2f screenPos = sf::Vector2f(x * (int)m_tilemap->getTileSize().x, y * (int)m_tilemap->getTileSize().y) + getPosition();
        rs.setPosition(screenPos + sf::Vector2f(1, 1));
        rs.setFillColor(sf::Color(bc.r, bc.g, bc.b, 64));
        rs.setOutlineColor(sf::Color(bc.r, bc.g, bc.b, 128));
        rs.setOutlineThickness(1);
        target.draw(rs);
    }
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::onUpdate(long long tickCounter)
{
    EGE::SceneObject::onUpdate(tickCounter);
    for(auto& pr: m_objects)
    {
        pr.second->onUpdate(m_tilemap.get(), pr.first, tickCounter);
    }
}

template<class _Tilemap>
void TileMapObject<_Tilemap>::setHighlight(EGE::Vec2i offset, EGE::Vec2u size)
{
    m_highlightPos = offset;
    m_highlightSize = size;
}
