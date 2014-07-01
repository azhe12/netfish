//azhe liuyuanzhe123@126.com
#include <boost/bind.hpp>
#include <stdio.h>  //snprintf

#include "Logging.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "SocketsOps.h"
#include "TcpServer.h"

using namespace netfish;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr)
    : loop_(loop),
      name_(listenAddr.toHostPort()),
      acceptor_(new Acceptor(loop, listenAddr)),
      threadPool_(new EventLoopThreadPool(loop)),
      started_(false),
      nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(
            boost::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{}

void TcpServer::setThreadNum(int numThreads)
{
    assert(0 <= numThreads);
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
    if (!started_) {
        started_ = true;
        threadPool_->start();
    }

    if (!acceptor_->isListening()) {
        loop_->runInLoop(
                boost::bind(&Acceptor::listen, get_pointer(acceptor_)));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    LOG_INFO("TcpServer::newConnection [%s] - \
              new connection [%s] from %s",
              name_.c_str(),
              connName.c_str(),
              peerAddr.toHostPort().c_str());

    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    EventLoop* ioLoop = threadPool_->getNextLoop();
    TcpConnectionPtr conn(
            new TcpConnection(ioLoop, connName, sockfd, localAddr,peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setCloseCallback(
            boost::bind(&TcpServer::removeConnection, this, _1));
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);

    ioLoop->runInLoop(boost::bind(&TcpConnection::connectEstablished, conn.get()));
}

//由于closeCallback会发生在其他的loop thread,
//为了保证对connections_操作的thread safe,
//因此需要runInLoop来保证在baseIoLoop 中进行
void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->runInLoop(boost::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    loop_->assertInLoopThread();
    LOG_INFO("TcpServer::removeConnection [%s] - connection %s",
             name_.c_str(), conn->name().c_str());
    size_t n = connections_.erase(conn->name());
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
            boost::bind(&TcpConnection::connectDestroyed, conn));
}
