#ifndef MDW_BUTTON_BUTTONSTATESM_H
#define MDW_BUTTON_BUTTONSTATESM_H

#include "xf/behavior.h"
#include <cstdint>

/**
 * @brief State machine for detecting button short and long presses.
 *
 * This state machine monitors button press/release events and determines
 * whether a button press is short or long based on timing.
 */
class ButtonStateSm : public XFBehavior
{
public:
    /**
     * @brief Callback interface for button state machine notifications.
     */
    class Callback
    {
    public:
        virtual ~Callback() {}
        virtual void onButtonShortPressed(uint16_t buttonIndex) = 0;
        virtual void onButtonLongPressed(uint16_t buttonIndex) = 0;
    };

    /**
     * @brief Constructor.
     * @param buttonIndex The index of the button this state machine monitors.
     * @param callback Callback interface for notifications.
     */
    ButtonStateSm(uint16_t buttonIndex, Callback * callback);

    /**
     * @brief Notifies the state machine that the button state has changed.
     * @param pressed True if button is pressed, false if released.
     */
    void onButtonChanged(bool pressed);

protected:
    XFEventStatus processEvent() override;

    typedef enum
    {
        STATE_IDLE,
        STATE_PRESSED,
        STATE_SHORT_PRESSED,
        STATE_LONG_PRESSED
    } State;

    typedef enum
    {
        evButtonPressed = 1,
        evButtonReleased = 2,
        evLongPressTimeout = 3
    } EventId;

protected:
    State state_;
    uint16_t buttonIndex_;
    Callback * callback_;

    static const uint32_t LONG_PRESS_TIMEOUT_MS = 1000;  ///< Timeout for long press detection (1 second)
};

#endif // MDW_BUTTON_BUTTONSTATESM_H
