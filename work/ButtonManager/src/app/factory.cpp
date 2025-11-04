#include "trace/trace.h"
#include "board/board.h"
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

// TODO: Define static factory attributes here

Factory::Factory()
{

}

// static
void Factory::initialize()
{
    Trace::initialize();
    board::initialize();

    Trace::out("Factory: Initializing app components...");

    // TODO: Initialize factory attributes here
    
#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    getGuiTask().initialize(_buttonEventsHandler);
#endif
}

// static
void Factory::build()
{
    Trace::out("Factory: Starting app components...");

    // Start state machine(s)
    // TODO: Start state-machines here

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

} /* namespace app */

void Factory_initialize()
{
    app::Factory::initialize();
}

void Factory_build()
{
    app::Factory::build();
}
