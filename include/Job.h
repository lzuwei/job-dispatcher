#pragma once

#include <limits.h>
#include <queue>
#include <string>
#include <boost/process.hpp>
#include <boost/process/mitigate.hpp>
#include <boost/random.hpp>

using namespace boost::process;
using namespace boost::process::initializers;
using namespace boost::iostreams;

namespace imrsv
{

typedef boost::uniform_int<> NumberDistribution;
typedef boost::mt19937 RandomNumberGenerator;
typedef boost::variate_generator<RandomNumberGenerator&,NumberDistribution> Generator;

/** \brief Generates a random number based on the given range.
 *
 * \param range_min int minimum value to be returned by the generator
 * \param range_max int maximum value to be returned by the generator
 * \return int a random number.
 *
 */
int generateRandomNumber(int range_min, int range_max)
{
    static RandomNumberGenerator generator(std::time(0));
    NumberDistribution distribution(range_min, range_max);
    Generator numberGenerator(generator, distribution);
    return numberGenerator();
}

/** \brief Task that each job will consist of.
 *         This is a holder for program arguments and redirections.
 *         Execution logic is held in the Job Instance with
 *         Task Completion Handles and Process Launch Error Handles.
 */
class Task
{
public:
    Task(const std::string& program, const std::string& args, const std::string& working_directory) :
        m_task_id(generateRandomNumber(1, INT_MAX)),
        m_program(program),
        m_args(args),
        m_working_directory(working_directory),
        m_stdout_redirect(""),
        m_stderr_redirect("")
    {

    }

    Task(std::string program, std::string args) :
        m_task_id(generateRandomNumber(1, INT_MAX)),
        m_program(program),
        m_args(args),
        m_working_directory("."),
        m_stdout_redirect(""),
        m_stderr_redirect("")
    {

    }
    void setRedirectStdOut(const std::string& redirect)
    {
        m_stdout_redirect = redirect;
    }
    void setRedirectStdErr(const std::string& redirect)
    {
        m_stderr_redirect = redirect;
    }
    const std::string& program() const
    {
        return m_program;
    }
    const std::string& args() const
    {
        return m_args;
    }
    const std::string& working_directory() const
    {
        return m_working_directory;
    }
    const std::string& redirect_stdout() const
    {
        return m_stdout_redirect;
    }
    const std::string& redirect_stderr() const
    {
        return m_stderr_redirect;
    }
    const int& task_id() const
    {
        return m_task_id;
    }
private:
    int m_task_id;
    std::string m_program;
    std::string m_args;
    std::string m_working_directory;
    std::string m_stdout_redirect;
    std::string m_stderr_redirect;

    //friend functions
    friend std::ostream& operator<< (std::ostream& out, const Task& t)
    {
        out << "{"
            << "\"task_id\": " << t.task_id() << ", "
            << "\"program\": " << "\"" << t.program() << "\"" << ", "
            << "\"arguments\": " << "\"" << t.args() << "\"" << ", "
            << "\"working_directory\": " << "\"" << t.working_directory() << "\"" << ", "
            << "\"stdout_redirect\": " << "\"" << t.redirect_stdout() << "\""
            << "}";
        return out;
    }
};

/** \brief A Collection of Tasks that will be executed. <p>
 *         Each Job consists of a ordered set of Tasks that will
 *         be completed by one at a time. <br>
 *         Jobs has a priority in which they can be ordered by the Job Dispatcher.
 *         Each Job has a unique identifier called job_id which can be used to
 *         identify itself.
 */
class Job
{
public:
    enum Priority {VERY_LOW, LOW, NORMAL, HIGH, VERY_HIGH};

    Job(Priority p = NORMAL) :
        m_job_id(generateRandomNumber(1, INT_MAX)),
        m_priority(p)
    {
    }

    int priority() const
    {
        return m_priority;
    }
    int job_id() const
    {
        return m_job_id;
    }

    void addTask(Task task)
    {
        m_tasks.push(task);
    }

    void run()
    {
        std::cout << *this << "started" <<std::endl;
        while(!m_tasks.empty())
        {
            Task t = m_tasks.front();

            if(t.redirect_stdout().empty())
            {
                child c = execute(run_exe(t.program()),
                                  set_cmd_line(t.program() + " " + t.args()),
                                  start_in_dir(t.working_directory()),
                                  inherit_env()
                                 );
                //convert the return code to platform independent ret codes
                int exit_code = wait_for_exit(c);
                BOOST_PROCESS_EXITSTATUS(exit_code);
            }
            else
            {
                file_descriptor_sink sink(t.redirect_stdout());
                child c = execute(run_exe(t.program()),
                                  set_cmd_line(t.program() + " " + t.args()),
                                  bind_stdout(sink),
                                  start_in_dir(t.working_directory()),
                                  inherit_env()
                                 );
                //convert the return code to platform independent ret codes
                int exit_code = wait_for_exit(c);
                BOOST_PROCESS_EXITSTATUS(exit_code);
            }
            m_tasks.pop();
        }
    }

private:
    std::queue<Task> m_tasks;
    int m_job_id;
    Priority m_priority;

    ///TODOs:
    // 1) Attach a task completion handler to a task ID, call with exit code
    // 2) Add a task completion handler that can be called to do post processing
    // 3) Add a process creation exception when launch process fails.


    friend bool operator< (const Job& a, const Job& b)
    {
        return a.priority() < b.priority();
    }

    friend std::ostream& operator << (std::ostream& out, const Job& j)
    {
        out << "{\"job_id\": " << j.job_id() << ", "
            << "\"priority\": " << j.priority()
            << "}";

        return out;
    }

};
}

