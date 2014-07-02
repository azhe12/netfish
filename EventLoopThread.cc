//azhe liuyuanzhe123@126.com
#include <boost/bind.hpp>

#include "EventLoop.h"
#include "EventLoopThread.h"

using namespace netfish;

EventLoopThread::EventLoopThread()
    : loop_(NULL),
      exiting_(false),
      thread_(boost::bind(&EventLoopThread::threadFunc, this)),
      mutex_(),
      cond_(mutex_)
{
}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    loop_->quit();
    thread_.join();
}

EventLoop* EventLoopThread::startLoop()
{
    thread_.start();

    {
        MutexLockGuard lock(mutex_);
        while (loop_ == NULL) {
            cond_.wait();
        }
    }
    return loop_;
}

//new loop thread
void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    loop.loop();
}
