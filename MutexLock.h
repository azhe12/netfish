//azhe liuyuanzhe123@126.com
#ifndef NETFISH_MUTEX_H
#define NETFISH_MUTEX_H
#include "Thread.h"
#include <pthread.h>
#include <boost/noncopyable.hpp>

namespace netfish {

class MutexLock : boost::noncopyable 
{
public:
    MutexLock()
        : holder_(0)
    {
        pthread_mutex_init(&mutex_, NULL);
    }
    ~MutexLock()
    {
        assert(holder_ == 0);
        pthread_mutex_destroy(&mutex_);
    }

    bool isLockedByThisThread()
    {
        return holder_ == tid();
    }

    void assertLocked()
    {
        assert(isLockedByThisThread());
    }

    //internal usage
    void lock()
    {
        pthread_mutex_lock(&mutex_);
        holder_ = tid();
    }

    void unlock()
    {
        holder_ = 0;
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t * getPthreadMutex()
    {
        return &mutex_;
    }
private:
    pthread_mutex_t mutex_;
    pid_t holder_;

};

class MutexLockGuard : boost::noncopyable 
{
public:
    MutexLockGuard(MutexLock& mutex)
        : mutex_(mutex)
    {
        mutex_.lock();
    }
    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};
}
#endif  //NETFISH_MUTEX_H
