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

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <cassert>

#include <boost/process.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread.hpp>

#include <INotifyEventPoller.h>
#include <INotifyEventListener.h>
#include <JobDispatcher.h>
#include <Job.h>

#define APPLICATION_NAME "Job Dispatcher"
#define VERSION_NUM "0.0.6"
#define COPYRIGHT "Copyright 2014 Immersive Labs. All rights reserved."

using namespace std;
using namespace boost::process;
using namespace boost::process::initializers;
using namespace imrsv;

template<typename Container>
static Container split(const std::string& input, const std::string& delim)
{
    Container split_list;
    std::size_t start = 0;
    std::size_t found = input.find_first_of(delim);
    while (found!=std::string::npos)
    {
        split_list.push_back(input.substr(start, found - start));
        start = found + 1;
        found = input.find_first_of(delim, start);
    }

    split_list.push_back(input.substr(start, found - start));
    return split_list;
}

class AbstractProgram
{
public:
    explicit AbstractProgram(const boost::property_tree::ptree& c) :
        m_config(c)
    {

    }
    virtual ~AbstractProgram(){}

    virtual Job createJob(const std::string& filename, const std::string& working_directory) = 0;

protected:
    boost::property_tree::ptree m_config;
};

class IpgOldProgram : public AbstractProgram
{
public:
    explicit IpgOldProgram(const boost::property_tree::ptree& c) :
        AbstractProgram(c)
    {
    }
    Job createJob(const std::string& filename, const std::string& working_directory)
    {
        std::string path = m_config.get<std::string>("path");

        Job j;
        //extract the filename without the extensions
        std::string::size_type pos = filename.find_first_of(".");
        std::string base_name = filename.substr(0, pos);

        std::string meta_data_file = base_name + ".meta";
        std::string output_file = base_name + ".output";

        //create a job for the job dispatcher
        Task t1a("/usr/local/bin/flvmeta", "-F -j " + filename, working_directory);
        t1a.setRedirectStdOut(working_directory + "/" + meta_data_file);
        Task t1b(path,
                 "-v " + filename + " -m " + meta_data_file + " -o " + output_file,
                 working_directory);

        std::cout << t1a << std::endl;
        std::cout << t1b << std::endl;

        j.addTask(t1a);
        j.addTask(t1b);

        return j;
    }
};

class IpgProgram: public AbstractProgram
{
public:
    explicit IpgProgram(const boost::property_tree::ptree& c) :
        AbstractProgram(c)
    {
    }
    Job createJob(const std::string& filename, const std::string& working_directory)
    {
        std::ostringstream oss;
        std::string path = m_config.get<std::string>("path");

        //emotion server arguments
        std::string database_data = m_config.get<std::string>("database_data","");
        std::string database_results = m_config.get<std::string>("database_results","");
        std::string collection_data = m_config.get<std::string>("collection_data","");
        std::string collection_results = m_config.get<std::string>("collection_results","");
        std::string bin_results_fieldname = m_config.get<std::string>("bin_results_fieldname","");
        bool realtime = m_config.get<bool>("realtime",false);
        bool print = m_config.get<bool>("print",false);
        bool print_raw = m_config.get<bool>("print_raw",false);
        bool print_time = m_config.get<bool>("print_time",false);
        int sampling_rate = m_config.get<int>("sampling",-1);
        bool verbose = m_config.get<bool>("verbose",false);

        //now we construct the arguments, remember to put spacing at end of each option
        std::string arguments = "";

        //look for db
        if(!database_data.empty())
            arguments.append("--database_data=" + database_data + " ");

        if(!database_results.empty())
            arguments.append("--database_results=" + database_results + " ");

        if(!collection_data.empty())
            arguments.append("--collection_data=" + collection_data + " ");

        if(!collection_results.empty())
            arguments.append("--collection_results=" + collection_results + " ");

        if(!bin_results_fieldname.empty())
            arguments.append("--bin_results_fieldname=" + bin_results_fieldname + " ");

        if(realtime)
            arguments.append("--realtime ");

        if(sampling_rate != -1)
        {
            //get the sampling rate and conver to string
            oss << sampling_rate;
            arguments.append("--sampling_rate=" + oss.str() + " ");
        }

        if(print)
            arguments.append("--print ");

        if(print_raw)
            arguments.append("--print_raw ");

        if(print_time)
            arguments.append("--print_time ");

        if(verbose)
            arguments.append("--verbose ");

        Job j;
        //extract the filename without the extensions
        std::string::size_type pos = filename.find_first_of(".");
        std::string base_name = filename.substr(0, pos);
        std::string meta_data_file = base_name + ".meta";

        //mandatory arguments
        arguments.append("--insert ")
        .append(filename + " ")
        .append(meta_data_file);

        //create a job for the job dispatcher
        Task t1a("/usr/local/bin/flvmeta", "-F -j " + filename, working_directory);
        t1a.setRedirectStdOut(working_directory + "/" + meta_data_file);
        Task t1b(path, arguments, working_directory);

        std::cout << t1a << std::endl;
        std::cout << t1b << std::endl;

        j.addTask(t1a);
        j.addTask(t1b);

        return j;
    }
};

