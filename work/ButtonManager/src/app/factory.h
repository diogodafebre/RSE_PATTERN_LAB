#ifndef APP_FACTORY_H
#define APP_FACTORY_H

//
// What is seen only by the C++ compiler
//
#ifdef __cplusplus

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    #include "gui-task.h"
#endif

#include "buttoneventslogger.h"

class ButtonEventsHandler;
class ButtonsController;

namespace app
{

/**
 * @brief Application factory responsible to create needed objects.
 */
class Factory
{
public:
    Factory();

    static void initialize();           ///< Initializes the factory
    static void build();                ///< Creates components and initializes relations

#if defined(CONFIG_LVGL) && (CONFIG_LVGL != 0)
    static GuiTask & getGuiTask();
#endif

    static ButtonEventsLogger & getButtonEventsLogger();
    static ButtonEventsHandler & getButtonEventsHandler();
    static ButtonsController & getButtonsController();

protected:
    static ButtonEventsLogger * pButtonEventsLogger_;
};

} /* namespace app */
#endif // __cplusplus

//
// What is seen by the C and C++ compiler
//
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void Factory_initialize();
void Factory_build();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // APP_FACTORY_H
