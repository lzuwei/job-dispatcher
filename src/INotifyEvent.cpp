#include <vector>

#include <INotifyEvent.h>
#include <INotifyWatch.h>

/// dump separator (between particular entries)
#define DUMP_SEP \
  ({ \
    if (!r_str.empty()) { \
      r_str.append(","); \
    } \
  })

/** \brief tokenizes a string.
 *
 * \param str const std::string& string to tokenize
 * \param delim const std::string& delimiter
 * \param tokenized std::vector<std::string>& tokenized string
 * \return void
 *
 */
static void tokenize(const std::string& str, const std::string& delim, std::vector<std::string>& tokenized)
{
    std::size_t prev = 0, curr = 0;
    if(str.empty()) //empty, nothing to tokenize
        return;

    while(curr != std::string::npos)
    {
        std::string token;
        curr = str.find_first_of(delim, curr);
        token = str.substr(prev, curr - prev);
        tokenized.push_back(token);
        if(curr != std::string::npos)
            curr += 1;
        prev = curr;
    }
}

uint32_t INotifyEvent::getMaskByName(const std::string& r_name)
{
    if (r_name == "IN_ACCESS")
        return IN_ACCESS;
    else if (r_name == "IN_MODIFY")
        return IN_MODIFY;
    else if (r_name == "IN_ATTRIB")
        return IN_ATTRIB;
    else if (r_name == "IN_CLOSE_WRITE")
        return IN_CLOSE_WRITE;
    else if (r_name == "IN_CLOSE_NOWRITE")
        return IN_CLOSE_NOWRITE;
    else if (r_name == "IN_OPEN")
        return IN_OPEN;
    else if (r_name == "IN_MOVED_FROM")
        return IN_MOVED_FROM;
    else if (r_name == "IN_MOVED_TO")
        return IN_MOVED_TO;
    else if (r_name == "IN_CREATE")
        return IN_CREATE;
    else if (r_name == "IN_DELETE")
        return IN_DELETE;
    else if (r_name == "IN_DELETE_SELF")
        return IN_DELETE_SELF;
    else if (r_name == "IN_UNMOUNT")
        return IN_UNMOUNT;
    else if (r_name == "IN_Q_OVERFLOW")
        return IN_Q_OVERFLOW;
    else if (r_name == "IN_IGNORED")
        return IN_IGNORED;
    else if (r_name == "IN_CLOSE")
        return IN_CLOSE;
    else if (r_name == "IN_MOVE")
        return IN_MOVE;
    else if (r_name == "IN_ISDIR")
        return IN_ISDIR;
    else if (r_name == "IN_ONESHOT")
        return IN_ONESHOT;
    else if (r_name == "IN_ALL_EVENTS")
        return IN_ALL_EVENTS;

#ifdef IN_DONT_FOLLOW
    else if (r_name == "IN_DONT_FOLLOW")
        return IN_DONT_FOLLOW;
#endif // IN_DONT_FOLLOW

#ifdef IN_ONLYDIR
    else if (r_name == "IN_ONLYDIR")
        return IN_ONLYDIR;
#endif // IN_ONLYDIR

#ifdef IN_MOVE_SELF
    else if (r_name == "IN_MOVE_SELF")
        return IN_MOVE_SELF;
#endif // IN_MOVE_SELF

}

uint32_t INotifyEvent::parseEventMask(const std::string& event)
{
    uint32_t mask = 0;
    std::vector<std::string> inotify_events;
    tokenize(event, "|", inotify_events);
    for(int i = 0; i < inotify_events.size(); ++i)
    {
        mask |= getMaskByName(inotify_events[i]);
    }
    return mask;
}

void INotifyEvent::dumpTypes(uint32_t value, std::string& r_str)
{
    r_str = "";

    if (isType(value, IN_ALL_EVENTS))
    {
        r_str.append("IN_ALL_EVENTS");
    }
    else
    {
        if (isType(value, IN_ACCESS))
        {
            DUMP_SEP;
            r_str.append("IN_ACCESS");
        }
        if (isType(value, IN_MODIFY))
        {
            DUMP_SEP;
            r_str.append("IN_MODIFY");
        }
        if (isType(value, IN_ATTRIB))
        {
            DUMP_SEP;
            r_str.append("IN_ATTRIB");
        }
        if (isType(value, IN_CREATE))
        {
            DUMP_SEP;
            r_str.append("IN_CREATE");
        }
        if (isType(value, IN_DELETE))
        {
            DUMP_SEP;
            r_str.append("IN_DELETE");
        }
        if (isType(value, IN_DELETE_SELF))
        {
            DUMP_SEP;
            r_str.append("IN_DELETE_SELF");
        }
        if (isType(value, IN_OPEN))
        {
            DUMP_SEP;
            r_str.append("IN_OPEN");
        }
        if (isType(value, IN_CLOSE))
        {
            DUMP_SEP;
            r_str.append("IN_CLOSE");
        }

#ifdef IN_MOVE_SELF
        if (isType(value, IN_MOVE_SELF))
        {
            DUMP_SEP;
            r_str.append("IN_MOVE_SELF");
        }
#endif // IN_MOVE_SELF

        else
        {
            if (isType(value, IN_CLOSE_WRITE))
            {
                DUMP_SEP;
                r_str.append("IN_CLOSE_WRITE");
            }
            if (isType(value, IN_CLOSE_NOWRITE))
            {
                DUMP_SEP;
                r_str.append("IN_CLOSE_NOWRITE");
            }
        }
        if (isType(value, IN_MOVE))
        {
            DUMP_SEP;
            r_str.append("IN_MOVE");
        }
        else
        {
            if (isType(value, IN_MOVED_FROM))
            {
                DUMP_SEP;
                r_str.append("IN_MOVED_FROM");
            }
            if (isType(value, IN_MOVED_TO))
            {
                DUMP_SEP;
                r_str.append("IN_MOVED_TO");
            }
        }
    }
    if (isType(value, IN_UNMOUNT))
    {
        DUMP_SEP;
        r_str.append("IN_UNMOUNT");
    }
    if (isType(value, IN_Q_OVERFLOW))
    {
        DUMP_SEP;
        r_str.append("IN_Q_OVERFLOW");
    }
    if (isType(value, IN_IGNORED))
    {
        DUMP_SEP;
        r_str.append("IN_IGNORED");
    }
    if (isType(value, IN_ISDIR))
    {
        DUMP_SEP;
        r_str.append("IN_ISDIR");
    }
    if (isType(value, IN_ONESHOT))
    {
        DUMP_SEP;
        r_str.append("IN_ONESHOT");
    }

#ifdef IN_DONT_FOLLOW
    if (isType(value, IN_DONT_FOLLOW))
    {
        DUMP_SEP;
        r_str.append("IN_DONT_FOLLOW");
    }
#endif // IN_DONT_FOLLOW

#ifdef IN_ONLYDIR
    if (isType(value, IN_ONLYDIR))
    {
        DUMP_SEP;
        r_str.append("IN_ONLYDIR");
    }
#endif // IN_ONLYDIR
}

void INotifyEvent::dumpTypes(std::string& r_str) const
{
    dumpTypes(m_mask, r_str);
}
