#include "trace/trace.h"
#include "board/board.h"
#include "board/buttonscontroller.h"
#include "mdw/button/buttoneventshandler.h"
#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    #include <lvgl.h>
    #include <lvgl_input_device.h>
    #include <zephyr/kernel.h>
    #include <zephyr/drivers/display.h>
#endif // CONFIG_LVGL
#if defined(CONFIG_DISK_ACCESS) && (CONFIG_DISK_ACCESS != 0)
    #include <zephyr/storage/disk_access.h>
    #include <zephyr/fs/fs.h>
    #include <ff.h>
#endif // CONFIG_DISK_ACCESS
#include "factory.h"

namespace app
{

ButtonEventsLogger * Factory::pButtonEventsLogger_ = nullptr;

Factory::Factory()
{

}

// static
void Factory::initialize()
{
    Trace::initialize();
    board::initialize();

    Trace::out("Factory: Initializing app components...");

    // Create ButtonEventsLogger
    pButtonEventsLogger_ = new ButtonEventsLogger();

    // Subscribe ButtonEventsLogger to ButtonEventsHandler
    bool success = getButtonEventsHandler().subscribe(&getButtonEventsLogger());
    assert(success && "Failed to subscribe ButtonEventsLogger");

    // Register callback from ButtonsController to ButtonEventsHandler
    success = getButtonsController().registerCallback(
        &getButtonEventsHandler(),
        static_cast<interface::ButtonsControllerCallbackProvider::CallbackMethod>(
            &ButtonEventsHandler::onButtonChanged
        )
    );
    assert(success && "Failed to register ButtonsController callback");

    Trace::out("Factory: Button system initialized");

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    getGuiTask().initialize(getButtonEventsHandler());
#endif
}

// static
void Factory::build()
{
    Trace::out("Factory: Starting app components...");

    // Start ButtonEventsHandler and ButtonsController state machines
    getButtonsController().start();
    getButtonEventsHandler().start();

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    getGuiTask().start();
#endif

    Trace::out("Factory: App ready");
}

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
// static
GuiTask & Factory::getGuiTask()
{
    static GuiTask guiTask;
    return guiTask;
}
#endif // CONFIG_LVGL

// static
ButtonEventsLogger & Factory::getButtonEventsLogger()
{
    assert(pButtonEventsLogger_ && "ButtonEventsLogger not initialized");
    return *pButtonEventsLogger_;
}

// static
ButtonEventsHandler & Factory::getButtonEventsHandler()
{
    static ButtonEventsHandler buttonEventsHandler;
    return buttonEventsHandler;
}

// static
ButtonsController & Factory::getButtonsController()
{
    static ButtonsController buttonsController;
    return buttonsController;
}

} /* namespace app */

void Factory_initialize()
{
    app::Factory::initialize();
}

void Factory_build()
{
    app::Factory::build();
}
