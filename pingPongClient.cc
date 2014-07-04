//azhe liuyuanzhe123@126.com
#include "EventLoop.h"
#include "TcpClient.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "Callbacks.h"

#include <stdio.h>

using namespace netfish;

void onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected()) {
        printf("onConnection connection[%s] - from %s to %s",
                conn->name().c_str(),
                conn->localAddress().toHostPort().c_str(),
                conn->peerAddress().toHostPort().c_str());
    } else {
        printf("onConnection connection[%s] is down",
                conn->name().c_str());
    }
}

void onMessage(const TcpConnectionPtr& conn,
               Buffer* buf,
               Timestamp time)
{
    printf("onMessage(): receive %0.2f KBytes from connection[%s] at %s\n",
            buf->readableBytes() / 1024.0,
            conn->name().c_str(),
            time.toFormattedString().c_str());

    //printf("onMessage(): [%s]", buf->retrieveAsString().c_str());
    conn->send(buf->retrieveAsString());
}

int main()
{
    EventLoop loop;
    InetAddress serverAddr(9876);
    TcpClient client(&loop, serverAddr);

    client.setConnectionCallback(onConnection);
    client.setMessageCallback(onMessage);

    client.connect();
    loop.loop();
}
