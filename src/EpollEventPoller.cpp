#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>

#include <EpollEventPoller.h>

EpollEventPoller::EpollEventPoller()
{
    m_epoll_fd = epoll_create1(0);
    if (m_epoll_fd == -1)
    {
      perror ("epoll_create");
    }
}

EpollEventPoller::~EpollEventPoller()
{

}

void EpollEventPoller::addEventHandler(int fd, EpollEventHandler* event_handler)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    int s = epoll_ctl (m_epoll_fd, EPOLL_CTL_ADD, fd, &event);
    if (s == -1)
    {
      perror ("epoll_ctl");
    }
}

void EpollEventPoller::removeEventHandler(int fd)
{

}
