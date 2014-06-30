//azhe liuyuanzhe123@126.com
#ifndef NETFISH_TIMERID_H
#define NETFISH_TIMERID_H
#include "copyable.h"

namespace netfish
{
class Timer;

class TimerId: public copyable
{
public:
    TimerId(Timer* timer = NULL, int64_t seq = 0)
        : timer_(timer),
          sequence_(seq)
    {
    }

    friend class TimerQueue;
private:
    Timer* timer_;
    int64_t sequence_;
};
}
#endif  //NETFISH_TIMERID_H
