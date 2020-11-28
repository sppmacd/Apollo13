#pragma once

#include "A13GUIAbstractBuilder.h"
#include "Builder/BuilderItem.h"
#include "Builder/BuilderPart.h"
#include "Entities/Entity.h"
#include "ResourceItem.h"
#include "ResourceItems/Container.h"
#include "FactoryTilemap.h"

#include <ege/gpo/GameplayObject.h>
#include <ege/util/Vector.h>
#include <algorithm>

class A13GUIFactoryBuilder;

class A13FactoryBuilding : public EGE::GameplayObject
{
public:
    A13FactoryBuilding(EGE::String id)
    : EGE::GameplayObject(id) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 0}; }
    virtual EGE::Vec2u getSize() const { return {1, 1}; }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap>) { return true; }

    // Used for deserialization.
    virtual EGE::SharedPtr<A13::FactoryBuildingPart> makePart() const
    {
        return make<A13::FactoryBuildingPart>(this);
    }

    virtual std::string getTooltip(const A13::FactoryTilemap*, EGE::Vec2i, const A13::FactoryTilemap::StateType&) const
    {
        return getId() + "\n";
    }

    virtual std::string getDescription()
    {
        return "(No description)";
    }

    virtual std::string getLabel() { return "(No label assigned)"; }

    virtual Cost getCost() const
    {
        return {};
    }

    // Returns if object can be placed here.
    virtual CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13::FactoryTilemap::TileType& tile) const
    {
        return !tile.part ? CanPlaceHere::Yes : CanPlaceHere::No;
    }
};

#define A13_CUSTOM_FACTORY_PART(clazz) \
    virtual EGE::SharedPtr<A13::FactoryBuildingPart> makePart() const \
    { \
        return make<clazz>(this); \
    } \

class A13FactoryBuildingItem : public EGE::GameplayObject, public BuilderItem<A13::FactoryTilemap>
{
public:
    A13FactoryBuildingItem(std::string id, A13FactoryBuilding* building = nullptr)
    : EGE::GameplayObject(id), m_building(building) {}

    virtual EGE::Vec2d getAtlasPosition(int) const
    {
        return m_building ? m_building->getItemAtlasPosition() : EGE::Vec2d(0, 0);
    }

    // Return item that is displayed on highlight. It't also placed by
    // default handler if it's not replaced.
    virtual EGE::SharedPtr<A13::FactoryBuildingPart> getPart() const
    {
        return m_building ? m_building->makePart() : nullptr;
    }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i tileRel, const A13::FactoryTilemap::TileType& tile) const
    {
        return m_building ? m_building->canPlaceHere(tileRel, tile) : CanPlaceHere::NotLoaded;
    }

    virtual Cost getCost() const
    {
        return m_building ? m_building->getCost() : Cost();
    }

    virtual std::string getDescription()
    {
        return m_building ? m_building->getDescription() : "(No building assigned)";
    }

    virtual std::string getLabel() { return m_building->getLabel(); }

    virtual bool onPlace(A13::FactoryTilemap* tilemap, int meta, EGE::Vec2i partPos) const;

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; };
    virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap>) { return true; };

private:
    A13FactoryBuilding* m_building;
};

class A13FactoryBuildingRocketFactory : public A13FactoryBuilding
{
public:
    A13FactoryBuildingRocketFactory()
    : A13FactoryBuilding("a13:rocket_factory") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {1, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 1}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }

    virtual std::string getLabel() { return "Rocket Factory"; }

    struct Part : public A13::FactoryBuildingPart
    {
        Part(const A13FactoryBuilding* bld)
        : A13::FactoryBuildingPart(bld) {}

        virtual void onActivate(A13::FactoryTilemap*, EGE::Vec2i);

        virtual std::string getTooltip(const A13::FactoryTilemap* tilemap, EGE::Vec2i pos, const A13::FactoryTilemap::StateType& state) const
        {
            return A13::FactoryBuildingPart::getTooltip(tilemap, pos, state) + "\nClick to open Project Builder";
        }

        virtual void onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i, EGE::TickCount tickCount);
    };

    virtual std::string getDescription()
    {
        return
        "Rocket Factory is needed to\n"
        "create rocket project. Launching\n"
        "working rocket is necessary to win\n"
        "game. You also get points for\n"
        "included parts\n";
    }

    virtual Cost getCost() const;

    A13_CUSTOM_FACTORY_PART(Part);
};