class EmoProgram : public AbstractProgram
{
public:
    explicit EmoProgram(const boost::property_tree::ptree& c) :
        AbstractProgram(c)
    {

    }
    Job createJob(const std::string& filename, const std::string& working_directory)
    {
        std::ostringstream oss;
        std::string path = m_config.get<std::string>("path");

        //emotion server arguments
        std::string database_data = m_config.get<std::string>("database_data","");
        std::string database_results = m_config.get<std::string>("database_results","");
        std::string collection_data = m_config.get<std::string>("collection_data","");
        std::string collection_results = m_config.get<std::string>("collection_results","");
        std::string bin_results_fieldname = m_config.get<std::string>("bin_results_fieldname","");
        std::string done_processing_dir = m_config.get<std::string>("done_processing_dir","");
        bool realtime = m_config.get<bool>("realtime",false);
        bool print = m_config.get<bool>("print",false);
        bool print_raw = m_config.get<bool>("print_raw",false);
        bool print_time = m_config.get<bool>("print_time",false);
        int sampling_rate = m_config.get<int>("sampling",-1);
        bool verbose = m_config.get<bool>("verbose",false);

        //now we construct the arguments, remember to put spacing at end of each option
        std::string arguments = "";

        //look for db
        if(!database_data.empty())
            arguments.append("--database_data=" + database_data + " ");

        if(!database_results.empty())
            arguments.append("--database_results=" + database_results + " ");

        if(!collection_data.empty())
            arguments.append("--collection_data=" + collection_data + " ");

        if(!collection_results.empty())
            arguments.append("--collection_results=" + collection_results + " ");

        if(!bin_results_fieldname.empty())
            arguments.append("--bin_results_fieldname=" + bin_results_fieldname + " ");

        if(realtime)
            arguments.append("--realtime ");

        if(sampling_rate != -1)
        {
            //get the sampling rate and conver to string
            oss << sampling_rate;
            arguments.append("--sampling_rate=" + oss.str() + " ");
        }

        if(print)
            arguments.append("--print ");

        if(print_raw)
            arguments.append("--print_raw ");

        if(print_time)
            arguments.append("--print_time ");

        if(verbose)
            arguments.append("--verbose ");

        Job j;
        //extract the filename without the extensions
        std::string::size_type pos = filename.find_first_of(".");
        std::string base_name = filename.substr(0, pos);
        std::string meta_data_file = base_name + ".meta";

        //mandatory arguments
        arguments.append("--insert ")
        .append(filename + " ")
        .append(meta_data_file);

        //create a job for the job dispatcher
        Task t1a("/usr/local/bin/flvmeta", "-F -j " + filename, working_directory);
        t1a.setRedirectStdOut(working_directory + "/" + meta_data_file);
        Task t1b(path, arguments, working_directory);

        std::cout << t1a << std::endl;
        std::cout << t1b << std::endl;

        j.addTask(t1a);
        j.addTask(t1b);

        if(!done_processing_dir.empty())
        {
            Task t1c("/bin/mv", "-f " + filename + " " + meta_data_file + " " + done_processing_dir, working_directory);
            std::cout << t1c << std::endl;
            j.addTask(t1c);
        }

        return j;
    }
};

//Simple Concrete INotifyEventListener
class AMSPostProcessor : public INotifyEventListener
{
public:

    class NoProgramConfigExistsException : public std::exception
    {
    public:
        NoProgramConfigExistsException(const std::string& program_name) :
            m_program_name(program_name)
        {
        }
        virtual const char* what() const throw()
        {
            return std::string("No Program Config Record for " +  m_program_name + " Found!").c_str();
        }
    private:
        std::string m_program_name;
    };

    AMSPostProcessor(JobDispatcher* dispatcher, const std::string& watch_directory) :
        m_dispatcher(dispatcher),
        m_watch_directory(watch_directory)
    {

    }

    ~AMSPostProcessor()
    {
        removeAllPrograms();
    }

    void addProgram(const std::string& program_name, const boost::property_tree::ptree& config)
    {
        AbstractProgram* program = nullptr;
        if(program_name == "emo" || program_name == "emo-dev")
            program = new EmoProgram(config);
        else if(program_name == "ipg-dev" || program_name == "ipg-prod")
            program = new IpgProgram(config);
        else if(program_name == "ipg")
            program = new IpgOldProgram(config);

        //check if a program handler is defined
        if(program != nullptr)
            m_programs[program_name] = program;
    }

    void removeProgram(const std::string& program_name)
    {
        std::map<std::string, AbstractProgram*>::iterator found = m_programs.find(program_name);
        if(found != m_programs.end())
            delete found->second;
    }

    void removeAllPrograms()
    {
        for(auto p : m_programs)
           delete p.second;
        m_programs.clear();
    }

    AbstractProgram* findProgram(const std::string& program_name)
    {
        std::map<std::string, AbstractProgram*>::const_iterator found = m_programs.find(program_name);
        if(found == m_programs.end())
            throw NoProgramConfigExistsException(program_name);
        else
            return found->second;
    }

