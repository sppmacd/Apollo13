#pragma once

#include <ege/gpo/GameplayObject.h>
#include <ege/util/Vector.h>

class A13RocketPart : public EGE::GameplayObject
{
public:
    A13RocketPart(EGE::String id)
    : EGE::GameplayObject(id) {}

    virtual EGE::Vec2d getAtlasPosition() { return {0, 0}; }
    virtual EGE::Vec2u getSize() { return {1, 1}; }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {}
};

class A13RocketPartEngine : public A13RocketPart
{
public:
    A13RocketPartEngine()
    : A13RocketPart("a13:engine:generic") {}

    virtual EGE::Vec2d getAtlasPosition() { return {0, 1}; }
    virtual EGE::Vec2u getSize() { return {2, 2}; }
};

class A13RocketPartFuelTank : public A13RocketPart
{
public:
    A13RocketPartFuelTank(EGE::Size size)
    : A13RocketPart("a13:fuel_tank:generic:" + std::to_string(size)), m_size(size) {}

    virtual EGE::Vec2d getAtlasPosition() { return {2, 1}; }
    virtual EGE::Vec2u getSize() { return {2, m_size}; }

private:
    EGE::Size m_size;
};

class A13RocketPartCone : public A13RocketPart
{
public:
    A13RocketPartCone()
    : A13RocketPart("a13:cone:generic:") {}

    virtual EGE::Vec2d getAtlasPosition() { return {0, 5}; }
    virtual EGE::Vec2u getSize() { return {2, 2}; }

private:
    EGE::Size m_size;
};

class A13RocketPartCapsule : public A13RocketPart
{
public:
    A13RocketPartCapsule()
    : A13RocketPart("a13:capsule:generic") {}

    virtual EGE::Vec2d getAtlasPosition() { return {0, 3}; }
    virtual EGE::Vec2u getSize() { return {2, 2}; }
};
