#pragma once

#include <ege/gpo/GameplayObject.h>
#include <ege/util/Vector.h>

class A13RocketPart : public EGE::GameplayObject
{
public:
    A13RocketPart(EGE::String id)
    : EGE::GameplayObject(id) {}

    virtual EGE::Vec2d getAtlasPosition() { return {0, 1}; }
    virtual EGE::Vec2u getSize() { return {2, 2}; }

    virtual EGE::SharedPtr<EGE::ObjectMap> serialize() { return nullptr; }
    virtual void deserialize(EGE::SharedPtr<EGE::ObjectMap>) {}
};
