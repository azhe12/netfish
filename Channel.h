//https://github.com/azhe12/netfish
//azhe liuyuanzhe123@126.com
#ifndef NETFISH_CHANNEL_H
#define NETFISH_CHANNEL_H
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <sys/poll.h>
#include "Timestamp.h"

namespace netfish {

class EventLoop;
class Channel : public boost::noncopyable {
public:
    typedef boost::function<void ()> EventCallback;
    typedef boost::function<void (Timestamp)> ReadEventCallback;
    Channel(EventLoop * loop, int fd);
    ~Channel();
    
    void setReadCallback(const ReadEventCallback & cb)
    {
        readCallback_ = cb;
    }

    void setWriteCallback(const EventCallback & cb)
    {
        writeCallback_ = cb;
    }
    void setErrorCallback(const EventCallback & cb)
    {
        errorCallback_ = cb;
    }
    void setCloseCallback(const EventCallback & cb)
    {
        closeCallback_ = cb;
    }

    int index() const { return index_; }
    void set_index(int index) { index_ = index; }
    int events() const {return events_;}
    void set_revents(int revt) { revents_ = revt; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    void update();
    int fd() const {return fd_;}
    EventLoop * ownerLoop() { return loop_;}

    //改变关注事件，并且更新loop中相应channel的状态
    void enableReading() { events_ |= kReadEvent; update();}
    void enableWriting() { events_ |= kWriteEvent; update();}
    void disableReading() { events_ &= ~(kReadEvent); update();}
    void disableWriting() { events_ &= ~(kWriteEvent); update();}

    void handleEvent(Timestamp reciveTime);

private:
    static const int kNoneEvent = 0;
    static const int kReadEvent = POLLIN | POLLPRI;
    static const int kWriteEvent = POLLOUT;

    EventLoop* loop_;
    int fd_;
    int events_;
    int revents_;
    int index_;     //poller使用
    bool eventHandling_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};

}
#endif  //NETFISH_CHANNEL_H
