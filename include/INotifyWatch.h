#pragma once

#include <list>
#include <INotifyEventPoller.h>
#include <INotifyEventListener.h>

/** \brief Wrapper for INotify Watch Descriptors
 *         Instances of this class are created when a
 *         watch is added to a INotifyEventPoller.
 */
class INotifyWatch
{
public:
    INotifyWatch(const std::string& path, int32_t mask, bool enabled = true) :
        m_path(path),
        m_mask(mask),
        m_wd((int32_t) -1),
        m_enabled(enabled)
    {

    }
    ~INotifyWatch();

    const std::string& path() const {return m_path;}
    const uint32_t mask() const {return m_mask;}
    const int32_t wd() const {return m_wd;}
    const bool enabled() const {return m_enabled;}
protected:
private:
    std::string m_path;   ///< watched file path
    uint32_t m_mask;     ///< event mask
    int32_t m_wd;         ///< watch descriptor
    //Inotify* m_pInotify;  ///< inotify object
    bool m_enabled;      ///< events enabled yes/no

    void addINotifyEventListener(INotifyEventListener* listener);
    void removeINotifyEventListener(INotifyEventListener* listener);
    void notifyAll(const INotifyEvent& event);

    //Event Listeners
    std::list<INotifyEventListener*> m_event_listeners;

    friend class INotifyEventPoller;
};
