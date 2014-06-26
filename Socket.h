//azhe liuyuanzhe123@126.com
#ifndef NETFISH_SOCKET_H
#define NETFISH_SOCKET_H

#include <boost/noncopyable.hpp>
//#include "InetAddress.h"

namespace netfish {
class InetAddress;

//对sockfd的封装, dtor会close sockfd
class Socket : boost::noncopyable {
public:
    explicit Socket(int sockfd)
        : sockfd_(sockfd)
    { }
    ~Socket();

    int fd() const { return sockfd_; }

    //如果addr被占用，abort()
    void bindAddress(const InetAddress& localaddr);
    //如果addr被占用，abort()
    void listen();

    //1. 如果成功, 返回非负的accept到的sockfd, 并且peerAddr被赋值
    //2. 如果失败，返回-1
    int accept(InetAddress* peerAddr);

    //SO_REUSEADDR
    int setReuseAddr(bool on);

    int shutdownWrite();

    //TCP_NODELAY disable/enable Nagle's algorithm
    int setTcpNoDelay(bool on);
private:
    const int sockfd_;
};
}
#endif  //NETFISH_SOCKET_H
