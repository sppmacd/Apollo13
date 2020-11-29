#include "Apollo13.h"
#include "A13GameplayObjectManager.h"
#include "A13GUIProjectBuilder.h"
#include "A13GUIFactoryBuilder.h"
#include "GUIMainMenu.h"
#include "PlayerStats.h"

#include <ege/gui/GUIResourceManager.h>

EGE::EventResult Apollo13::load()
{
    // Setup GPO
    if(!A13GameplayObjectManager::instance().reload())
        return EGE::EventResult::Failure;

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

    // Set GUI
    setCurrentGUIScreen(make<A13::GUIMainMenu>(this));

    return EGE::EventResult::Success;
}

void Apollo13::loadSave(std::string name)
{
    // Load / create world
    save.load("main");

    pause(false);

    // Switch to Factory Builder
    setCurrentGUIScreen(make<A13GUIFactoryBuilder>(this));
}

EGE::EventResult Apollo13::onFinish(int)
{
    if(!save.save())
        return EGE::EventResult::Failure;
    return EGE::EventResult::Success;
}

void Apollo13::logicTick(long long tickCount)
{
    if(save.loaded() && !m_paused)
    {
        save.playerStats().update();
        save.projectTilemap()->update();

        if(tickCount % (60 * 60 * 5) == 0)
        {
            log() << "Auto saving...";
            if(!save.save())
                log(LogLevel::Error) << "Failed to auto save!";
        }
    }
}

void Apollo13::messageBoxHelper(EGE::SharedPtr<EGE::GUIScreen> scr, std::string message, EGE::IdType id, A13::MessageBox::Type type)
{
    if(scr->getDialog())
        messageBoxHelper(scr->getDialog(), message, id, type);
    else
        scr->openDialog(make<A13::MessageBox>(scr.get(), id, message, type));
}

void Apollo13::messageBox(std::string message, EGE::IdType id, A13::MessageBox::Type type)
{
    messageBoxHelper(getCurrentGUIScreen(), message, id, type);
}
