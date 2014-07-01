#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include <stdio.h>

using namespace netfish;

TimerId g_timerId;
EventLoop* g_loop;

void cancelLoop()
{
    g_loop->quit();
}

int main()
{
    EventLoop loop;
    EventLoopThreadPool threadPool(&loop);
    printf("main thread: %d\n", tid());

    int n = 10;
    threadPool.setThreadNum(n);
    threadPool.start();
    for (int i = 0; i < n; i++) {
        EventLoop* l = threadPool.getNextLoop();
        printf("thread %d: pid: %d\n", i, l->threadId());
    }

    g_loop = &loop;
    //loop.runAfter(3, cancelLoop);
    loop.loop();
    return 0;
}
