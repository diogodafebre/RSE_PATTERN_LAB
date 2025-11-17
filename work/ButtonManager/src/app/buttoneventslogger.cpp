#include "buttoneventslogger.h"
#include "trace/trace.h"

namespace app
{

ButtonEventsLogger::ButtonEventsLogger()
{
    Trace::out("ButtonEventsLogger: Created");
}

ButtonEventsLogger::~ButtonEventsLogger()
{
}

void ButtonEventsLogger::onButtonShortPressed(ButtonIndex buttonIndex)
{
    Trace::out(">>> ButtonEventsLogger: Button %d SHORT PRESSED <<<", buttonIndex);
}

void ButtonEventsLogger::onButtonLongPressed(ButtonIndex buttonIndex)
{
    Trace::out(">>> ButtonEventsLogger: Button %d LONG PRESSED <<<", buttonIndex);
}

} // namespace app
