//azhe liuyuanzhe123@126.com
#ifndef NETFISH_TCPSERVER_H
#define NETFISH_TCPSERVER_H
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "Callbacks.h"
#include "TcpConnection.h"

namespace netfish
{

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer
{
public:
    TcpServer(EventLoop* loop, const InetAddress& listenAddr);
    ~TcpServer();


    //called before start
    //@numThreads:
    //  - 0 means all I/O in loop's thread
    //  - 1 means all I/O in another thread
    //  - N means a thread poll with N threads, new connections
    //    are assigned on a roud-robin basis
    void setThreadNum(int numThreads);

    //start server and listening
    void start();

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);

    void removeConnection(const TcpConnectionPtr& conn);

    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop* loop_;   //acceptor loop
    const std::string name_;
    boost::scoped_ptr<Acceptor> acceptor_;
    boost::scoped_ptr<EventLoopThreadPool> threadPool_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    bool started_;
    int nextConnId_;
    ConnectionMap connections_;
};
}
#endif  //NETFISH_TCPSERVER_H
