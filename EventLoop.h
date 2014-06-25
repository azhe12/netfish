//https://github.com/azhe12/netfish
//azhe liuyuanzhe123@126.com
#ifndef NETFISH_EVENTLOOP_H
#define NETFISH_EVENTLOOP_H
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <vector>
#include "Timestamp.h"
#include "Thread.h"
#include "Channel.h"
#include "Atomic.h"
#include "Logging.h"

namespace netfish {
class EPoller;

class EventLoop : public boost::noncopyable {
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    void updateChannel(Channel *);
    void removeChannel(Channel *);
    void assertInLoopThread()
    {
        if (!isInLoopThread()) {
            LOG_FATAL("EventLoop::assertInLoopThread - EventLoop %p was created in threadId_ = %d , current thread id = %d", 
                    this, threadId_, tid());
        }
    }

    bool isInLoopThread() const {
        return threadId_ == tid();
    }
private:
    typedef std::vector<Channel *> ChannelList;
    //AtomicInteger<bool> looping_;
    //AtomicInteger<bool> quit_;
    bool looping_;
    bool quit_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    boost::scoped_ptr<EPoller> poller_;
    ChannelList activeChannels_;
    

};
}
#endif  //NETFISH_EVENTLOOP_H
