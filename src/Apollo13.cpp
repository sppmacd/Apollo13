#include "Apollo13.h"
#include "A13GameplayObjectManager.h"
#include "A13GUIProjectBuilder.h"
#include "A13GUIFactoryBuilder.h"
#include "PlayerStats.h"

#include <ege/gui/GUIResourceManager.h>

EGE::EventResult Apollo13::load()
{
    // Setup GPO
    if(!A13GameplayObjectManager::instance().reload())
        return EGE::EventResult::Failure;

    A13::PlayerStats::instance().initialize();

    // Setup ResourceManager
    auto resMan = make<EGE::GUIResourceManager>();

    resMan->registerDefaultFont("font.ttf");

    resMan->registerTexture("game/resource_items.png");
    resMan->registerTexture("game/entities.png");

    // GUI
    resMan->registerTexture("gui/pb/rocket_parts.png");
    resMan->registerTexture("gui/pb/background.png");

    resMan->registerTexture("gui/factory_builder/background.png");
    resMan->registerTexture("gui/factory_builder/builder_atlas.png");
    resMan->registerTexture("gui/factory_builder/items.png");
    resMan->registerTexture("gui/factory_builder/terrain.png");
    resMan->registerTexture("gui/factory_builder/ores.png");
    resMan->registerTexture("gui/factory_builder/shadows.png");
    resMan->registerTexture("gui/factory_builder/logistic.png");

    setResourceManager(resMan);

    // Setup window
    getWindow().lock()->setFramerateLimit(60);

    // Setup world
    m_seed = time(EGE::Time::Unit::Seconds);

    // Set GUI
    setCurrentGUIScreen(make<A13GUIFactoryBuilder>(this));

    return EGE::EventResult::Success;
}

void Apollo13::logicTick(long long tickCount)
{
    A13::PlayerStats::instance().update();
}
