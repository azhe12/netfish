//https://github.com/azhe12/netfish
//azhe liuyuanzhe123@126.com
#ifndef NETFISH_EVENTLOOP_H
#define NETFISH_EVENTLOOP_H
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>
#include "Timestamp.h"
#include "Thread.h"
#include "Channel.h"
#include "Atomic.h"
#include "Logging.h"
#include "MutexLock.h"
#include "TimerId.h"
#include "TimerQueue.h"
#include "Callbacks.h"

namespace netfish {
class EPoller;

//class EventLoop : private boost::noncopyable {
class EventLoop : boost::noncopyable {
public:
    typedef boost::function<void ()> Functor;
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    //将cb放到eventLoop相同的线程中去run
    //若当前已经在loop thread中，直接run
    void runInLoop(const Functor& cb);

    //queue cb到loop thread执行
    //其他thread去调用此函数为thread safe
    //会在poll结束之后执行所有pending的cb
    void queueInLoop(const Functor& cb);


    //internal use
    void updateChannel(Channel *);
    void removeChannel(Channel *);
    pid_t threadId() { return threadId_; }
    void assertInLoopThread()
    {
        if (!isInLoopThread()) {
            LOG_FATAL("EventLoop::assertInLoopThread - EventLoop %p was created in threadId_ = %d , current thread id = %d", 
                    this, threadId_, tid());
        }
    }

    bool isInLoopThread() const {
        return threadId_ == tid();
    }

    //wakeup loop poll来run pending cb
    void wakeup();

    //timers相关api
    //是thread safe
    //run callback at time
    TimerId runAt(const Timestamp& time, const TimerCallback& cb);

    //run callback after @delay second
    TimerId runAfter(double delay, const TimerCallback& cb);

    //run callback every @inverval seconds
    TimerId runEvery(double interval, const TimerCallback& cb);

    void cancel(TimerId& timerId);

private:
    void abortNotInLoopThread();
    void handleRead();  //wake up
    void doPendingFunctors();

    typedef std::vector<Channel *> ChannelList;

    //AtomicInteger<bool> looping_;
    //AtomicInteger<bool> quit_;
    bool looping_;
    bool quit_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    boost::scoped_ptr<EPoller> poller_;
    ChannelList activeChannels_;

    //for wakeup loop
    bool callingPendingFunctors_;
    int wakeupFd_;
    Channel wakeupChannel_;
    MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
    //for timer
    boost::scoped_ptr<TimerQueue> timerQueue_;

};
}
#endif  //NETFISH_EVENTLOOP_H
