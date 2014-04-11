#pragma once

#include <INotifyEvent.h>

/** \brief Event Listener Interface for INotifyEvents.
 *         Inherit from this class to receive INotifyEvents from
 *         INotifyEventPoller.
 */
class INotifyEventListener
{
    public:
        INotifyEventListener() {}
        virtual ~INotifyEventListener() {}
        virtual void onReceiveINotifyEvent(const INotifyEvent& inotify_event) = 0;
};
