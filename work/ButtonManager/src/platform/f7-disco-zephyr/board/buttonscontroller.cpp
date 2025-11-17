#include "buttonscontroller.h"
#include "hal/buttons.h"
#include "trace/trace.h"

ButtonsController * ButtonsController::pInstance_ = nullptr;

ButtonsController::ButtonsController() :
    XFBehavior(/* active = */ false),  // Not an active behavior
    callbackProvider_(nullptr),
    callbackMethod_(nullptr)
{
    assert(!pInstance_);  // Only one instance of this class allowed!
    pInstance_ = this;

    // Initialize previous button states to not pressed
    for (uint16_t i = 0; i < BUTTON_COUNT; i++)
    {
        previousButtonState_[i] = false;
    }
}

ButtonsController::~ButtonsController()
{
    pInstance_ = nullptr;
}

void ButtonsController::start()
{
    startBehavior();
    Trace::out("ButtonsController: Started");
}

void ButtonsController::onIrq()
{
    // Called by ISR when any button changes state
    // Push event to trigger button polling
    pushEvent(evButtonIrq);
}

bool ButtonsController::registerCallback(interface::ButtonsControllerCallbackProvider * callbackProvider,
                                         interface::ButtonsControllerCallbackProvider::CallbackMethod callbackMethod)
{
    if (!callbackProvider || !callbackMethod)
    {
        Trace::out("ButtonsController: Failed to register callback - invalid parameters");
        return false;
    }

    callbackProvider_ = callbackProvider;
    callbackMethod_ = callbackMethod;

    Trace::out("ButtonsController: Callback registered successfully");
    return true;
}

XFEventStatus ButtonsController::processEvent()
{
    const XFEvent * currentEvent = getCurrentEvent();

    if (currentEvent->getEventType() == XFEvent::Initial)
    {
        Trace::out("ButtonsController: Initial state - starting periodic polling");
        // Start periodic polling
        pushEvent(evPollButtons, POLL_INTERVAL_MS);
        return XFEventStatus::Consumed;
    }

    if (currentEvent->getEventType() == XFEvent::Event)
    {
        int eventId = currentEvent->getId();

        switch (eventId)
        {
        case evPollButtons:
            // Poll buttons and schedule next poll
            pollButtons();
            pushEvent(evPollButtons, POLL_INTERVAL_MS);
            return XFEventStatus::Consumed;

        case evButtonIrq:
            // ISR triggered - poll buttons immediately
            Trace::out("ButtonsController: IRQ received - polling buttons");
            pollButtons();
            return XFEventStatus::Consumed;

        default:
            break;
        }
    }

    return XFEventStatus::Consumed;
}

void ButtonsController::pollButtons()
{
    for (uint16_t i = 0; i < BUTTON_COUNT; i++)
    {
        bool currentState = isButtonPressed(i);

        // Check if state changed
        if (currentState != previousButtonState_[i])
        {
            Trace::out("ButtonsController: Button %d state changed to %s",
                      i, currentState ? "PRESSED" : "RELEASED");

            // Update previous state
            previousButtonState_[i] = currentState;

            // Call registered callback if available
            if (callbackProvider_ && callbackMethod_)
            {
                (callbackProvider_->*callbackMethod_)(i, currentState);
            }
        }
    }
}

bool ButtonsController::isButtonPressed(uint16_t buttonIndex)
{
    if (buttonIndex >= BUTTON_COUNT)
    {
        return false;
    }

    return board::hal::buttons::isButtonPressed(buttonIndex);
}
