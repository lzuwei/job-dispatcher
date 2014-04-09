#pragma once

#include <INotifyEventPoller.h>
#include <sys/inotify.h>
#include <sys/epoll.h

INotifyEventPoller::INotifyEventPoller()
{
    //ctor
}

INotifyEventPoller::~INotifyEventPoller()
{
    //dtor
}

/** \brief Initialize an INotifyWatch.
 *
 * \return int
 *
 */
int INotifyEventPoller::initWatch()
{

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
int INotifyEventPoller:addWatch(const std::string& pathname, uint32_t mask)
{
    return 0;
}


/** \brief Removes a INotify watch using the File Descriptor return by addWatch.
 *
 * \param fd int file descriptor to unwatch
 * \return int On success, removeWatch() returns zero,
 *             or -1 if an error occurred (in which case, errno is set appropriately). <p>
 *
 *           EBADF
 *              fd is not a valid file descriptor.
 *           EINVAL
 *                The watch descriptor wd is not valid; or fd is not an inotify file descriptor.
 *
 */
int INotifyEventPoller::removeWatch(int fd)
{
    return 0;
}


