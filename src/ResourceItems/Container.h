#pragma once

#include "Inventory.h"

namespace A13
{

class Container
{
public:
    enum _NR
    {
        NoRegisterInQueue
    };

    Container();
    Container(_NR) {}

    virtual ~Container();

    // Get the internal storage.
    virtual Inventory& getInventory() { return m_inventory; }
    virtual const Inventory& getInventory() const { return m_inventory; }

    // Transfers items from carrier to internal storage
    // and updates internal state.
    // maxCount - The maximum count of items to transfer
    // (-1 to transfer all items that carrier has and that
    // are needed)
    // Returns - some unstable bool for now, don't rely on it!
    virtual bool loadItemsFrom(Container* carrier, int maxCount = -1);

    // Returns stack that is a "part" of input stack, or
    // %stack if the stack can be fully placed in container.
    virtual ResourceItemStack allowLoadItem(ResourceItemStack stack) { return stack; }

protected:
    Inventory m_inventory;
};

}