#include <INotifyWatch.h>

INotifyWatch::~INotifyWatch()
{
    //dtor
}

void INotifyWatch::addINotifyEventListener(INotifyEventListener* listener)
{
    m_event_listeners.push_back(listener);
}

void INotifyWatch::removeINotifyEventListener(INotifyEventListener* listener)
{
    m_event_listeners.remove(listener);
}

void INotifyWatch::notifyAll(const INotifyEvent& event)
{
    for(std::list<INotifyEventListener*>::iterator it = m_event_listeners.begin(), end = m_event_listeners.end();
    it != end; ++it)
    {
        (*it)->onReceiveINotifyEvent(event);
    }
}
