//azhe liuyuanzhe123@126.com

#define __STDC_LIMIT_MACROS
#include "TimerQueue.h"
#include "EventLoop.h"
#include "Timer.h"
#include "TimerId.h"
#include "Logging.h"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <sys/timerfd.h>

namespace netfish
{
namespace detail
{
int createTimerFd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, 
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        LOG_ERROR("detail::createTimerFd");
    }
    return timerfd;
}

struct timespec howMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.microSecondsSinceEpoch()
                           - Timestamp::now().microSecondsSinceEpoch();
    if (microseconds < 100) {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    LOG_TRACE("TimerQueue::handleRead() %lu at %s", 
              howmany, now.toString().c_str());
    if (n != sizeof howmany) {
        LOG_ERROR("TimerQueue::handleRead() read %lu bytes instead of 8", n);
    }
}
//
//设置expirtation
void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof newValue);
    bzero(&oldValue, sizeof oldValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret) {
        LOG_ERROR("timerfd_settime()");
    }
}
}

using namespace netfish;
using namespace netfish::detail;

TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop),
      timerfd_(detail::createTimerFd()),
      timerfdChannel_(loop, timerfd_),
      timers_(),
      callingExpiredTimers_(false)
{
    //timer expire callback
    timerfdChannel_.setReadCallback(
            boost::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}
//dtor:
//1. close timerfd
//2. 删除timers中的Timer对象
TimerQueue::~TimerQueue()
{
    ::close(timerfd_);
    for (TimerList::iterator it = timers_.begin();
            it != timers_.end(); it++) {
        delete it->second;
    }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb,
                             Timestamp when,
                             double interval)
{
    Timer* timer = new Timer(cb, when, interval);
    loop_->runInLoop(
        boost::bind(&TimerQueue::addTimerInLoop, this, timer));
    LOG_TRACE("TimerQueue::addTimer timer sequence %ld", 
            timer->sequence());
    return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId& timerId)
{
    loop_->runInLoop(
        boost::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if (earliestChanged) {
        resetTimerfd(timerfd_, timer->expiration());
    }
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    LOG_TRACE("TimerQueue::cancelInLoop timer sequence:%ld",
            timerId.sequence_);
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    ActiveTimerSet::iterator it = activeTimers_.find(timer);
    if (it != activeTimers_.end()) {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1);
        delete it->first;   //FIXME:不用delete
        activeTimers_.erase(it);
    } else if (callingExpiredTimers_) { 
    //如果正在handle expired callback, 那么应将timer记录到cancelingTimers_中
    //避免timer的repeat属性而重新将timer insert到queue中
        cancelingTimers_.insert(timer);
    }
    assert(timers_.size() == activeTimers_.size());
}

//若是第一个timer返回true, 否则false
bool TimerQueue::insert(Timer* timer)
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if (it == timers_.end() || when < it->first) {
        earliestChanged = true;
    }

    {
        std::pair<TimerList::iterator, bool> result =
            timers_.insert(Entry(when, timer));
        assert(result.second);
    }
    {
        std::pair<ActiveTimerSet::iterator, bool> result =
            activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
        assert(result.second);
    }

    assert(timers_.size() == activeTimers_.size());
    return earliestChanged;
}

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    //user callback
    for (std::vector<Entry>::iterator it = expired.begin();
            it != expired.end(); it++) {
        it->second->run();
    }
    callingExpiredTimers_ = false;
    
    reset(expired, now);
}

//找出now之前超时的timers
//然后将超时的timer从timers_和activeTimers_中erase
std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    assert(timers_.size() == activeTimers_.size());
    std::vector<Entry> expired;
    Entry nowEntry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(nowEntry);
    assert(it == timers_.end() || now < it->first);
    std::copy(timers_.begin(), it, back_inserter(expired));

    //将到期的timers从timers_和activeTimers_中erase
    timers_.erase(timers_.begin(), it);

    BOOST_FOREACH(Entry entry, expired) {
        ActiveTimer timer(entry.second, entry.second->sequence());
        ssize_t n = activeTimers_.erase(timer);
        assert(n == 1);
    }
    assert(timers_.size() == activeTimers_.size());
    return expired;
}

//1. 到期timer中是否有repeat
//2. 找到下个未到期timer
void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire;
    for (std::vector<Entry>::const_iterator it = expired.begin();
            it != expired.end(); it++) {
        ActiveTimer timer(it->second, it->second->sequence());
        //到期timer是否为repeat
        //如果timer被cancel()那么即使有repeat也要delete
        if (it->second->repeat() &&
                cancelingTimers_.find(timer) == cancelingTimers_.end()) {
            //重设expire为now+expiration, 然后insert
            it->second->restart(now);
            insert(it->second);
        } else {    //delete, can use
            delete it->second;
        }
    }

    if (!timers_.empty()) {
        nextExpire = timers_.begin()->second->expiration();
    }
    if (nextExpire.valid()) {
        resetTimerfd(timerfd_, nextExpire);
    }
}

}
