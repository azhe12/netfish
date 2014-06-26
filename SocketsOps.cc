//azhe liuyuanzhe123@126.com
#include "SocketsOps.h"
#include "Logging.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>  //snprintf
#include <string.h> //bzero
#include <sys/socket.h>
#include <unistd.h>

using namespace netfish;

namespace {
typedef struct sockaddr SA;
//将sockaddr_in类型转换为sockaddr
const SA* sockaddr_cast(const struct sockaddr_in* addr)
{
    return static_cast<const SA*>(static_cast<const void*>(addr));
}

SA* sockaddr_cast(struct sockaddr_in* addr)
{
    return static_cast<SA*>(static_cast<void*>(addr));
}

void setNonBlockAndCloseOnExec(int sockfd)
{
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);
    if (ret < 0) {
        LOG_ERROR("set fd:%d non-block faild!", sockfd);
        return;
    }

    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);
    if (ret < 0) {
        LOG_ERROR("set fd:%d FD_CLOEXEC faild!", sockfd);
        return;
    }
}

}

int sockets::createNonBlockingOrDie()
{
    int sockfd = ::socket(AF_INET,
                        SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 
                        IPPROTO_TCP);
    if (sockfd < 0) {
        LOG_FATAL("sockets::createNonblockingOrDie()");
    }
    return sockfd;
}

int sockets::connect(int sockfd, const struct sockaddr_in& addr)
{
    return ::connect(sockfd, sockaddr_cast(&addr), sizeof(addr));
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in& addr)
{
    int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
    if (ret < 0) {
        LOG_FATAL("sockets::bindOrDie()");
    }
}

void sockets::listenOrDie(int sockfd)
{
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        LOG_FATAL("sockets::listenOrDie()");
    }
}

int sockets::accept(int sockfd, struct sockaddr_in* addr)
{
    socklen_t addrlen = sizeof(*addr);
    //从listen的sockfd请求队列中取出第一个连接
    int connfd = ::accept4(sockfd, sockaddr_cast(addr),
                         &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        int savedErrno = errno;
        LOG_ERROR("sockets::accept()");
        switch (savedErrno) {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO:
            case EPERM:
            case EMFILE:    //达到单个进程打开描述符的上限
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
            //unexcepted errors
                LOG_FATAL("unexcepted error of sockets::accept errno: %d %s", savedErrno, strerror(savedErrno));
                break;
            default:
                LOG_FATAL("unknow error of sockets::accept errno: %d %s", savedErrno, strerror(savedErrno));
                break;

        }
    }
    return connfd;
}

void sockets::close(int sockfd)
{
    if (::close(sockfd) < 0) {
        LOG_ERROR("sockets::close");
    }
}

int sockets::shutdownWrite(int sockfd)
{
    int ret = 0;
    if ((ret = ::shutdown(sockfd, SHUT_WR)) < 0) {
        LOG_ERROR("sockets::shutdownWrite");
    }
    return ret;
}

void sockets::toHostPort(char * buf, size_t size,
                         const struct sockaddr_in& addr)
{
    char host[INET_ADDRSTRLEN] = "INVALID";
    ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
    uint16_t port = sockets::networkToHost16(addr.sin_port);
    snprintf(buf, size, "%s:%u", host, port);
}

void sockets::fromHostPort(const char *ip, uint16_t port,
                           struct sockaddr_in* addr)
{
    addr->sin_family = AF_INET;
    addr->sin_port = hostToNetwork16(port);
    if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
        LOG_ERROR("sockets::fromHostPort");
    }
}

struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof(localaddr));
    socklen_t addrlen = sizeof(localaddr);
    if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
        LOG_ERROR("sockets::getLocalAddr");
    }
    return localaddr;
}

//得到远端sockaddr, 必须在建立连接之后
struct sockaddr_in sockets::getPeerAddr(int sockfd)
{
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof(peeraddr));
    socklen_t addrlen = sizeof(peeraddr);
    if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0) {
        LOG_ERROR("sockets::getPeerAddr");
    }
    return peeraddr;
}

int sockets::getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = sizeof(optval);

    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return errno;
    } else {
        return optval;
    }
}

bool sockets::isSelfConnect(int sockfd)
{
    struct sockaddr_in localaddr = getLocalAddr(sockfd);
    struct sockaddr_in peeraddr = getPeerAddr(sockfd);
    return localaddr.sin_port == peeraddr.sin_port && 
           localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}
