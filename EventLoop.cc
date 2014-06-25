//azhe liuyuanzhe123@126.com
#include "Logging.h"
#include "EventLoop.h"
#include "EPoller.h"
using namespace netfish;

__thread EventLoop * t_loopInThisThread = NULL; //线程全局变量，在线程创建时初始化
const int kPollTimeMs = 10000;  //10s

EventLoop::EventLoop() 
    : looping_(false),
      quit_(false),
      threadId_(tid()),
      poller_(new EPoller(this))
{
    LOG_TRACE("EventLoop created %p in thread %d", this, threadId_);
    if (t_loopInThisThread) {   //一个thread中最多一个loop
        LOG_FATAL("Another EventLoop %p exisits in this thread %d", 
                t_loopInThisThread, threadId_);
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;
    
    while (!quit_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (ChannelList::iterator it = activeChannels_.begin();
                it != activeChannels_.end(); it++) {
            (*it)->handleEvent(pollReturnTime_);
        }
    }
    LOG_TRACE("EventLoop %p stop looping", this);
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
}

void EventLoop::updateChannel(Channel * channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel * channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}
