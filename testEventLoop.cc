//对stdin的read事件进行关注并callback readFunc
#include "EventLoop.h"
#include "Timestamp.h"
#include "Channel.h"
#include "TimerId.h"
#include "TimerQueue.h"

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

using namespace netfish;

EventLoop* g_loop;
TimerId g_timerId;

void callBack()
{
    printf("callBack() in loop thread %d\n", tid());
}

void threadFunc()
{
    //sleep(3);
    sleep(1);
    //g_loop->assertInLoopThread();
    g_loop->runInLoop(callBack);
    printf("thread %d\n", tid());
}

void timerCallback()
{
    static int cnt = 0;
    if (cnt > 3)
        g_loop->cancel(g_timerId);
    printf("timerCallback cnt=%d\n", cnt);
    cnt++;
}

int main()
{
    EventLoop loop;
    g_loop = &loop;
    printf("loop thread %d\n", tid());
    Thread thread(threadFunc);
    thread.start();
    g_timerId = loop.runEvery(2, timerCallback);

    loop.loop();
    thread.join();
    return 0;
}
