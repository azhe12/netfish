//azhe liuyuanzhe123@126.com

#include <boost/bind.hpp>
#include <errno.h>

#include "Connector.h"
#include "EventLoop.h"
#include "Channel.h"
#include "SocketsOps.h"
#include "Logging.h"

using namespace netfish;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
    : loop_(loop),
      serverAddr_(serverAddr),
      connect_(false),
      state_(kDisconnected),
      retryDelayMs_(kInitRetryDelayMs)
{
    LOG_DEBUG("ctor [%p]", this);
}

Connector::~Connector()
{
    LOG_DEBUG("dtor [%p]", this);
    loop_->cancel(timerId_);
    assert(!channel_);
}

void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(boost::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop()
{
    loop_->assertInLoopThread();
    assert(state_ == kDisconnected);
    if (connect_) {
        connect();
    }
}

void Connector::connect()
{
    int sockfd = sockets::createNonBlockingOrDie();
    int ret = sockets::connect(sockfd, serverAddr_.getSockAddrInet());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;
        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;
        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_ERROR("Connector::connect error %d", savedErrno);
            sockets::close(sockfd);
            break;
        default:
            LOG_ERROR("Connector::connect unexpected error %d", savedErrno);
            sockets::close(sockfd);
            break;
    }
}

void Connector::connecting(int sockfd)
{
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setWriteCallback(
            boost::bind(&Connector::handleWrite, this));
    channel_->setErrorCallback(
            boost::bind(&Connector::handleError, this));

    channel_->enableWriting();
}

void Connector::handleWrite()
{
    LOG_TRACE(" state = %d", state_);
    
    if (state_ == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if (err) {
            LOG_WARN("Connector::handleWrite - SO_ERROR = %d %s",
                     err, strerror(err));
            retry(sockfd);
        } else if (sockets::isSelfConnect(sockfd)) {
            LOG_WARN("Connector::handleWrite - Self connect");
            retry(sockfd);
        } else {
            setState(kConnected);
            if (connect_) {
                if (newConnectionCallback_)
                    newConnectionCallback_(sockfd);
            } else {
                sockets::close(sockfd);
            }
        }
    } else {
        LOG_TRACE("do nothing");
        //abort();
    }
}

void Connector::handleError()
{
    assert(state_ == kConnecting);

    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);
    LOG_ERROR("Connector::handleError SO_ERROR = %d %s",
            err, strerror(err));
    retry(sockfd);
}

void Connector::retry(int sockfd)
{
    sockets::close(sockfd);
    setState(kDisconnected);
    if (connect_) {
        LOG_INFO("Connector::retry - Retry connecting to %s \
                  in %d milliseconds",
                  serverAddr_.toHostPort().c_str(),
                  retryDelayMs_);
        timerId_ = loop_->runAfter(retryDelayMs_/1000.0,
                                   boost::bind(&Connector::startInLoop, this));
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
    } else {
        LOG_DEBUG("do not connect");
    }
}

void Connector::restart()
{
    loop_->assertInLoopThread();
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

void Connector::stop()
{
    connect_ = false;
    loop_->cancel(timerId_);
}

int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    loop_->removeChannel(get_pointer(channel_));
    int sockfd = channel_->fd();
    //不能在此reset channel_, 因为处于Channel::handleEvent
    loop_->queueInLoop(boost::bind(&Connector::resetChannel, this));
    return sockfd;
}

void Connector::resetChannel()
{
    channel_.reset();
}

