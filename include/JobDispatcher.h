#ifndef JOBDISPATCHER_H
#define JOBDISPATCHER_H

#include <vector>
#include <queue>

#include <boost/thread.hpp>

#include <Job.h>

namespace imrsv
{

class JobDispatcher
{
public:
    JobDispatcher(int num_workers, int max_queue) :
        m_num_workers(num_workers),
        m_max_queue_size(max_queue)
    {

    }

    ~JobDispatcher() {}

    bool push(const Job& new_job)
    {
        m_jobs.push(new_job);
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

    bool isEmpty() const
    {
        return m_jobs.empty();
    }


protected:
private:
    JobDispatcher();
    JobDispatcher(const JobDispatcher& jd);
    JobDispatcher& operator=(const JobDispatcher& id);

    int m_num_workers;
    int m_max_queue_size;
    std::priority_queue<Job> m_jobs;

};
}



#endif // JOBDISPATCHER_H
