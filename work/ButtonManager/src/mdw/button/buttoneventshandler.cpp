#include "buttoneventshandler.h"
#include "interface/buttoneventshandlerobserver.h"
#include "trace/trace.h"
#include "xf/customevent.h"

/**
 * @brief Internal custom event holding button index as attribute.
 */
class evButtonInternal : public XFCustomEvent
{
public:
    evButtonInternal(uint16_t buttonIndex, int eventId, interface::XFBehavior * pBehavior) :
        XFCustomEvent(eventId, pBehavior),
        buttonIndex(buttonIndex)
    {}

    uint16_t buttonIndex;
};

ButtonEventsHandler::ButtonEventsHandler() :
    XFBehavior(/* active = */ false),  // Not an active behavior
    observerCount_(0)
{
    // Initialize button state machines
    for (uint16_t i = 0; i < MAX_BUTTONS; i++)
    {
        buttonStateSm_[i] = new ButtonStateSm(i, this);
    }

    // Initialize observers array
    for (uint16_t i = 0; i < MAX_OBSERVERS; i++)
    {
        observers_[i] = nullptr;
    }
}

ButtonEventsHandler::~ButtonEventsHandler()
{
    // Clean up button state machines
    for (uint16_t i = 0; i < MAX_BUTTONS; i++)
    {
        if (buttonStateSm_[i])
        {
            delete buttonStateSm_[i];
            buttonStateSm_[i] = nullptr;
        }
    }
}

void ButtonEventsHandler::start()
{
    startBehavior();

    // Start all button state machines
    for (uint16_t i = 0; i < MAX_BUTTONS; i++)
    {
        if (buttonStateSm_[i])
        {
            buttonStateSm_[i]->startBehavior();
        }
    }

    Trace::out("ButtonEventsHandler: Started");
}

bool ButtonEventsHandler::subscribe(interface::ButtonEventsHandlerObserver * observer)
{
    if (!observer)
    {
        return false;
    }

    // Check if already subscribed
    for (uint16_t i = 0; i < observerCount_; i++)
    {
        if (observers_[i] == observer)
        {
            return true;  // Already subscribed
        }
    }

    // Add new observer if space available
    if (observerCount_ < MAX_OBSERVERS)
    {
        observers_[observerCount_] = observer;
        observerCount_++;
        Trace::out("ButtonEventsHandler: Observer subscribed (total: %d)", observerCount_);
        return true;
    }

    Trace::out("ButtonEventsHandler: Failed to subscribe observer - max observers reached");
    return false;
}

void ButtonEventsHandler::unsubscribe(interface::ButtonEventsHandlerObserver * observer)
{
    if (!observer)
    {
        return;
    }

    // Find and remove observer
    for (uint16_t i = 0; i < observerCount_; i++)
    {
        if (observers_[i] == observer)
        {
            // Shift remaining observers down
            for (uint16_t j = i; j < observerCount_ - 1; j++)
            {
                observers_[j] = observers_[j + 1];
            }
            observers_[observerCount_ - 1] = nullptr;
            observerCount_--;
            Trace::out("ButtonEventsHandler: Observer unsubscribed (total: %d)", observerCount_);
            return;
        }
    }
}

void ButtonEventsHandler::onButtonChanged(uint16_t buttonIndex, bool pressed)
{
    if (buttonIndex < MAX_BUTTONS && buttonStateSm_[buttonIndex])
    {
        // Trace removed to save stack - happens too frequently
        buttonStateSm_[buttonIndex]->onButtonChanged(pressed);
    }
}

void ButtonEventsHandler::onButtonShortPressed(uint16_t buttonIndex)
{
    // Trace::out("ButtonEventsHandler: Button %d SHORT", buttonIndex);
    // Push internal event to decouple the call
    GEN(evButtonInternal(buttonIndex, evButtonShortPressedInternal, this));
}

void ButtonEventsHandler::onButtonLongPressed(uint16_t buttonIndex)
{
    // Trace::out("ButtonEventsHandler: Button %d LONG", buttonIndex);
    // Push internal event to decouple the call
    GEN(evButtonInternal(buttonIndex, evButtonLongPressedInternal, this));
}

XFEventStatus ButtonEventsHandler::processEvent()
{
    const XFEvent * currentEvent = getCurrentEvent();

    if (currentEvent->getEventType() == XFEvent::Initial)
    {
        // Trace::out("ButtonEventsHandler: Initial");
        return XFEventStatus::Consumed;
    }

    if (currentEvent->getEventType() == XFEvent::Event)
    {
        int eventId = currentEvent->getId();

        switch (eventId)
        {
        case evButtonShortPressedInternal:
            {
                const evButtonInternal * event = static_cast<const evButtonInternal *>(currentEvent);
                // Trace::out("ButtonEventsHandler: Short[%d]", event->buttonIndex);
                notifyButtonShortPressed(event->buttonIndex);
            }
            return XFEventStatus::Consumed;

        case evButtonLongPressedInternal:
            {
                const evButtonInternal * event = static_cast<const evButtonInternal *>(currentEvent);
                // Trace::out("ButtonEventsHandler: Long[%d]", event->buttonIndex);
                notifyButtonLongPressed(event->buttonIndex);
            }
            return XFEventStatus::Consumed;

        default:
            break;
        }
    }

    return XFEventStatus::Consumed;
}

void ButtonEventsHandler::notifyButtonShortPressed(ButtonIndex buttonIndex)
{
    // Trace removed to save stack
    for (uint16_t i = 0; i < observerCount_; i++)
    {
        if (observers_[i])
        {
            observers_[i]->onButtonShortPressed(buttonIndex);
        }
    }
}

void ButtonEventsHandler::notifyButtonLongPressed(ButtonIndex buttonIndex)
{
    // Trace removed to save stack
    for (uint16_t i = 0; i < observerCount_; i++)
    {
        if (observers_[i])
        {
            observers_[i]->onButtonLongPressed(buttonIndex);
        }
    }
}
