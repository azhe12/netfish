//azhe
//liuyuanzhe123@126.com
#include "Thread.h"
#include <sys/syscall.h>
#include <sys/unistd.h>
#include <boost/weak_ptr.hpp>

namespace netfish {


//依据系统调用来得到pid
pid_t getpid()
{
    return static_cast<pid_t>(syscall(SYS_gettid));
}

class ThreadData : public boost::noncopyable{
    public:
        typedef netfish::Thread::ThreadFunc ThreadFunc;
        explicit ThreadData(const ThreadFunc & func, boost::shared_ptr<pid_t> & tid)
            :func_(func), 
            wkTid_(tid)
        {}

        boost::weak_ptr<pid_t> wkTid_;
        ThreadFunc func_;
};

Thread::Thread(const ThreadFunc& func, const std::string & name)
    :func_(func),
    name_(name),
    tid_(new pid_t(0)),
    started_(false),
    pthreadId_(0)
{
    
}
Thread::~Thread()
{
    if (started_ && !joined_) { //如果线程没有被join(), 那么执行detach避免线程成为zoombie
        pthread_detach(pthreadId_);
    }
}

//void * Thread::runInThread(void * obj)
void * runInThread(void * obj)
{
    ThreadData * data = reinterpret_cast<ThreadData *>(obj);
    data->func_();
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    ThreadData * data = new ThreadData(func_, tid_);
    if (pthread_create(&pthreadId_, NULL, &runInThread, (void *)data)) {
        started_ = false;
        delete data;
        abort();
    }
}

void Thread::join()
{
    if (pthreadId_) {
        pthread_join(pthreadId_, NULL);
        joined_ = true;
    }
}

}
