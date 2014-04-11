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
#include <vector>
#include <boost/process.hpp>

#include <INotifyEventPoller.h>
#include <INotifyEventListener.h>
#include <JobDispatcher.h>
#include <Job.h>

using namespace std;
using namespace boost::process;
using namespace boost::process::initializers;
using namespace imrsv;

//Simple Concrete INotifyEventListener
class IpgPostProcessor : public INotifyEventListener
{
public:
    IpgPostProcessor(JobDispatcher* dispatcher) :
        m_dispatcher(dispatcher)
    {

    }

    void onReceiveINotifyEvent(const INotifyEvent& e)
    {
        std::cout << e << std::endl;
        std::string filename = e.name();

        //check for .ipg and .flv extensions
        if(filename.find(".ipg") != std::string::npos && filename.find(".flv") != std::string::npos)
        {
            Job j;
            //extract the filename without the extensions
            std::string::size_type pos = filename.find(".ipg");
            std::string base_name = filename.substr(0, pos);

            std::cout << "Extracted filename: " << base_name << std::endl;

            std::string meta_data_file = base_name + ".json";
            std::string output_file = base_name + ".output";

            //create a job for the job dispatcher
            Task t1a("/usr/local/bin/flvmeta", "-F -j " + filename, "/home/imrsv/completed");
            t1a.setRedirectStdOut("/home/imrsv/completed/" + meta_data_file);
            Task t1b("/home/imrsv/workspace/ipg/build/codeblocks/bin/Release/cara_ipg",
             "-v " + filename + " -m " + meta_data_file + " -o " + output_file + " -d",
             "/home/imrsv/completed");

             std::cout << t1a << std::endl;
             std::cout << t1b << std::endl;

             j.addTask(t1a);
             j.addTask(t1b);

             m_dispatcher->addJob(j);
        }

    }

private:
    JobDispatcher* m_dispatcher;
};


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

    INotifyEventPoller inotify_poller;

    IpgPostProcessor* ipg = new IpgPostProcessor(&dispatcher);

    int wd = inotify_poller.addWatch("/home/zu/completed", IN_MOVED_TO);
    inotify_poller.addINotifyEventListener(wd,ipg);

    while(1)
    {
        if(inotify_poller.poll(-1) > 0)
        {
            inotify_poller.service();
        }
    }
}
