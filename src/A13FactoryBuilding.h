#pragma once

#include "BuilderPart.h"

#include <ege/gpo/GameplayObject.h>
#include <ege/util/Vector.h>

class A13FactoryBuilding : public EGE::GameplayObject, public BuilderPart
{
public:
    A13FactoryBuilding(EGE::String id)
    : EGE::GameplayObject(id) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 0}; }
    virtual EGE::Vec2u getSize() const { return {1, 1}; }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {}
};

class A13FactoryBuildingRocketFactory : public A13FactoryBuilding
{
public:
    A13FactoryBuildingRocketFactory()
    : A13FactoryBuilding("a13:rocket_factory") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {1, 0}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }
};

class A13FactoryBuildingStartPlatform : public A13FactoryBuilding
{
public:
    A13FactoryBuildingStartPlatform()
    : A13FactoryBuilding("a13:start_platform") {}

    virtual EGE::Vec2d getAtlasPosition() const { return {5, 0}; }
    virtual EGE::Vec2u getSize() const { return {4, 4}; }
};

class A13FactoryBuildingRoad : public A13FactoryBuilding
{
public:
    A13FactoryBuildingRoad(EGE::Size level)
    : A13FactoryBuilding("a13:building_road:" + std::to_string(level)), m_level(level) {}

    virtual EGE::Vec2d getAtlasPosition() const { return {0, 1 + m_level}; }
    virtual EGE::Vec2u getSize() const { return {1, 1}; }

private:
    EGE::Size m_level;
};
