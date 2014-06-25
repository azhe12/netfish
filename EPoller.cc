//azhe liuyuanzhe123@126.com
#include "EPoller.h"
#include "Channel.h"
#include "Logging.h"
#include <assert.h>
#include <poll.h>
#include <sys/epoll.h>
#include <errno.h>

using namespace netfish;

namespace {
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
}

EPoller::EPoller(EventLoop * loop)
    : ownerLoop_(loop),
      epollfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)
{
    if (epollfd_ < 0)
        LOG_FATAL("EPoller::EPoller create epollfd failed");
}

EPoller::~EPoller()
{
    close(epollfd_);
}

Timestamp EPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = epoll_wait(epollfd_, 
                                &*(events_.begin()),
                                static_cast<int>(events_.size()),
                                timeoutMs);
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
        LOG_TRACE("%d events happended", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) {
        LOG_TRACE("nothing happended");
    } else {
        LOG_ERROR("EPoller::poll()");
    }
    return now;
}

void EPoller::fillActiveChannels(int numEvents, ChannelList * activeChannels) const
{
    assert(static_cast<size_t>(numEvents) <= events_.size());
    for (int i = 0; i < numEvents; i++) {
        Channel * channel = static_cast<Channel*>(events_[i].data.ptr);
        int fd = channel->fd();
        ChannelMap::const_iterator it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPoller::updateChannel(Channel * channel)
{
    assertInLoopThread();
    LOG_TRACE("fd = %d events = %d", channel->fd(), channel->events());
    const int index = channel->index();
    if (index == kNew || index == kDeleted) {   //new or delete
        int fd = channel->fd();
        if (index == kNew) {    //添加新的channel到EPoller
            assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        } else {    //删除
            assert(channels_.find(fd) != channels_.end());
            assert(channels_[fd] == channel);
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {    //modify已存在的channel
        int fd = channel->fd();
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if (channel->isNoneEvent()) {   //events_空
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoller::removeChannel(Channel * channel)
{
    assertInLoopThread();
    int fd = channel->fd();
    LOG_TRACE("fd=%d", fd);
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent()); //??
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = channels_.erase(fd);
    assert(n == 1);
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPoller::update(int operation, Channel * channel)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        LOG_ERROR("EPoller::update - epoll_ctl op=%d fd=%d failed!",
                    operation, fd);
    }
}













