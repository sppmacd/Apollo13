#pragma once

#include "Container.h"

namespace A13
{

class ItemCarrier
{
public:
    ItemCarrier() {}

    // Get the internal storage.
    Container& getContainer() { return m_container; }
    const Container& getContainer() const { return m_container; }

private:
    Container m_container;
};

}
