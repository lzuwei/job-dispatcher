/*This is the sample program to notify us for the file creation and file deletion takes place in “/tmp” directory*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/epoll.h>

#define MAXPOLLSIZE 256
#define INOTIFY_EVENT_SIZE (sizeof (struct inotify_event))
#define INOTIFY_BUF_LEN (1024 * (INOTIFY_EVENT_SIZE + 16))

#include <iostream>
#include <boost/process.hpp>
#include <vector>
#include <JobDispatcher.h>
#include <Job.h>

using namespace std;
using namespace boost::process;
using namespace boost::process::initializers;
using namespace imrsv;


int main()
{
    /*
    child c = execute(run_exe("/home/imrsv/workspace/ipg/build/codeblocks/bin/Release/cara_ipg"),
                set_cmd_line("/home/imrsv/workspace/ipg/build/codeblocks/bin/Release/cara_ipg -i /home/imrsv/workspace/ipg/build/codeblocks/bin/Release/zeta.jpg")
                );
    wait_for_exit(c);
    */
    JobDispatcher dispatcher(2, 100);

    Job j1(Job::VERY_LOW),j2(Job::LOW),j3(Job::HIGH),j4(Job::HIGH),j5(Job::VERY_HIGH),j6,j7(Job::LOW),j8,j9(Job::VERY_LOW),j10(Job::VERY_HIGH);

    Task t1a("/usr/local/bin/flvmeta", "-F -j test1.ipg.flv", "/home/imrsv/completed");
    t1a.setRedirectStdOut("/home/imrsv/completed/test1.json");
    Task t1b("/home/imrsv/workspace/ipg/build/codeblocks/bin/Release/cara_ipg",
             "-v test1.ipg.flv -m test1.json -o test1.output -d",
             "/home/imrsv/completed");

    Task t2a("/usr/local/bin/flvmeta", "-F -j test2.ipg.flv", "/home/imrsv/completed");
    t2a.setRedirectStdOut("/home/imrsv/completed/test2.json");
    Task t2b("/home/imrsv/workspace/ipg/build/codeblocks/bin/Release/cara_ipg",
             "-v test2.ipg.flv -m test2.json -o test2.output -d",
             "/home/imrsv/completed");

    Task t3a("/usr/local/bin/flvmeta", "-F -j test3.ipg.flv", "/home/imrsv/completed");
    t3a.setRedirectStdOut("/home/imrsv/completed/test3.json");
    Task t3b("/home/imrsv/workspace/ipg/build/codeblocks/bin/Release/cara_ipg",
             "-v test3.ipg.flv -m test3.json -o test3.output -d",
             "/home/imrsv/completed");

    Task t4a("/usr/local/bin/flvmeta", "-F -j test4.ipg.flv", "/home/imrsv/completed");
    t4a.setRedirectStdOut("/home/imrsv/completed/test4.json");
    Task t4b("/home/imrsv/workspace/ipg/build/codeblocks/bin/Release/cara_ipg",
             "-v test4.ipg.flv -m test4.json -o test4.output -d",
             "/home/imrsv/completed");

    Task t5a("/usr/local/bin/flvmeta", "-F -j test5.ipg.flv", "/home/imrsv/completed");
    t5a.setRedirectStdOut("/home/imrsv/completed/test5.json");
    Task t5b("/home/imrsv/workspace/ipg/build/codeblocks/bin/Release/cara_ipg",
             "-v test5.ipg.flv -m test5.json -o test5.output -d",
             "/home/imrsv/completed");

    j1.addTask(t1a);
    j1.addTask(t1b);

    j2.addTask(t2a);
    j2.addTask(t2b);

    j3.addTask(t3a);
    j3.addTask(t3b);

    j4.addTask(t4a);
    j4.addTask(t4b);

    j5.addTask(t5a);
    j5.addTask(t5b);

    //using epoll
    int inotify_init_fd;
    int inotify_watch_fd;
    int epoll_fd;
    struct epoll_event epoll_events[MAXPOLLSIZE];
    char inotify_buf[INOTIFY_BUF_LEN];
    int inotify_buf_len;

    // create an initofy instance
    inotify_init_fd = inotify_init();
    if ( inotify_init_fd < 0)
        perror( "inotify_init" );

    // handle error here if fd not valid
    epoll_fd = epoll_create(MAXPOLLSIZE);


    // add the inotify_init_fd to the epoll instance
    static struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI;
    ev.data.fd = inotify_init_fd;
    int res = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, inotify_init_fd, &ev);
    if(res < 0)
        perror("Epoll Add Control");

    inotify_watch_fd = inotify_add_watch(inotify_init_fd, "/home/imrsv/completed", IN_CREATE | IN_DELETE);

    // handle error here when fd not set/valid

    while (1)
    {
        int number_of_fds = epoll_wait(epoll_fd, epoll_events, MAXPOLLSIZE, -1);

        if (number_of_fds < 0)
        {
            perror("epoll_wait");
            return 1;
        }

        for (int i = 0; i < number_of_fds; i++)
        {

            int fd = epoll_events[i].data.fd;

            if ( fd = inotify_init_fd )
            {

                // it's an inotify thing

                inotify_buf_len = read (fd, inotify_buf, INOTIFY_BUF_LEN);

                int j=0;

                while ( j < inotify_buf_len)
                {

                    struct inotify_event *event;

                    event = (struct inotify_event *) &inotify_buf[j];

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
    }

    /*removing the “/tmp” directory from the watch list.*/
    inotify_rm_watch( inotify_init_fd, inotify_watch_fd );

    /*closing the INOTIFY instance*/
    close( inotify_init_fd );

}