class A13FactoryBuildingStartPlatform : public A13FactoryBuilding
{
public:
    A13FactoryBuildingStartPlatform()
    : A13FactoryBuilding("a13:start_platform") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {5, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {0, 2}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }

    virtual std::string getLabel() { return "Start Platform"; }

    struct Part : public A13::FactoryBuildingPart
    {
        Part(const A13FactoryBuilding* bld)
        : A13::FactoryBuildingPart(bld) {}

        virtual void onActivate(A13::FactoryTilemap*, EGE::Vec2i);

        virtual std::string getTooltip(const A13::FactoryTilemap* tilemap, EGE::Vec2i pos, const A13::FactoryTilemap::StateType& state) const
        {
            return A13::FactoryBuildingPart::getTooltip(tilemap, pos, state) + "\nClick to launch rocket";
        }

        virtual void onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i, EGE::TickCount tickCount) override;
        virtual void render(A13GUIFactoryBuilder* gui, EGE::Vec2i pos, sf::RenderTarget& target) const override;
    };

    virtual Cost getCost() const;

    virtual std::string getDescription()
    {
        return
        "Use Start Platform to launch your\n"
        "rocket!";
    }

    A13_CUSTOM_FACTORY_PART(Part);
};

namespace A13
{
    class Recipe;
}

class A13FactoryBuildingFactory : public A13FactoryBuilding
{
public:
    A13FactoryBuildingFactory(std::string suffix = "generic")
    : A13FactoryBuilding("a13:factory:" + suffix) {}

    struct Container : public A13::Container
    {
    public:
        virtual ResourceItemStack allowLoadItem(ResourceItemStack stack)
        {
            return stack;
        }
    };

    class Part : public A13::FactoryBuildingPart
    {
    public:
        Part(const A13FactoryBuilding* bld)
        : A13::FactoryBuildingPart(bld, std::make_unique<Container>()) {}

        virtual std::string getTooltip(const A13::FactoryTilemap* tilemap, EGE::Vec2i pos, const A13::FactoryTilemap::StateType& state) const;
        virtual void onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i, EGE::TickCount tickCount);

        virtual bool onPlace(A13::FactoryTilemap* tilemap, EGE::Vec2i partPos)
        {
            if(!A13::FactoryBuildingPart::onPlace(tilemap, partPos))
                return false;

            // TODO: maybe separate storage for each input?
            container->getInventory().setMaxItemCount(100);

            return true;
        }

        virtual void setRecipe(A13::Recipe* recipe);

        virtual A13::Recipe* getRecipe()
        {
            return m_recipe;
        }

        virtual EGE::SharedPtr<EGE::ObjectMap> serialize();
        virtual bool deserialize(EGE::SharedPtr<EGE::ObjectMap> obj);

        virtual void onActivate(A13::FactoryTilemap*, EGE::Vec2i);

        virtual void render(A13GUIFactoryBuilder* gui, EGE::Vec2i pos, sf::RenderTarget& target) const override;

        int nextRandomTime = 0;
        EGE::TickCount lastCrafting = 0;
        bool m_error = false;
    private:
        A13::Recipe* m_recipe = nullptr;
    };

    A13_CUSTOM_FACTORY_PART(Part);

    virtual EGE::Vec2d getAtlasPosition() const { return {1, 4}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {1, 1}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }
    virtual EGE::TickCount getCraftingDuration() const { return 60; }

    virtual std::string getLabel() { return "Factory"; }

    virtual Cost getCost() const;

    virtual std::string getDescription()
    {
        return
        "Factory is a building used to\n"
        "craft new items!";
    }
};

class A13FactoryBuildingQuickFactory : public A13FactoryBuildingFactory
{
public:
    A13FactoryBuildingQuickFactory()
    : A13FactoryBuildingFactory("quick") {}

