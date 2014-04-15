#pragma once
#include <stdint.h>
#include <string>
#include <iostream>

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

    INotifyEvent(const struct inotify_event* evt) :
        m_mask(0),
        m_cookie(0)
    {
        if(evt != NULL)
        {
            m_mask = (uint32_t) evt->mask;
            m_cookie = (uint32_t) evt->cookie;
            if(evt->name != NULL)
                m_name = evt->len > 0 ? evt->name : "";
            m_wd = evt->wd;
        }
    }

    ~INotifyEvent() {}

    //static helper methods
    static uint32_t parseEventMask(const std::string& event);
    static uint32_t getMaskByName(const std::string& event);
    static void dumpTypes(uint32_t uValue, std::string& r_str);
    static bool isType(uint32_t mask, uint32_t event_type)
    {
        return ((mask & event_type) != 0) && ((~mask & event_type) == 0);
    }

    //member variables
    uint32_t mask() const {return m_mask;}
    uint32_t cookie() const {return m_cookie;}
    uint32_t length() const {return m_name.length();}
    const std::string& name() const {return m_name;}
    uint32_t wd() const {return m_wd;}

    //Event Mask Identification Helpers
    bool isType(uint32_t event_type) const
    {
        return isType(m_mask, event_type);
    }

    void dumpTypes(std::string& str) const;

    friend std::ostream& operator<< (std::ostream& out, const INotifyEvent& e)
    {
        std::string event_type;
        e.dumpTypes(event_type);

        out << "{"
        << "\"wd\": " << e.wd() << ", "
        << "\"name\": " << e.name() << ", "
        << "\"mask\": " << event_type << ", "
        << "\"cookie\": " << e.cookie()
        << "}";

        return out;
    }

protected:
private:
    uint32_t m_mask;
    uint32_t m_cookie;
    std::string m_name;
    uint32_t m_wd;
};
