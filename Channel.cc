//https://github.com/azhe12/netfish
//azhe liuyuanzhe123@126.com
#include "Channel.h"
#include "EventLoop.h"
#include <sys/poll.h>

namespace netfish {

Channel::Channel(EventLoop * loop, int fd)
    : loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    eventHandling_(false)
{
}

Channel::~Channel()
{
    assert(!eventHandling_);
}
//添加channel到loop，或 更改已存在loop中channel的关注event
void Channel::update()
{
    loop_->updateChannel(this);
}

//依据revents_的变化, 处理fd相关的user callback
void Channel::handleEvent(Timestamp reciveTime)
{
    eventHandling_ = true;
    if (revents_ & POLLNVAL) {
        LOG_WARN("Channel::handle_event() POLLNVAL");
    }
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        LOG_WARN("Channel::handle_event() POLLUP");
        if (closeCallback_) closeCallback_();
    }

    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) errorCallback_();
    }

    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) readCallback_(reciveTime);
    }
    if (revents_ & POLLOUT) {
        if (writeCallback_) writeCallback_();
    }
    eventHandling_ = false;
}
}
