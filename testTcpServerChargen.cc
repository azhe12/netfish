//azhe liuyuanzhe123@126.com

#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Callbacks.h"
#include "TcpConnection.h"
#include "Thread.h"

#include <string.h>
#include <stdio.h>
#include <string>
#include <algorithm>

using namespace netfish;

std::string msg;

void onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected()) {
        printf("onConnection(): tid %d new connection [%s] from %s\n",
                tid(),
                conn->name().c_str(),
                conn->peerAddress().toHostPort().c_str());

        conn->send(msg);
            
    } else {
        printf("onConnection(): tid=%d connection [%s] is down\n",
                tid(),
                conn->name().c_str());
    }
}

void onWriteComplete(const TcpConnectionPtr& conn)
{
    conn->send(msg);
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

    std::string line;
    for (int i = 33; i < 127; i++) {
        line.push_back(char(i));
    }
    line += line;

    for (int i = 0; i < 127- 33; i++) {
        msg += line.substr(i, 72);
    }

    if (argc > 1)
        server.setThreadNum(atoi(argv[1]));

    server.start();
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);
    server.setWriteCompleteCallback(onWriteComplete);

    loop.loop();
    return 0;
}