    virtual EGE::Vec2d getAtlasPosition() const override { return {0, 3}; }
    virtual EGE::Vec2d getItemAtlasPosition() const override { return {3, 1}; }
    virtual EGE::Vec2u getSize() const override { return {1, 1}; }
    virtual EGE::TickCount getCraftingDuration() const override { return 180; }

    virtual Cost getCost() const override;

    virtual std::string getDescription() override
    {
        return
        "The Quick Factory is a factory that you\n"
        "place as the first building. It\n"
        "can be used also when you don't\n"
        "have resources";
    }
    virtual std::string getLabel() { return "Quick Factory"; }
};

class A13FactoryBuildingMine : public A13FactoryBuilding
{
public:
    A13FactoryBuildingMine(EGE::Size level)
    : A13FactoryBuilding("a13:mine:" + std::to_string(level)), m_level(level) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {9 + m_level * 4, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {m_level, 4}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i pos, const A13::FactoryTilemap::StateType& state) const
    {
        A13::Ore* ore = (A13::Ore*)&state.addObjs[FACTORY_BUILDER_LAYER_ORES];
        CanPlaceHere cph = A13FactoryBuilding::canPlaceHere(pos, state);
        if(ore->id != ORE_NONE)
            return cph == CanPlaceHere::Yes ? CanPlaceHere::Match : CanPlaceHere::No;
        return cph == CanPlaceHere::Yes ? CanPlaceHere::Restricted : CanPlaceHere::No;
    }

    virtual double getMultiplier() const
    {
        return (m_level + 1) * 1.2;
    }

    virtual EGE::Size getBufferSize() const
    {
        return 10;
    }

    struct Container : public A13::Container
    {
    public:
        virtual ResourceItemStack allowLoadItem(ResourceItemStack stack)
        {
            log() << "check if item needed";
            if(stack.getItem() && stack.getItem()->getId() == "a13:coal_ore")
                return stack;
            return {};
        }
    };

    class Part : public A13::FactoryBuildingPart
    {
    public:
        Part(const A13FactoryBuilding* bld)
        : A13::FactoryBuildingPart(bld, std::make_unique<Container>()) {}

        virtual std::string getTooltip(const A13::FactoryTilemap* tilemap, EGE::Vec2i pos, const A13::FactoryTilemap::StateType& state) const
        {
            return A13::FactoryBuildingPart::getTooltip(tilemap, pos, state)
                + "\nLevel " + std::to_string(((A13FactoryBuildingMine*)building)->m_level) + "\n"
                + std::to_string(fuelContainer.getInventory().getItemCount()) + " items in internal storage\n";
        }

        virtual bool onPlace(A13::FactoryTilemap* tilemap, EGE::Vec2i partPos)
        {
            if(!A13::FactoryBuildingPart::onPlace(tilemap, partPos))
                return false;

            container->getInventory().setMaxItemCount(((A13FactoryBuildingMine*)building)->getBufferSize());

            for(EGE::Size x = 0; x < getSize().x; x++)
            for(EGE::Size y = 0; y < getSize().y; y++)
            {
                auto& tile = tilemap->ensureTile({x + partPos.x, y + partPos.y});
                A13::Ore* ore = (A13::Ore*)&tile.addObjs[FACTORY_BUILDER_LAYER_ORES];
                if(ore->id != ORE_NONE && ore->count > 0)
                    orePos.push_back({x + partPos.x, y + partPos.y});
            }

            multiplier = ((A13FactoryBuildingMine*)building)->getMultiplier();

            return true;
        }

        virtual void render(A13GUIFactoryBuilder* gui, EGE::Vec2i pos, sf::RenderTarget& target) const override;

        virtual void onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i, EGE::TickCount tickCount);

        virtual EGE::SharedPtr<EGE::ObjectMap> serialize();
        bool deserialize(EGE::SharedPtr<EGE::ObjectMap> obj);

