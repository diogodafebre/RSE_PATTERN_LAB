#ifndef APP_BUTTONEVENTSLOGGER_H
#define APP_BUTTONEVENTSLOGGER_H

#include "interface/buttoneventshandlerobserver.h"

namespace app
{

/**
 * @brief Logger for button events.
 *
 * This class observes button events and logs them using the trace facility.
 */
class ButtonEventsLogger : public interface::ButtonEventsHandlerObserver
{
public:
    ButtonEventsLogger();
    virtual ~ButtonEventsLogger();

protected:
    // ButtonEventsHandlerObserver interface
    void onButtonShortPressed(ButtonIndex buttonIndex) override;
    void onButtonLongPressed(ButtonIndex buttonIndex) override;
};

} // namespace app

#endif // APP_BUTTONEVENTSLOGGER_H
