#include "buttonstatesm.h"
#include "trace/trace.h"

ButtonStateSm::ButtonStateSm(uint16_t buttonIndex, Callback * callback) :
    XFBehavior(/* active = */ false),  // Not an active behavior (no own thread)
    state_(STATE_IDLE),
    buttonIndex_(buttonIndex),
    callback_(callback)
{
}

void ButtonStateSm::onButtonChanged(bool pressed)
{
    if (pressed)
    {
        pushEvent(evButtonPressed);
    }
    else
    {
        pushEvent(evButtonReleased);
    }
}

XFEventStatus ButtonStateSm::processEvent()
{
    const XFEvent * currentEvent = getCurrentEvent();
    int eventId = currentEvent->getId();

    switch (state_)
    {
    case STATE_IDLE:
        if (currentEvent->getEventType() == XFEvent::Initial)
        {
            Trace::out("ButtonStateSm[%d]: Initial state", buttonIndex_);
            state_ = STATE_IDLE;
            return XFEventStatus::Consumed;
        }
        else if (eventId == evButtonPressed)
        {
            Trace::out("ButtonStateSm[%d]: Button pressed, waiting for release or timeout", buttonIndex_);
            state_ = STATE_PRESSED;
            // Start timeout for long press detection
            pushEvent(evLongPressTimeout, LONG_PRESS_TIMEOUT_MS);
            return XFEventStatus::Consumed;
        }
        break;

    case STATE_PRESSED:
        if (eventId == evButtonReleased)
        {
            Trace::out("ButtonStateSm[%d]: Button released before timeout - SHORT PRESS", buttonIndex_);
            state_ = STATE_SHORT_PRESSED;
            // Notify callback about short press
            if (callback_)
            {
                callback_->onButtonShortPressed(buttonIndex_);
            }
            // Return to idle
            state_ = STATE_IDLE;
            return XFEventStatus::Consumed;
        }
        else if (eventId == evLongPressTimeout)
        {
            Trace::out("ButtonStateSm[%d]: Timeout reached while pressed - LONG PRESS", buttonIndex_);
            state_ = STATE_LONG_PRESSED;
            // Notify callback about long press
            if (callback_)
            {
                callback_->onButtonLongPressed(buttonIndex_);
            }
            return XFEventStatus::Consumed;
        }
        break;

    case STATE_LONG_PRESSED:
        if (eventId == evButtonReleased)
        {
            Trace::out("ButtonStateSm[%d]: Button released after long press", buttonIndex_);
            state_ = STATE_IDLE;
            return XFEventStatus::Consumed;
        }
        break;

    case STATE_SHORT_PRESSED:
        // This state is transient, should not receive events
        state_ = STATE_IDLE;
        break;

    default:
        break;
    }

    return XFEventStatus::Consumed;
}