        EGE::Vector<EGE::Vec2i> orePos;
        int nextRandomTime = 0;
        EGE::TickCount lastOre = 0;
        double multiplier = 1;
        int m_fuel = 1;
        A13::Container fuelContainer;
        bool m_requestedCoal = false;
    };

    A13_CUSTOM_FACTORY_PART(Part);

    virtual Cost getCost() const;

    virtual std::string getDescription()
    {
        return
        "Mines allows you to get resources.\n"
        "The higher is level, the more\n"
        "resources it mines.";
    }
    virtual std::string getLabel() { return "Mine (Level " + std::to_string(m_level) + ")"; }

private:
    EGE::Size m_level;
};

class A13FactoryBuildingQuickMine : public A13FactoryBuildingMine
{
public:
    A13FactoryBuildingQuickMine()
    : A13FactoryBuildingMine(9999) {}

    virtual EGE::Vec2d getAtlasPosition() const override { return {0, 2}; }
    virtual EGE::Vec2d getItemAtlasPosition() const override { return {2, 1}; }
    virtual EGE::Vec2u getSize() const override { return {1, 1}; }
    virtual EGE::Size getBufferSize() const override { return 2; }
    virtual double getMultiplier() const override { return 0.9; }

    virtual Cost getCost() const override;

    virtual std::string getDescription() override
    {
        return
        "The Quick Mine is a mine that you\n"
        "place as the first building. It\n"
        "can be used also when you don't\n"
        "have resources";
    }

    virtual std::string getLabel() { return "Quick Mine"; }
};

class A13FactoryBuildingPortal : public A13FactoryBuilding
{
public:
    A13FactoryBuildingPortal(EGE::Size level)
    : A13FactoryBuilding("a13:mine:" + std::to_string(level)), m_level(level) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {9 + m_level * 4, 0}; }
    virtual EGE::Vec2d getItemAtlasPosition() const { return {m_level, 4}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }

    class Part : public A13::FactoryBuildingPart
    {
    public:
        Part(const A13FactoryBuilding* bld)
        : A13::FactoryBuildingPart(bld, std::make_unique<A13::Container>()) {}

        virtual std::string getTooltip(const A13::FactoryTilemap* tilemap, EGE::Vec2i pos, const A13::FactoryTilemap::StateType& state) const
        {
            return A13::FactoryBuildingPart::getTooltip(tilemap, pos, state)
                + std::to_string(container->getInventory().getItemCount()) + " items in internal storage\n";
        }

        //virtual void onUpdate(A13::FactoryTilemap* tilemap, EGE::Vec2i, EGE::TickCount tickCount);
    };

    A13_CUSTOM_FACTORY_PART(Part);

    virtual Cost getCost() const;

    virtual std::string getDescription()
    {
        return
        "Mines allows you to get resources.\n"
        "The higher is level, the more\n"
        "resources it mines.";
    }

private:
    EGE::Size m_level;
};

class A13FactoryBuildingItemRoad : public A13FactoryBuildingItem
{
public:
    A13FactoryBuildingItemRoad(EGE::Size index, EGE::Size placedTerrain)
    : A13FactoryBuildingItem("a13:building_road:" + std::to_string(index)), m_index(index), m_placed(placedTerrain) {}

    struct Part : public A13::FactoryBuildingPart
    {
        Part()
        : A13::FactoryBuildingPart(nullptr) {}

        virtual EGE::Vec2u getSize() const { return {1, 1}; }
        virtual EGE::Vec2d getAtlasPosition(int) const { return {0, 1}; }
        virtual int getHighlightLayer() { return FACTORY_BUILDER_LAYER_TERRAIN; }
    };

    virtual EGE::Vec2d getAtlasPosition(int) const { return {m_index, 3}; }

    virtual bool onPlace(A13::FactoryTilemap* tilemap, int meta, EGE::Vec2i partPos) const
    {
        if(A13FactoryBuildingItem::onPlace(tilemap, meta, partPos))
            return true; // We don't have enough coins!

        tilemap->ensureTile(partPos).addObjs[FACTORY_BUILDER_LAYER_TERRAIN] = m_placed;
        return true;
    }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i pos, const A13::FactoryTilemap::StateType& state) const
    {
        if(state.addObjs[FACTORY_BUILDER_LAYER_TERRAIN] == m_placed)
            return CanPlaceHere::Restricted;
        return CanPlaceHere::Yes;
    }

    virtual EGE::SharedPtr<A13::FactoryBuildingPart> getPart() const
    {
        return make<Part>();
    }

    virtual Cost getCost() const;

    virtual std::string getDescription()
    {
        return
        "Roads are decorative items which\n"
        "you can place on terrain, below\n"
        "regular buildings.";
    }
    virtual std::string getLabel() { return "Road"; }

