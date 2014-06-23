//azhe
//liuyuanzhe123@126.com
#include "Thread.h"
#include "Logging.h"
#include <sys/syscall.h>
#include <sys/unistd.h>
#include <boost/weak_ptr.hpp>

namespace netfish {

//用线程变量来cache tid
__thread pid_t t_cachedTid;

pid_t tid()
{
    if (!t_cachedTid) {
        t_cachedTid = getpid();
    }
    return t_cachedTid;
}

//依据系统调用来得到pid
pid_t getpid()
{
    return static_cast<pid_t>(syscall(SYS_gettid));
}

class ThreadData : public boost::noncopyable{
    public:
        typedef netfish::Thread::ThreadFunc ThreadFunc;
        explicit ThreadData(const ThreadFunc & func, boost::shared_ptr<pid_t> & tid)
            : wkTid_(tid),
            func_(func)
        {}

        boost::weak_ptr<pid_t> wkTid_;
        ThreadFunc func_;
};

Thread::Thread(const ThreadFunc& func, const std::string & name)
    :func_(func),
    name_(name),
    tid_(new pid_t(0)),
    started_(false),
    joined_(false),
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
    boost::shared_ptr<pid_t> shTid = data->wkTid_.lock();
    if (shTid) {
        *shTid = netfish::tid();    //记录tid
        shTid.reset();
    }
    data->func_();
    delete data;
    return NULL;
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
    assert(started_);
    assert(!joined_);
    if (pthreadId_) {
        pthread_join(pthreadId_, NULL);
        joined_ = true;
    }
}

}
