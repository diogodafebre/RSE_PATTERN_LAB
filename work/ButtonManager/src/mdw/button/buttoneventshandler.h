#ifndef MDW_BUTTON_BUTTONEVENTSHANDLER_H
#define MDW_BUTTON_BUTTONEVENTSHANDLER_H

#include "interface/buttoneventshandlersubject.h"
#include "board/interface/buttonscontrollercallbackprovider.h"
#include "buttonstatesm.h"
#include "xf/behavior.h"
#include <cstdint>

/**
 * @brief Handles button events and manages button state machines.
 *
 * This class receives button press/release events from the ButtonsController,
 * manages ButtonStateSm instances for each button, and notifies observers
 * about short and long presses.
 */
class ButtonEventsHandler : public XFBehavior,
                            public interface::ButtonEventsHandlerSubject,
                            public interface::ButtonsControllerCallbackProvider,
                            public ButtonStateSm::Callback
{
public:
    ButtonEventsHandler();
    ~ButtonEventsHandler() override;

    void start();

    // ButtonEventsHandlerSubject interface
    bool subscribe(interface::ButtonEventsHandlerObserver * observer) override;
    void unsubscribe(interface::ButtonEventsHandlerObserver * observer) override;

    // ButtonsControllerCallbackProvider interface
    // Callback method implementation - called by ButtonsController
    void onButtonChanged(uint16_t buttonIndex, bool pressed);

    // ButtonStateSm::Callback interface
    void onButtonShortPressed(uint16_t buttonIndex) override;
    void onButtonLongPressed(uint16_t buttonIndex) override;

protected:
    // XFBehavior interface
    XFEventStatus processEvent() override;

    // ButtonEventsHandlerSubject interface
    void notifyButtonShortPressed(ButtonIndex buttonIndex) override;
    void notifyButtonLongPressed(ButtonIndex buttonIndex) override;

    typedef enum
    {
        evButtonShortPressedInternal = 10,
        evButtonLongPressedInternal = 11
    } InternalEventId;

protected:
    static const uint16_t MAX_BUTTONS = 4;
    static const uint16_t MAX_OBSERVERS = 4;

    ButtonStateSm * buttonStateSm_[MAX_BUTTONS];
    interface::ButtonEventsHandlerObserver * observers_[MAX_OBSERVERS];
    uint16_t observerCount_;
};

#endif // MDW_BUTTON_BUTTONEVENTSHANDLER_H