private:
    EGE::Size m_index;
    EGE::Size m_placed;
};

class A13FactoryBuildingItemRobotSpawner : public A13FactoryBuildingItem
{
public:
    A13FactoryBuildingItemRobotSpawner()
    : A13FactoryBuildingItem("a13:robot_spawner") {}

    struct Part : public A13::FactoryBuildingPart
    {
        Part()
        : A13::FactoryBuildingPart(nullptr) {}

        virtual EGE::Vec2u getSize() const { return {1, 1}; }

        virtual EGE::Vec2d getAtlasPosition(int meta) const
        {
            return {0, 1};
        }
    };

    virtual EGE::Vec2d getAtlasPosition(int) const { return {0, 6}; }

    virtual bool onPlace(A13::FactoryTilemap* tilemap, int meta, EGE::Vec2i partPos) const
    {
        auto carrier = make<A13::EntityItemCarrier>(tilemap->scene, tilemap);
        carrier->setPosition(sf::Vector2f(partPos.x * 16 + 8, partPos.y * 16 + 8));
        tilemap->scene->addObject(carrier);
        return true;
    }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i, const A13::FactoryTilemap::StateType&) const
    {
        return CanPlaceHere::Yes;
    }

    virtual EGE::SharedPtr<A13::FactoryBuildingPart> getPart() const
    {
        return make<Part>();
    }

    virtual std::string getDescription()
    {
        return
        "The Robots are used for transferring items.";
    }
};

class A13FactoryBuildingItemLogistic : public A13FactoryBuildingItem
{
public:
    A13FactoryBuildingItemLogistic(int mode)
    : A13FactoryBuildingItem("a13:logistic:" + std::to_string(mode)), m_mode(mode) {}

    static int modeMeta2Index(int current, int mode, int meta)
    {
        switch(mode)
        {
            case 0: return current | (0b1 << (meta % 8));
            case 1: return current | (0b1 << (meta % 4)) | (0b10000 << ((meta + 2) % 4));
            default: return current;
        }
    }

    struct Part : public A13::FactoryBuildingPart
    {
        int m_mode = 0;

        Part(int mode)
        : A13::FactoryBuildingPart(nullptr), m_mode(mode) {}

        virtual EGE::Vec2u getSize() const { return {1, 1}; }

        virtual EGE::Vec2d getAtlasPosition(int meta) const
        {
            int index = modeMeta2Index(0, m_mode, meta);
            return {index % 16, index / 16};
        }

        virtual int getHighlightLayer() { return FACTORY_BUILDER_LAYER_LOGISTIC; }
    };

    virtual EGE::Vec2d getAtlasPosition(int) const { return {m_mode, 5}; }

    virtual bool onPlace(A13::FactoryTilemap* tilemap, int meta, EGE::Vec2i partPos) const
    {
        int current = tilemap->ensureTile(partPos).addObjs[FACTORY_BUILDER_LAYER_LOGISTIC];
        tilemap->ensureTile(partPos).addObjs[FACTORY_BUILDER_LAYER_LOGISTIC] = modeMeta2Index(current, m_mode, meta);
        return true;
    }

    virtual CanPlaceHere canPlaceHere(EGE::Vec2i pos, const A13::FactoryTilemap::StateType& state) const
    {
        if(state.part)
            return CanPlaceHere::No;
        if(state.addObjs[FACTORY_BUILDER_LAYER_LOGISTIC])
            return CanPlaceHere::Match;
        return CanPlaceHere::Yes;
    }

    virtual EGE::SharedPtr<A13::FactoryBuildingPart> getPart() const
    {
        return make<Part>(m_mode);
    }

    virtual std::string getDescription()
    {
        return
        "This item allows you to define\n"
        "logistic network. The item carriers\n"
        "will follow the arrows.";
    }

    int m_mode = 0;
};
