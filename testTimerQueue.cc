#include "EventLoop.h"
#include "Timestamp.h"
#include "TimerQueue.h"
#include "TimerId.h"

using namespace netfish;

static TimerQueue* g_tQ;
static TimerId* g_tId;

void timeout()
{
    static int cnt = 0;
    if (cnt > 3)
        g_tQ->cancel(*g_tId);

    printf("timeout cnt=%d \n", cnt);
    cnt++;
}

int main()
{
    EventLoop loop;
    Timestamp when = addTime(Timestamp::now(), 3);
    TimerQueue tQ(&loop);
    g_tQ = &tQ;

    TimerId tId = tQ.addTimer(timeout, when, 1);
    g_tId = &tId;

    loop.loop();
    return 0;
}
