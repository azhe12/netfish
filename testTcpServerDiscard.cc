#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "TcpConnection.h"
#include "Thread.h"

#include <string.h>
#include <stdio.h>

using namespace netfish;

void onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected()) {
        printf("onConnection(): tid %d new connection [%s] from %s\n",
                tid(),
                conn->name().c_str(),
                conn->peerAddress().toHostPort().c_str());
    } else {
        printf("onConnection(): tid=%d connection [%s] is down\n",
                tid(),
                conn->name().c_str());
    }
}

void onMessage(const TcpConnectionPtr& conn,
               Buffer* buf,
               Timestamp time)
{
    printf("onMessage(): tid=%d receive %lu bytes from connection [%s] at %s\n",
            tid(),
            buf->readableBytes(),
            conn->name().c_str(),
            time.toFormattedString().c_str());

    buf->retrieveAll();
}

int main(int argc, char** argv)
{
    EventLoop loop;
    InetAddress listenAddr(9876);
    TcpServer server(&loop, listenAddr);
    
    if (argc > 1)
        server.setThreadNum(atoi(argv[1]));

    server.start();
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    loop.loop();
    return 0;
}
