//azhe liuyuanzhe123@126.com
#ifndef NETFISH_TCPCLIENT_H
#define NETFISH_TCPCLIENT_H

#include <boost/noncopyable.hpp>

#include "MutexLock.h"
#include "TcpConnection.h"
#include "Connector.h"

namespace netfish
{

class TcpClient : boost::noncopyable
{
public:
    TcpClient(EventLoop* loop, const InetAddress& serverAddr);
    ~TcpClient();

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const
    {
        MutexLockGuard lock(mutex_);
        return connection_;
    }

    bool isRetry() const;
    void enableRetry() { retry_ = true; }

    //following function not thread safe
    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

private:
    void newConnection(int sockfd);

    void removeConnection(const TcpConnectionPtr& conn);

    EventLoop* loop_;
    ConnectorPtr connector_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    bool retry_;
    bool connect_;
    int nextConnId_;
    mutable MutexLock mutex_;
    TcpConnectionPtr connection_;
};

}

#endif  //NETFISH_TCPCLIENT_H
