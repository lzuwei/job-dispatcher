/*This is the sample program to notify us for the file creation and file deletion takes place in “/tmp” directory*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

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

    dispatcher.addJob(j1);
    dispatcher.addJob(j2);
    dispatcher.addJob(j3);
    dispatcher.addJob(j4);
    dispatcher.addJob(j5);

    while(1)
    {
        sleep(10);
    }

#if 0
    int length, i = 0;
    int fd;
    int wd;
    char buffer[EVENT_BUF_LEN];

    /*creating the INOTIFY instance*/
    fd = inotify_init();

    /*checking for error*/
    if ( fd < 0 )
    {
        perror( "inotify_init" );
    }

    /*adding the “/tmp” directory into watch list. Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
    wd = inotify_add_watch( fd, "/home/zu/tmp", IN_CREATE | IN_DELETE | IN_MODIFY);

    /*read to determine the event change happens on “/tmp” directory. Actually this read blocks until the change event occurs*/

    length = read( fd, buffer, EVENT_BUF_LEN );

    /*checking for error*/
    if ( length < 0 )
    {
        perror( "read" );
    }

    /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
    while ( i < length )
    {
        struct inotify_event *event = ( struct inotify_event * ) &buffer[ i ];
        if ( event->len )
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
        i += EVENT_SIZE + event->len;
    }
    /*removing the “/tmp” directory from the watch list.*/
    inotify_rm_watch( fd, wd );

    /*closing the INOTIFY instance*/
    close( fd );
#endif
}
