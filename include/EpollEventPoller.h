#pragma once

#include <EventPoller.h>

class EpollEventHandler
{
public:
    virtual void handleEvent(int fd) = 0;
};

class EpollEventPoller : public EventPoller
{
    public:
        EpollEventPoller();
        ~EpollEventPoller();
        void addEventHandler(int fd, EpollEventHandler* event_handler);
        void removeEventHandler(int fd);
    protected:
    private:
        int m_epoll_fd;
};
