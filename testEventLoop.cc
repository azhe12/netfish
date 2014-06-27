//对stdin的read事件进行关注并callback readFunc
#include "EventLoop.h"
#include "Timestamp.h"
#include "Channel.h"
#include <iostream>
#include <unistd.h>
#include "stdio.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>

using namespace netfish;

EventLoop* g_loop = NULL;

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

int main()
{
    EventLoop loop;
    g_loop = &loop;
    printf("loop thread %d\n", tid());
    Thread thread(threadFunc);
    thread.start();

    loop.loop();

    thread.join();
    return 0;
}
