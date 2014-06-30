//azhe liuyuanzhe123@126.com
#ifndef NETFISH_TIMER_H
#define NETFISH_TIMER_H
#include <boost/noncopyable.hpp>
#include "Timestamp.h"
#include "Atomic.h"
#include "Callbacks.h"

namespace netfish {

//internal for Timer event
class Timer : boost::noncopyable
{
public:
    Timer(const TimerCallback& cb, Timestamp when, double interval)
        : callback_(cb),
          expiration_(when),
          interval_(interval),
          repeat_(interval > 0.0),
          sequence_(s_numCreated_.incrementAndGet())
    {
    }

    void run() const
    {
        callback_();
    }

    Timestamp expiration() const { return expiration_; }
    bool repeat() const{ return repeat_; }
    int64_t sequence() const { return sequence_; }

    void restart(Timestamp now);

private:
    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;

    //记录Timer对象的个数
    static AtomicInt64 s_numCreated_;
};

}

#endif  //NETFISH_TIMER_H
