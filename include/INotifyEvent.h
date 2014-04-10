#pragma once
#include <stdint.h>
#include <string>

//inotify
#include <sys/inotify.h>

//forward Declarations
class INotifyWatch;

class INotifyEvent
{
public:
    INotifyEvent() :
        m_mask(0),
        m_cookie(0)
    {

    }

    INotifyEvent(const struct inotify_event* evt, INotifyWatch* watch) :
        m_mask(0),
        m_cookie(0)
    {
        if(evt != NULL)
        {
            m_mask = (uint32_t) evt->mask;
            m_cookie = (uint32_t) evt->cookie;
            if(evt->name != NULL)
                m_name = evt->len > 0 ? evt->name : "";
        }
        m_watch = watch;
    }

    ~INotifyEvent() {}

    //static helper methods
    static uint32_t getMaskByName(const std::string& rName);
    static void dumpTypes(uint32_t uValue, std::string& rStr);
    static bool isType(uint32_t mask, uint32_t event_type)
    {
        return ((mask & event_type) != 0) && ((~mask & event_type) == 0);
    }

    //member variables
    uint32_t mask() const {return m_mask;}
    uint32_t cookie() const {return m_cookie;}
    uint32_t length() const {return m_name.length();}
    const std::string& name() const {return m_name;}
    INotifyWatch* watch() const {return m_watch;}

    //Event Mask Identification Helpers
    bool isType(uint32_t event_type) const
    {
        return isType(m_mask, event_type);
    }

    void dumpTypes(std::string& str) const;
    int32_t watch_descriptor() const;

protected:
private:
    uint32_t m_mask;
    uint32_t m_cookie;
    std::string m_name;
    INotifyWatch* m_watch;
};
