#ifndef JOBDISPATCHER_H
#define JOBDISPATCHER_H

#include <vector>
#include <queue>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/heap/priority_queue.hpp>

#include <Job.h>

namespace imrsv
{

class JobDispatcher
{
public:
    JobDispatcher(int num_workers = 4, int max_queue = 100) :
        m_max_queue_size(max_queue),
        m_num_workers(num_workers),
        m_idle_workers(num_workers),
        m_running(true)
    {
        //initialize the thread group
        for ( std::size_t i = 0; i < m_num_workers; ++i )
        {
            m_workers.create_thread( boost::bind( &JobDispatcher::pollForWork, this ) ) ;
        }
    }

    ~JobDispatcher()
    {
        // Set running flag to false then notify all threads.
        {
            JobQueueLock lock(m_job_queue_mutex);
            m_running = false;
            m_condition.notify_all();
        }

        try
        {
            m_workers.join_all();
        }
        // Suppress all exceptions.
        catch ( ... ) {}
    }

    bool addJob(const Job& new_job)
    {
        return push(new_job);
    }

    bool removeJob(const Job& remove_job)
    {
        return false;
    }

    bool removeJob(const int job_id)
    {
        return false;
    }

    bool isEmpty() const
    {
        return m_jobs.empty();
    }

    int idleWorkers() const
    {
        return m_idle_workers;
    }

    int pendingJobs() const
    {
        return m_jobs.size();
    }

private:

    bool push(const Job& new_job)
    {
        JobQueueLock lock(m_job_queue_mutex);

        //check if the max queue is exceeded, reject new jobs if
        //size is greater than max queue size allowed
        if(m_jobs.size() < m_max_queue_size)
        {
            m_jobs.push(new_job);
            m_condition.notify_one();
        }
        else
            return false;

        return true;
    }

    bool pop()
    {
        m_jobs.pop();
        return true;
    }

    Job top() const
    {
        return m_jobs.top();
    }

    //Job Dispatcher are not supposed to be cloned.
    JobDispatcher();
    JobDispatcher(const JobDispatcher& jd);
    JobDispatcher& operator=(const JobDispatcher& id);

    void pollForWork()
    {
        while(m_running)
        {
            JobQueueLock lock(m_job_queue_mutex);

            while(m_jobs.empty() && m_running)
            {
                //wait on condition where new jobs are available
                m_condition.wait(lock);
            }

            //termination of JobDispatcher all threads should exit
            if(!m_running)
                break;

            //create a local scoped copy of the Job to run,
            //this Job clone will be deleted immediately after the Job
            //completes
            {
                Job j = m_jobs.top();
                m_jobs.pop();

                //unlock the job queue so other threads can access it
                lock.unlock();

                //run the job
                try
                {
                    m_idle_workers--;
                    j.run();
                }
                catch(...) {} //suppress all exceptions
            }

            m_idle_workers++;
        }
    }

    //print out the Job Schedule
    friend std::ostream& operator<< (std::ostream& out, const JobDispatcher& jd)
    {
        out << "{";

        for (boost::heap::priority_queue<Job>::const_iterator it = jd.m_jobs.begin(), end = jd.m_jobs.end(); it != end; ++it)
            out << *it << ", ";

        out << "}";

        return out;
    }

    std::size_t m_max_queue_size;
    boost::heap::priority_queue<Job> m_jobs;

    //typedefs
    typedef boost::unique_lock<boost::mutex> JobQueueLock;

    //stuff to maintain thread pool
    boost::thread_group m_workers;
    std::size_t m_num_workers;
    boost::atomic_int m_idle_workers;
    boost::mutex m_job_queue_mutex;
    boost::condition_variable m_condition;
    bool m_running;

};
}



#endif // JOBDISPATCHER_H
