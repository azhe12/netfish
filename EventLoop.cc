//azhe liuyuanzhe123@126.com
#include <signal.h>
#include <sys/eventfd.h>
#include <boost/bind.hpp>

#include "Logging.h"
#include "EventLoop.h"
#include "EPoller.h"
#include "Channel.h"
#include "TimerId.h"
#include "TimerQueue.h"


using namespace netfish;

__thread EventLoop * t_loopInThisThread = NULL; //线程全局变量，在线程创建时初始化
const int kPollTimeMs = 10000;  //10s

static int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOG_ERROR("Failed to createEventfd");
        abort();
    }
    return evtfd;
}

//忽略SIGPIPE
//由于多次对关闭的sockfd进行write会发生SIGPIPE, 导致server终止
class IgnoreSigPipe
{
public:
    IgnoreSigPipe()
    {
        ::signal(SIGPIPE, SIG_IGN);
    }
};

IgnoreSigPipe initObj;

EventLoop::EventLoop() 
    : looping_(false),
      quit_(false),
      threadId_(tid()),
      poller_(new EPoller(this)),
      callingPendingFunctors_(false),
      wakeupFd_(createEventfd()),
      wakeupChannel_(this, wakeupFd_),
      timerQueue_(new TimerQueue(this))
{
    LOG_TRACE("created %p in thread %d", this, threadId_);
    if (t_loopInThisThread) {   //一个thread中最多一个loop
        LOG_FATAL("Another EventLoop %p exisits in this thread %d", 
                t_loopInThisThread, threadId_);
    } else {
        t_loopInThisThread = this;
    }
    wakeupChannel_.setReadCallback(
            boost::bind(&EventLoop::handleRead, this));
    wakeupChannel_.enableReading();
}

EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = NULL;
    ::close(wakeupFd_);
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    
    while (!quit_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (ChannelList::iterator it = activeChannels_.begin();
                it != activeChannels_.end(); it++) {
            (*it)->handleEvent(pollReturnTime_);
        }
        doPendingFunctors();
    }
    LOG_TRACE("%p stop looping", this);
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor& cb)
{
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor& cb)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }

    //如果不在loopthread或者已经在执行pendingFunctor,则wakeup
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::wakeup() write %ld bytes instead of 8", n);
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    //read will clear eventfd buffer
    ssize_t n = ::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::handleRead() reads %ld bytes instead of 8", n);
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    //LOG_TRACE("EventLoop::doPendingFunctors()");

    //swap到stack空间来 减少加锁时间
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.swap(functors);
    }

    for (size_t i = 0; i < functors.size(); i++) {
        functors[i]();
    }
    callingPendingFunctors_ = true;
}

void EventLoop::updateChannel(Channel * channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel * channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}

TimerId EventLoop::runAt(const Timestamp& time, const TimerCallback& cb)
{
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb)
{
    return timerQueue_->addTimer(cb, addTime(Timestamp::now(), delay), 0.0);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb)
{
    return timerQueue_->addTimer(cb, addTime(Timestamp::now(), interval), interval);
}

void EventLoop::cancel(TimerId& timerId)
{
    timerQueue_->cancel(timerId);
}
