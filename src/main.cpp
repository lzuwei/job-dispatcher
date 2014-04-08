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
    JobDispatcher dispatcher(5, 100);

    Job j(Job::HIGH);
    Task t1("/usr/local/bin/flvmeta", "-F -j jason.ipg.flv");
    t1.setRedirectStdOut("jason.json");
    Task t2("/home/zu/workspace/ipg/build/codeblocks/bin/Release/cara_ipg",
            "-v jason.ipg.flv -m jason.json -o jason.output -d");
    j.addTask(t1);
    j.addTask(t2);

    std::cout << t1 << std::endl;
    std::cout << t2 << std::endl;

    Job j1,j2,j3,j4,j5,j6,j7(Job::LOW),j8,j9(Job::VERY_LOW),j10(Job::VERY_HIGH);

    std::cout<< j << std::endl;
    std::cout<< j1 << std::endl;
    std::cout<< j2 << std::endl;
    std::cout<< j3 << std::endl;
    std::cout<< j4 << std::endl;
    std::cout<< j5 << std::endl;
    std::cout<< j6 << std::endl;
    std::cout<< j7 << std::endl;
    std::cout<< j8 << std::endl;
    std::cout<< j9 << std::endl;
    std::cout<< j10 << std::endl;
    std::cout<< std::endl;

    dispatcher.push(j);
    dispatcher.push(j1);
    dispatcher.push(j2);
    dispatcher.push(j3);
    dispatcher.push(j4);
    dispatcher.push(j5);
    dispatcher.push(j6);
    dispatcher.push(j7);
    dispatcher.push(j8);
    dispatcher.push(j9);
    dispatcher.push(j10);

    while(!dispatcher.isEmpty()) {
        Job j = dispatcher.top();
        std::cout<< j << std::endl;
        dispatcher.pop();
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
