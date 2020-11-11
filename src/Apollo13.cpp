#include "Apollo13.h"
#include "A13GameplayObjectManager.h"
#include "A13GUIProjectBuilder.h"

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

    setResourceManager(resMan);

    // Set GUI
    setCurrentGUIScreen(make<A13GUIProjectBuilder>(this));

    return EGE::EventResult::Success;
}
