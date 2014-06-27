//azhe liuyuanzhe123@126.com
#ifndef NETFISH_CONDITION_H
#define NETFISH_CONDITION_H
#include "Mutex.h"
#include <boost/noncopyable.hpp>
#include <pthread.h>
#include <errno.h>

namespace netfish {

class Condition : boost::noncopyable
{
public:
    explicit Condition(MutexLock& mutex)
        : mutex_(mutex)
    {
        pthread_cond_init(&pcond_, NULL);
    }

    ~Condition()
    {
        pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        pthread_cond_wait(&pcond_, mutex_.getPthreadMutex());
    }

    //return true if timeout, false otherwise 
    bool waitForSeconds(int seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += seconds;
        return ETIMEDOUT == pthread_cond_timeout(&pcond_, mutex_.getPthreadMutex(), &abstime);
    }

    void notify()
    {
        pthread_cond_signal(&pcond_);
    }

    void notifyAll()
    {
        pthread_cond_broadcast(&pcond_);
    }

private:
    MutexLock& mutex_;
    pthread_cond_t pcond_;

};
}

#endif  //NETFISH_CONDITION_H
