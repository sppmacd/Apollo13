#pragma once

#include "TileMapObject.h"

#include <ege/gpo/GameplayObjectRegistry.h>
#include <ege/gui/Widget.h>

// Widget for Part Selection (left panel)
template<class _Tilemap, class _Item>
class PartSelectWidget : public EGE::Widget
{
public:
    PartSelectWidget(EGE::Widget* parent, TileMapObject<_Tilemap>* tmObj)
    : EGE::Widget(parent), m_tmObject(tmObj)
    {
    }

    void onLoad();
    void onMouseButtonRelease(sf::Event::MouseButtonEvent& event);
    void renderOnly(sf::RenderTarget& target, const EGE::RenderStates& states = {});

    EGE::Vec2u getPartSize() const { if(m_index == -1) return {}; return m_items[m_index]->getSize(); }

    // -1 for no item
    void setCurrentItemIndex(int index) { m_index = index; }
    _Item* getCurrentItem() { return m_index == -1 ? nullptr : m_items[m_index]; }
    int* getCurrentItemIndex() { return m_index; }

    std::string m_partAtlasTextureName;
    EGE::GameplayObjectRegistry<EGE::String, _Item>* m_gpo = nullptr;

private:
    EGE::Vector<_Item*> m_items;
    sf::Texture* m_atlas = nullptr;
    TileMapObject<_Tilemap>* m_tmObject;
    int m_index = -1;
};

#include "PartSelectWidget.inl"
