//azhe liuyuanzhe123@126.com
#include <vector>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "Thread.h"
#include "MutexLock.h"
#include "Condition.h"

namespace netfish
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : boost::noncopyable
{
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads)
    {
        numThreads_ = numThreads;
    }

    void start();

    //按照循环的方式 在EventLoopThreadPool中取得一个EventLoop
    //如果pool为空，那么返回baseLoop_
    EventLoop* getNextLoop();

private:
    EventLoop* baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    boost::ptr_vector<EventLoopThread> threads_;
    std::vector<EventLoop*> loops_;
};

}
