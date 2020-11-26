#include "A13GUIFactoryBuilder.h"

#include "PlayerStats.h"

A13GUIFactoryBuilder::A13GUIFactoryBuilder(EGE::GUIGameLoop* loop)
: A13GUIAbstractBuilder(loop, Apollo13::instance().save.tilemap())
{
    setBuilderBackground("gui/factory_builder/background.png");

    // main layer
    setBuilderAtlas("gui/factory_builder/builder_atlas.png");
    // additional layers
    setBuilderAtlas("gui/factory_builder/terrain.png", FACTORY_BUILDER_LAYER_TERRAIN);
    setBuilderAtlas("gui/factory_builder/ores.png", FACTORY_BUILDER_LAYER_ORES);
    setBuilderAtlas("gui/factory_builder/shadows.png", FACTORY_BUILDER_LAYER_SHADOWS);
    setBuilderAtlas("gui/factory_builder/logistic.png", FACTORY_BUILDER_LAYER_LOGISTIC);

    setSelectorAtlas("gui/factory_builder/items.png");

    setGPO(&A13GameplayObjectManager::instance().factoryBuildingItems);
    setUseEnsure(true);

    m_tilemap->setTileSize(EGE::Vec2u(16, 16));
    m_tilemap->setGenerator(
        [this](EGE::Vec2i chunkPos, A13::FactoryTilemap::ChunkType& chunk) {
            if(!m_tilemap->readFrom(chunkPos, chunk, Apollo13::instance().save.getFileName()))
            {
                log(LogLevel::Error) << "Failed to load chunk " << chunkPos.x << "," << chunkPos.y;
            }
        }
    );
}

void A13GUIFactoryBuilder::onLoad()
{
    A13GUIAbstractBuilder::onLoad();

    // Custom part selector renderer (for description)
    m_partSelector->setCustomRenderer([this](sf::RenderTarget& target, A13FactoryBuildingItem* item)
    {
        EGE::Renderer renderer(target);
        const unsigned HEIGHT = 40;

        // Background
        renderer.renderRectangle(m_partSelector->getSize().x, 0, 230, m_partSelector->getSize().y, sf::Color(0, 0, 0, 127));

        // Item name
        renderer.renderText(m_partSelector->getSize().x + 10, 20, *m_font, item->getId(), 16);

        // Cost
        Cost cost = item->getCost();

        EGE::Size s = 0;
        for(ResourceItemStack& stack: cost)
        {
            std::string str = stack.getItem()->getId() + " x" + std::to_string(stack.getItemCount());

            // Text
            sf::Color color = A13::PlayerStats::instance().getInventory()[stack.getItem()] < stack.getItemCount() ? sf::Color::Red : sf::Color::White;
            renderer.renderText(m_partSelector->getSize().x + 47, HEIGHT * (s + 1) + 20, *m_font, str, 11, color);

            // Icon
            sf::IntRect texRect;
            // TODO: Name that '16' somehow
            texRect.left = stack.getItem()->getAtlasPosition().x * 16;
            texRect.top = stack.getItem()->getAtlasPosition().y * 16;
            texRect.width = 16;
            texRect.height = 16;
            renderer.renderTexturedRectangle(m_partSelector->getSize().x + 10, HEIGHT * (s + 1) + 10, 32, 32, *m_resourceStatsWidget->getAtlas(), texRect);

            s++;
        }

        // Description
        renderer.renderText(m_partSelector->getSize().x + 10, HEIGHT * (s + 1) + 20, *m_font, item->getDescription(), 10, sf::Color(200, 200, 200));
    });

    // Custom part renderer (for in-world indicators)
    m_tileMapObject->setPartRenderer([this](EGE::Vec2i pos, const A13::FactoryBuildingPart& part, sf::RenderTarget& target)
    {
        EGE::Renderer renderer(target);
        const A13FactoryBuilding* bld = part.building;
        if(!bld)
            return;

        // Inventory (if it's a Container)
        if(part.container)
            renderInventory(pos, part.container->getInventory(), target);

        // Custom renderer
        part.render(this, pos, target);
    });

    m_resourceStatsWidget = make<ResourceStatsWidget>(this, A13::PlayerStats::instance().getInventory());
    addWidget(m_resourceStatsWidget);
}
