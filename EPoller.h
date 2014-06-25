//azhe liuyuanzhe123@126.com
#ifndef NETFISH_EPOLLER_H
#define NETFISH_EPOLLER_H
#include <map>
#include <vector>
#include <sys/epoll.h>
#include "EventLoop.h"
#include "Timestamp.h"

namespace netfish {
class EPoller : boost::noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;

    EPoller(EventLoop *);
    ~EPoller();
    Timestamp poll(int timeoutMs, ChannelList* activeChannels);
    //必须在loop thread
    void updateChannel(Channel * channel);
    void removeChannel(Channel * channel);
    void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }


private:
    //保存监控的event
    typedef std::vector<struct epoll_event> EventList;
    //fd到channel*的对应
    typedef std::map<int, Channel*> ChannelMap;

    static const int kInitEventListSize = 16;

    void fillActiveChannels(int numEvents, ChannelList * activeChannels) const;

    void update(int operation, Channel * channel);

    EventLoop * ownerLoop_;
    int epollfd_;
    EventList events_;
    ChannelMap channels_;
};
}
#endif
