//azhe liuyuanzhe123@126.com

#include <boost/bind.hpp>

#include "EventLoop.h"
#include "EventLoopThread.h"
#include "EventLoopThreadPool.h"

using namespace netfish;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop)
    : baseLoop_(baseLoop),
      started_(false),
      numThreads_(0),
      next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
//nothing
}

void EventLoopThreadPool::start()
{
    assert(!started_);
    baseLoop_->assertInLoopThread();

    started_ = true;
    for (int i = 0; i < numThreads_; i++) {
        EventLoopThread* t = new EventLoopThread;
        EventLoop* loop = t->startLoop();
        loops_.push_back(loop);
        threads_.push_back(t);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    EventLoop* loop = baseLoop_;

    if (!loops_.empty()) {
        loop = loops_[next_];
        next_++;
        if (static_cast<size_t>(next_) >= loops_.size()) {
            next_ = 0;
        }
    }

    return loop;
}