    const std::string& watch_directory() const
    {
        return m_watch_directory;
    }

    void setWatchDirectory(const std::string& directory)
    {
        m_watch_directory = directory;
    }

    void onReceiveINotifyEvent(const INotifyEvent& e)
    {
        std::cout << e << std::endl;
        std::string filename = e.name();

        try
        {
            std::list<std::string> s = split<std::list<std::string> >(filename,".");
            std::string extension = s.back();
            if(extension == "flv" && s.size() > 2) //if it is an flv file, run the programs
            {
                std::list<std::string>::iterator start = ++(s.begin());
                std::list<std::string>::iterator end = --(s.end());

                std::for_each(start, end,
                              [&](std::string& program_name)
                              {
                                  try
                                  {
                                      AbstractProgram* p = findProgram(program_name);
                                      Job j = p->createJob(filename, m_watch_directory);
                                      m_dispatcher->addJob(j);
                                  }
                                  catch (const NoProgramConfigExistsException& e)
                                  {
                                      std::cerr << e.what() << std::endl;
                                  }
                              }
                              );
            }
        }
        catch (boost::property_tree::ptree_error& e)
        {
            std::cerr << e.what() << std::endl;
        }

    }

private:
    JobDispatcher* m_dispatcher;
    std::string m_watch_directory;
    std::map<std::string, AbstractProgram*> m_programs;
};

void testSplit()
{
    std::vector<std::string> s = split<std::vector<std::string> >("12345677880.emo.ipg.emo-dev.ipg-prod.ipg-dev.flv", ".");
    assert(s[0] == "12345677880");
    assert(s[1] == "emo");
    assert(s[2] == "ipg");
    assert(s[3] == "emo-dev");
    assert(s[4] == "ipg-prod");
    assert(s[5] == "ipg-dev");
    assert(s[6] == "flv");
}

void printConfig(const std::string& program_name, const boost::property_tree::ptree& config)
{
    if(program_name == "ipg")
    {
        std::string path = config.get<std::string>("path");
        std::cout<< "ipg path: " << path << std::endl;
    }
    if(program_name == "emo")
    {
        std::string path = config.get<std::string>("path");
        std::string database_data = config.get<std::string>("database_data");
        std::string database_results = config.get<std::string>("database_results");
        std::string done_processing_dir = config.get<std::string>("done_processing_dir");
        int sampling = config.get<int>("sampling");
        bool realtime = config.get<bool>("realtime");
        std::cout<< "emo path: " << path << std::endl;
        std::cout<< "emo database_data: " << database_data << std::endl;
        std::cout<< "emo database_results: " << database_results << std::endl;
        std::cout<< "emo done_processing_dir: " << done_processing_dir << std::endl;
        std::cout<< "emo sampling: " << sampling << std::endl;
        std::cout<< "emo realtime: " << realtime << std::endl;
    }
}

int main(int argc, char* argv[])
{
    namespace po = boost::program_options;

    po::options_description description("Usage: job-dispatcher --config=[FILE]");

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

    //property trees
    boost::property_tree::ptree pt;

    try
    {
        boost::property_tree::read_json(config_file, pt);

        boost::property_tree::ptree dispatcher_conf = pt.get_child("dispatcher");
        num_workers = dispatcher_conf.get<int>("num_workers",boost::thread::hardware_concurrency() - 1);
        max_job_queue = dispatcher_conf.get<int>("max_job_queue");

        std::cout << num_workers << std::endl;
        std::cout << max_job_queue << std::endl;

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

    //Job dispatcher successfully initialized
    JobDispatcher dispatcher(num_workers,max_job_queue);
    INotifyEventPoller inotify_poller;

    try //try looking for ams conf
    {
        boost::property_tree::ptree ams_conf;
        ams_conf = pt.get_child("ams");
        std::string ams_watch_directory = ams_conf.get<std::string>("watch_directory");
        std::string ams_watch_event = ams_conf.get<std::string>("watch_event");

        std::cout << "AMS watch directory: " << ams_watch_directory << std::endl;
        std::cout << "AMS watch event: " << ams_watch_event << std::endl;

        AMSPostProcessor* ams = new AMSPostProcessor(&dispatcher, ams_watch_directory);
        //iterate through the projects node and add the projects into the AMS Post Processor
        std::cout << "Programs installed on AMS Post Processor: ";
        for(const boost::property_tree::ptree::value_type& v : ams_conf.get_child("projects"))
        {
            // v.first is the name of the child/program.
            // v.second is the child tree/config options.
            std::cout << v.first << " ";
            ams->addProgram(v.first,v.second);
        }
        std::cout << std::endl;

        //add ams to InotifyPoller
        uint32_t event_mask = INotifyEvent::parseEventMask(ams_watch_event);
        int wd = inotify_poller.addWatch(ams_watch_directory, event_mask);
        inotify_poller.addINotifyEventListener(wd,ams);

    }
    catch (boost::property_tree::ptree_error& e)
    {
        e.what();
        std::cout << "Warning no AMS config found." << std::endl;
    }

    while(1)
    {
        if(inotify_poller.poll(-1) > 0)
            inotify_poller.service();
    }
}
