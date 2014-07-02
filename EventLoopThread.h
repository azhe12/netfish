//azhe liuyuanzhe123@126.com
#ifndef NETFISH_EVENTLOOPTHREAD_H
#define NETFISH_EVENTLOOPTHREAD_H
#include <boost/noncopyable.hpp>

#include "Condition.h"
#include "MutexLock.h"
#include "Thread.h"

namespace netfish
{
class EventLoop;

//创建thread并且包含一个EventLoop
class EventLoopThread : boost::noncopyable
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
};

}
#endif  //NETFISH_EVENTLOOPTHREAD_H
