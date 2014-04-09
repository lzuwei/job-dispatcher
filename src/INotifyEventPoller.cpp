#include <INotifyEventPoller.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

INotifyEventPoller::INotifyEventPoller()
{
    //ctor
}

INotifyEventPoller::~INotifyEventPoller()
{
    /*closing the INOTIFY instance*/
    close( m_inotify_init_fd );
}

/** \brief Initialize an INotifyWatch.
 *
 * \return int
 *
 */
int INotifyEventPoller::initWatch()
{
    m_inotify_init_fd = inotify_init();
    if(m_inotify_init_fd < 0)
        perror("Error Initializing INotify");

    //setup the epoll events, consider doing it in the constructor
    m_epoll_fd = epoll_create(MAXPOLLSIZE);
    m_epoll_event.events = EPOLLIN | EPOLLPRI;
    m_epoll_event.data.fd = m_inotify_init_fd;
    int res = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_inotify_init_fd, &m_epoll_event);
    if(res < 0)
        perror("Epoll Add Control");

    return m_inotify_init_fd;
}

/** \brief Adds a File or Folder to be watched.
 *
 * \param pathname const std::string&
 * \param mask uint32_t INotify Event Masks
 * \return int On success, inotify_add_watch() returns a nonnegative watch
 *              descriptor. On error, -1 is returned and errno is set appropriately.
 *
 *       EACCES Read access to the given file is not permitted.
 *       EBADF  The given file descriptor is not valid.
 *       EFAULT pathname points outside of the process's accessible address
 *              space.
 *       EINVAL The given event mask contains no valid events; or fd is not an
 *              inotify file descriptor.
 *       ENAMETOOLONG
 *              pathname is too long.
 *       ENOENT A directory component in pathname does not exist or is a
 *              dangling symbolic link.
 *       ENOMEM Insufficient kernel memory was available.
 *       ENOSPC The user limit on the total number of inotify watches was
 *              reached or the kernel failed to allocate a needed resource.
 */
int INotifyEventPoller::addWatch(const std::string& pathname, const uint32_t mask)
{
    m_inotify_watch_fd = inotify_add_watch(m_inotify_init_fd, pathname.c_str(), mask);
    return m_inotify_watch_fd;
}


/** \brief Removes a INotify watch using the File Descriptor return by addWatch.
 *
 * \param wd int watch descriptor to unwatch
 * \return int On success, removeWatch() returns zero,
 *             or -1 if an error occurred (in which case, errno is set appropriately). <p>
 *
 *           EBADF
 *              fd is not a valid file descriptor.
 *           EINVAL
 *                The watch descriptor wd is not valid; or fd is not an inotify file descriptor.
 *
 */
int INotifyEventPoller::removeWatch(int wd)
{
    /*removing the “/tmp” directory from the watch list.*/
    return inotify_rm_watch(m_inotify_init_fd, wd);
}

int INotifyEventPoller::poll(int timeout = -1)
{
    m_number_of_fds = epoll_wait(m_epoll_fd, m_epoll_events, MAXPOLLSIZE, timeout);
    return m_number_of_fds;
}

int INotifyEventPoller::service()
{
    int inotify_buf_len;

    if (m_number_of_fds < 0)
    {
        perror("epoll_wait");
        return 1;
    }

    for (int i = 0; i < m_number_of_fds; i++)
    {
        int fd = m_epoll_events[i].data.fd;

        if ( fd = m_inotify_init_fd )
        {

            // it's an inotify thing
            inotify_buf_len = read (fd, m_inotify_buf, INOTIFY_BUF_LEN);
            int j=0;

            while ( j < inotify_buf_len)
            {
                struct inotify_event *event;

                event = (struct inotify_event *) &m_inotify_buf[j];

                if(event->len)
                {
                    if ( event->mask & IN_CREATE )
                    {
                        if ( event->mask & IN_ISDIR )
                        {
                            printf( "New directory %s created.\n", event->name );
                        }
                        else
                        {
                            printf( "New file %s created.\n", event->name );
                        }
                    }
                    else if ( event->mask & IN_DELETE )
                    {
                        if ( event->mask & IN_ISDIR )
                        {
                            printf( "Directory %s deleted.\n", event->name );
                        }
                        else
                        {
                            printf( "File %s deleted.\n", event->name );
                        }
                    }
                    else if ( event->mask & IN_MODIFY)
                    {
                        if( event->mask & IN_ISDIR)
                        {
                            printf( "Directory %s is modified\n", event->name);
                        }
                        else
                        {
                            printf( "File %s is modified\n", event->name);
                        }
                    }
                }

                printf ("wd=%d mask=%u cookie=%u len=%u\n", event->wd, event->mask, event->cookie, event->len);

                if (event->len) printf("name=%s\n", event->name);

                j += INOTIFY_EVENT_SIZE + event->len;

            }
        }
    }

    return 0;
}


