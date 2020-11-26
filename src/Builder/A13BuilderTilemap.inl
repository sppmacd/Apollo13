// INLINE

template <class _Tilemap, class _PartType, EGE::Size _AddLayerCount>
void A13BuilderTilemap<_Tilemap, _PartType, _AddLayerCount>::placePart(EGE::Vec2i tileRel, int meta, EGE::SharedPtr<StateType> part)
{
    EGE::Vec2u partSize = part->getSize();

    // Check if part can be placed here
    if(!canPartBePlacedHere(tileRel, partSize) || !part->onPlace(this, tileRel))
        return;

    auto it = getParts().insert(std::make_pair(EGE::Vec2i(tileRel.x, tileRel.y), part));

    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = (useEnsure() ? &ensureTile({x + tileRel.x, y + tileRel.y}) : getTile({x + tileRel.x, y + tileRel.y}));

        // Sanity check
        ASSERT(tile);

        tile->part = it.first->second.get();
        tile->cornerPos = {x, y};
        tile->meta = meta;
    }
}

template <class _Tilemap, class _PartType, EGE::Size _AddLayerCount>
void A13BuilderTilemap<_Tilemap, _PartType, _AddLayerCount>::removePart(EGE::Vec2i tileRel)
{
    // Check if tilemap doesn't have its own remove handler for that tile
    if(onRemove(tileRel))
        return;

    // Check which object is here
    auto tile = (useEnsure() ? &ensureTile({tileRel.x, tileRel.y}) : getTile({tileRel.x, tileRel.y}));
    if(!tile)
        return; // Out of bounds!

    EGE::FlatPtr<typename _Tilemap::TileType::PartType> part = tile->part;
    if(!part)
        return; // Nothing is here!

    // Calculate left-top coordinate
    EGE::Vec2i objectPos = tileRel - EGE::Vec2i(tile->cornerPos);

    // Get object iterator
    auto it = getParts().find(objectPos);
    if(it == getParts().end())
    {
        err() << "Can't find object at " << objectPos.x << "," << objectPos.y;
        return;
    }

    EGE::Vec2u partSize = tile->part->getSize();

    // Check if we want it object to be removed
    if(!it->second->onRemove(et(), objectPos))
        return;

    // Remove that object
    getParts().erase(it);

    // Clear tilemap
    for(EGE::Uint32 x = 0; x < partSize.x; x++)
    for(EGE::Uint32 y = 0; y < partSize.y; y++)
    {
        auto tile = (useEnsure() ? &ensureTile({x + objectPos.x, y + objectPos.y}) : getTile({x + objectPos.x, y + objectPos.y}));
        if(!tile)
            return;

        tile->part = nullptr;
        tile->cornerPos = {0, 0};
    }
}
