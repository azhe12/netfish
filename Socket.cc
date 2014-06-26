//azhe liuyuanzhe123@126.com
#include "InetAddress.h"
#include "Socket.h"
#include "SocketsOps.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>

using namespace netfish;

Socket::~Socket()
{ 
    ::close(sockfd_);
}
void Socket::bindAddress(const InetAddress& addr)
{
    sockets::bindOrDie(sockfd_, addr.getSockAddrInet());
}

void Socket::listen()
{
    sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof addr);
    int connfd = sockets::accept(sockfd_, &addr);
    if (connfd >= 0) {
        peeraddr->setSockAddrInet(addr);
    }
    return connfd;
}

int Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                 &optval, sizeof optval);
    return ret;
}

int Socket::shutdownWrite()
{
    sockets::shutdownWrite(sockfd_);
}

int Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                           &optval, sizeof optval);
    return ret;
}

