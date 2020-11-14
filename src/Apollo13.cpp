#include "Apollo13.h"
#include "A13GameplayObjectManager.h"
#include "A13GUIProjectBuilder.h"
#include "A13GUIFactoryBuilder.h"

#include <ege/gui/GUIResourceManager.h>

EGE::EventResult Apollo13::load()
{
    // Setup GPO
    if(!A13GameplayObjectManager::instance().reload())
        return EGE::EventResult::Failure;

    // Setup ResourceManager
    auto resMan = make<EGE::GUIResourceManager>();

    resMan->registerDefaultFont("font.ttf");

    // GUI
    resMan->registerTexture("gui/pb/rocket_parts.png");
    resMan->registerTexture("gui/pb/background.png");

    resMan->registerTexture("gui/factory_builder/background.png");
    resMan->registerTexture("gui/factory_builder/builder_atlas.png");
    resMan->registerTexture("gui/factory_builder/items.png");
    resMan->registerTexture("gui/factory_builder/terrain.png");
    resMan->registerTexture("gui/factory_builder/shadows.png");

    setResourceManager(resMan);

    // Set GUI
    setCurrentGUIScreen(make<A13GUIFactoryBuilder>(this));

    return EGE::EventResult::Success;
}
