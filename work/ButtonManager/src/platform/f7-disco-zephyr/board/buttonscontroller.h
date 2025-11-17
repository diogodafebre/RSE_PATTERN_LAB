#ifndef BOARD_BUTTONSCONTROLLER_H
#define BOARD_BUTTONSCONTROLLER_H

#include "xf/behavior.h"
#include "interface/buttonirq.h"
#include "interface/buttonscontrollercallbackcaller.h"
#include <cassert>
#include <cstdint>

/**
 * @brief Controller for managing button hardware and notifying about button state changes.
 *
 * This class implements a state machine that periodically polls button states
 * and notifies registered callbacks when button states change.
 */
class ButtonsController : public XFBehavior,
                          public interface::ButtonIrq,
                          public interface::ButtonsControllerCallbackCaller
{
public:
    static const uint16_t BUTTON_COUNT = 4;

    ButtonsController();
    virtual ~ButtonsController();

    static ButtonsController & getInstance() { assert(pInstance_); return *pInstance_; }

    void start();

    // ButtonIrq interface
protected:
    void onIrq() override;

    // ButtonsControllerCallbackCaller interface
protected:
    bool registerCallback(interface::ButtonsControllerCallbackProvider * callbackProvider,
                         interface::ButtonsControllerCallbackProvider::CallbackMethod callbackMethod) override;

    // XFBehavior interface
protected:
    XFEventStatus processEvent() override;

    typedef enum
    {
        evPollButtons = 1,
        evButtonIrq = 2
    } EventId;

protected:
    void pollButtons();
    bool isButtonPressed(uint16_t buttonIndex);

protected:
    static ButtonsController * pInstance_;

    interface::ButtonsControllerCallbackProvider * callbackProvider_;
    interface::ButtonsControllerCallbackProvider::CallbackMethod callbackMethod_;

    bool previousButtonState_[BUTTON_COUNT];

    static const uint32_t POLL_INTERVAL_MS = 10;  ///< Polling interval in milliseconds
};

#endif // BOARD_BUTTONSCONTROLLER_H
