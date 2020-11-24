#pragma once

#include "A13FactoryBuilding.h"
#include "A13RocketPart.h"
#include "ResourceItem.h"

#include <ege/gpo/GameplayObjectManager.h>
#include <ege/gpo/GameplayObjectRegistry.h>
#include <ege/util/Types.h>

namespace A13
{
    struct Recipe
    {
        EGE::Vector<ResourceItemStack> input;
        ResourceItemStack output;

        Recipe(EGE::Vector<ResourceItemStack> _input, ResourceItemStack _output)
        : input(_input), output(_output) {}
    };
}

class A13GameplayObjectManager : public EGE::GameplayObjectManager
{
public:
    EGE_SINGLETON(A13GameplayObjectManager);

    bool load()
    {
        log() << "[A13GameplayObjectManager] Loaded";
        registerRocketParts();
        registerFactoryBuildings();
        registerResourceItems();
        registerRecipes();
        return true;
    }

    void registerRocketParts();
    void registerFactoryBuildings();
    void registerResourceItems();
    void registerRecipes();

    void registerRocketPart(EGE::UniquePtr<A13RocketPart> part);
    void registerFactoryBuilding(EGE::UniquePtr<A13FactoryBuilding> part);

    void registerFactoryBuildingItem(EGE::UniquePtr<A13FactoryBuildingItem> part);

    void registerResourceItem(EGE::UniquePtr<ResourceItem> item);

    void registerRecipe(EGE::UniquePtr<A13::Recipe> recipe);

    bool clear()
    {
        rocketParts.clear();
        rocketPartItems.clear();
        factoryBuildings.clear();
        factoryBuildingItems.clear();
        resourceItems.clear();
        recipes.clear();
        return true;
    }

    static class _ResourceItems
    {
    public:
        ResourceItem* coal_ore;
        ResourceItem* iron_ore;
        ResourceItem* iron;
        ResourceItem* copper_ore;
        ResourceItem* copper;
        ResourceItem* titanium_ore;
        ResourceItem* titanium;
        ResourceItem* silicon_sand;
        ResourceItem* silicon;
        ResourceItem* aluminum_ore;
        ResourceItem* aluminum;
        ResourceItem* diamond_ore;
        ResourceItem* diamond;
        ResourceItem* gold_ore;
        ResourceItem* gold;
        ResourceItem* silver_ore;
        ResourceItem* silver;
    } items;

    // Rocket parts (that are placed)
    EGE::GameplayObjectRegistry<EGE::String, A13RocketPart> rocketParts;

    // Items for Rocket Parts (that are in left panel)
    EGE::GameplayObjectRegistry<EGE::String, A13RocketPartItem> rocketPartItems;

    // Factory Building (that are placed)
    EGE::GameplayObjectRegistry<EGE::String, A13FactoryBuilding> factoryBuildings;

    // Items for Factory Buildings (that are in left panel)
    EGE::GameplayObjectRegistry<EGE::String, A13FactoryBuildingItem> factoryBuildingItems;

    // Resource Items (stone, coal, wood etc.)
    EGE::GameplayObjectRegistry<EGE::String, ResourceItem> resourceItems;

    // Recipes for Factory FB
    EGE::GameplayObjectRegistry<ResourceItem*, A13::Recipe> recipes;
};
