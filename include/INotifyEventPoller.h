#pragma once

#include <EventPoller.h>
#include <string>
#include <stdint.h>
#include <sys/inotify.h>
#include <sys/epoll.h>

#define MAXPOLLSIZE 256
#define INOTIFY_EVENT_SIZE (sizeof (struct inotify_event))
#define INOTIFY_BUF_LEN (1024 * (INOTIFY_EVENT_SIZE + 16))

class INotifyEventPoller : public EventPoller
{


public:
    INotifyEventPoller();
    ~INotifyEventPoller();

    //Override
    virtual int poll(int timeout);
    virtual int service();

    //Class specific functions to handle inotify
    int initWatch();
    int addWatch(const std::string& pathname, const uint32_t mask);
    int removeWatch(int fd);

protected:
private:

    //epoll
    int m_epoll_fd;
    struct epoll_event m_epoll_event;
    struct epoll_event m_epoll_events[MAXPOLLSIZE];
    int m_number_of_fds;

    //inotify
    int m_inotify_init_fd;
    int m_inotify_watch_fd;
    char m_inotify_buf[INOTIFY_BUF_LEN];
    int m_inotify_buf_len;

    //TODO: maintain a set of the the fd and the corresponding watch pathname?
};
