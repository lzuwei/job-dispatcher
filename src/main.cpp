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
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <INotifyEventPoller.h>
#include <INotifyEventListener.h>
#include <JobDispatcher.h>
#include <Job.h>

#define APPLICATION_NAME "Job Dispatcher"
#define VERSION_NUM "0.0.1"
#define COPYRIGHT "Copyright 2014 Immersive Labs. All rights reserved."

using namespace std;
using namespace boost::process;
using namespace boost::process::initializers;
using namespace imrsv;

//Simple Concrete INotifyEventListener
class IpgPostProcessor : public INotifyEventListener
{
public:
    IpgPostProcessor(JobDispatcher* dispatcher, const std::string ipg_executable_path, const std::string& watch_directory) :
        m_dispatcher(dispatcher),
        m_ipg_executable_path(ipg_executable_path),
        m_watch_directory(watch_directory)
    {

    }

    const std::string& ipg_executable_path() const
    {
        return m_ipg_executable_path;
    }
    const std::string& watch_directory() const
    {
        return m_watch_directory;
    }

    void setIpgExecutablePath(const std::string& path)
    {
        m_ipg_executable_path = path;
    }
    void setWatchDirectory(const std::string& directory)
    {
        m_watch_directory = directory;
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

            std::string meta_data_file = base_name + ".json";
            std::string output_file = base_name + ".output";

            //create a job for the job dispatcher
            Task t1a("/usr/local/bin/flvmeta", "-F -j " + filename, m_watch_directory);
            t1a.setRedirectStdOut(m_watch_directory + "/" + meta_data_file);
            Task t1b(m_ipg_executable_path,
                     "-v " + filename + " -m " + meta_data_file + " -o " + output_file,
                     m_watch_directory);

            std::cout << t1a << std::endl;
            std::cout << t1b << std::endl;

            j.addTask(t1a);
            j.addTask(t1b);

            m_dispatcher->addJob(j);
        }
    }

private:
    JobDispatcher* m_dispatcher;
    std::string m_ipg_executable_path;
    std::string m_watch_directory;
};


int main(int argc, char* argv[])
{
    namespace po = boost::program_options;
    po::options_description description("Usage: job-dispatcher --config [FILE]");

    //add program options
    description.add_options()
    ("help", "Display this help message")
    ("version", "Display the version number")
    ("config", po::value<std::string>(), "Configuration File");

    //parse command line
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << APPLICATION_NAME " - Asynchronous Job Dispatcher using Thread Pools and Boost Process" << std::endl
                  << description << std::endl;
        return 0;
    }

    if(vm.count("version"))
    {
        std::cout << APPLICATION_NAME << " " << VERSION_NUM << std::endl;
        std::cout << COPYRIGHT << std::endl;
        return 0;
    }

    //config file name
    std::string config_file;
    if(vm.count("config"))
        config_file = vm["config"].as<std::string>();
    else
    {
        std::cout << APPLICATION_NAME " - Asynchronous Job Dispatcher using Thread Pools and Boost Process" << std::endl
                  << description << std::endl;
        return 0;
    }

    //parse the json config file to obtain configuration
    int num_workers;
    int max_job_queue;
    std::string ipg_exe_path;
    std::string ipg_watch_dir;
    std::string ipg_watch_event;
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_json(config_file, pt);

        num_workers = pt.get<int>("dispatcher.num_workers");
        max_job_queue = pt.get<int>("dispatcher.max_job_queue");

        ipg_exe_path = pt.get<std::string>("ipg.executable_path");
        ipg_watch_dir = pt.get<std::string>("ipg.watch_directory");
        ipg_watch_event = pt.get<std::string>("ipg.watch_event");

        std::cout << num_workers << std::endl;
        std::cout << max_job_queue << std::endl;
        std::cout << ipg_exe_path << std::endl;
        std::cout << ipg_watch_dir << std::endl;
    }
    catch (boost::property_tree::json_parser::json_parser_error& e)
    {
        e.what();
        std::terminate();
    }
    catch (boost::property_tree::ptree_error& e)
    {
        e.what();
        std::terminate();
    }

    JobDispatcher dispatcher(num_workers,max_job_queue);
    INotifyEventPoller inotify_poller;

    IpgPostProcessor* ipg = new IpgPostProcessor(&dispatcher,
            ipg_exe_path,
            ipg_watch_dir);

    uint32_t event_mask = INotifyEvent::parseEventMask(ipg_watch_event);
    int wd = inotify_poller.addWatch(ipg_watch_dir, event_mask);
    inotify_poller.addINotifyEventListener(wd,ipg);

    while(1)
    {
        if(inotify_poller.poll(-1) > 0)
            inotify_poller.service();
    }
}
