//azhe liuyuanzhe123@126.com
#ifndef NETFISH_TCPCONNECTION_H
#define NETFISH_TCPCONNECTION_H
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "Callbacks.h"
#include "Buffer.h"
#include "InetAddress.h"

namespace netfish
{

class Channel;
class EventLoop;
class Socket;

//TCP connection, for both client and server usage
class TcpConnection : boost::noncopyable,
                      public boost::enable_shared_from_this<TcpConnection>
{
public:
    //Construct a TcpConnection with a connected sockfd
    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() { return localAddr_; }
    const InetAddress& peerAddress() { return peerAddr_; }
    bool connected() const { return state_ == kConnected; }

    //thread safe
    //内部实现为sendInLoop
    void send(const std::string& message);
    //thread safe
    void shutdown();
    void setTcpNoDelay(bool on);

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

    //internel use only
    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }

    //called when TcpServer accepts a now connection
    void connectEstablished();

    //called when TcpServer has removed TcpConnection from its map
    void connectDestroyed();

private:
    enum StateE { kConnecting, 
                  kConnected, 
                  kDisconnecting,
                  kDisconnected, 
                };
    void setState(StateE s) { state_ = s; }
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(const std::string& message);
    void shutdownInLoop();

    EventLoop* loop_;
    std::string name_;
    StateE state_;  //FIXME:use atomic

    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback closeCallback_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
};

}
#endif  //NETFISH_TCPCONNECTION_H
