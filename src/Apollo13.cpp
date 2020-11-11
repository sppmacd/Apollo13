#include "Apollo13.h"
#include "A13GUIProjectBuilder.h"

#include <ege/gui/GUIResourceManager.h>

EGE::EventResult Apollo13::load()
{
    // Setup ResourceManager
    auto resMan = make<EGE::GUIResourceManager>();

    resMan->registerDefaultFont("font.ttf");
    resMan->registerTexture("rocket_parts.png");

    setResourceManager(resMan);

    setCurrentGUIScreen(make<A13GUIProjectBuilder>(this));

    return EGE::EventResult::Success;
}
