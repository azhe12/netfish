//azhe liuyuanzhe123@126.com
#ifndef NETFISH_ACCEPTOR_H
#define NETFISH_ACCEPTOR_H
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include "Socket.h"
#include "Channel.h"

namespace netfish {

class EventLoop;
class InetAddress;

//accept new connection
class Acceptor : boost::noncopyable {
public:
    typedef boost::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const InetAddress& addr);

    //user callback
    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

    bool isListening() const { return listening_; }
    void listen();

private:
    //internel new connection callback
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listening_;
};
}
#endif  //NETFISH_ACCEPTOR_H
