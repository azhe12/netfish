//azhe liuyuanzhe123@126.com

#include <boost/bind.hpp>
#include <stdio.h>

#include "TcpClient.h"
#include "Connector.h"
#include "EventLoop.h"
#include "SocketsOps.h"
#include "Logging.h"
#include "MutexLock.h"

using namespace netfish;

namespace netfish
{
namespace detail
{

void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn)
{
    loop->queueInLoop(boost::bind(&TcpConnection::connectDestroyed, conn));
}

}
}

TcpClient::TcpClient(EventLoop* loop,
                     const InetAddress& serverAddr)
    : loop_(loop),
      connector_(new Connector(loop, serverAddr)),
      retry_(false),
      connect_(true),
      nextConnId_(1)
{
    connector_->setNewConnectionCallback(
            boost::bind(&TcpClient::newConnection, this, _1));
    LOG_INFO("TcpClient::TcpClient[%p] - connector %p",
              this, get_pointer(connector_));
}

TcpClient::~TcpClient()
{
    LOG_INFO("TcpClient::~TcpClient[%p] - connector %p",
              this, get_pointer(connector_));

    TcpConnectionPtr conn;
    {
        MutexLockGuard lock(mutex_);
        conn = connection_;
    }

    if (conn) {
        CloseCallback cb = boost::bind(&detail::removeConnection,
                                       loop_, _1);
        loop_->runInLoop(
                boost::bind(&TcpConnection::setCloseCallback, conn, cb));
    }
}

void TcpClient::connect()
{
    LOG_INFO("TcpClient::connect[%p] - connecting to %s",
              this, connector_->serverAddress().toHostPort().c_str());
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect()
{
    connect_ = false;
    {
        MutexLockGuard lock(mutex_);
        if (connection_) {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop()
{
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
    loop_->assertInLoopThread();
    //server addr
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toHostPort().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    TcpConnectionPtr conn(new TcpConnection(loop_,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
            boost::bind(&TcpClient::removeConnection, this, _1));
    {
        MutexLockGuard lock(mutex_);
        connection_ = conn;
    }

    conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());

    {
        MutexLockGuard lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(boost::bind(&TcpConnection::connectDestroyed, conn));
    if (retry_ && connect_) {
        LOG_INFO("TcpClient::connect[%p] - Reconnecting to %s",
                  this, connector_->serverAddress().toHostPort().c_str());
        connector_->restart();
    }
}
