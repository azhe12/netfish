//azhe liuyuanzhe123@126.com
#ifndef NETFISH_TIMERQUEUE_H
#define NETFISH_TIMERQUEUE_H

#include <set>
#include <vector>
#include <boost/noncopyable.hpp>
#include "Timestamp.h"
#include "Thread.h"
#include "Callbacks.h"
#include "Channel.h"

namespace netfish {
class EventLoop;
class Timer;
class TimerId;

class TimerQueue : boost::noncopyable
{
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    //添加timer， 并在@when时刻调用@cb
    //interval>0.0, 则repeat
    //thread safe
    TimerId addTimer(const TimerCallback& cb,
                     Timestamp when,
                     double interval);
    void cancel(TimerId& timerId);

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);
    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);

    bool insert(Timer* timer);

    EventLoop* loop_;
    const int timerfd_;
    Channel timerfdChannel_;
    //timer list is sorted by expiration
    //存储所有timer, 包括已经到期的timer.
    //如果cancel(), 那么把相应的timer从中删除
    TimerList timers_;

    //for cancel()
    bool callingExpiredTimers_;
    //存储未到期timer
    ActiveTimerSet activeTimers_;
    ActiveTimerSet cancelingTimers_;
};
}
#endif  //NETFISH_TIMERQUEUE_H
