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

std::string msg1;
std::string msg2;

void onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected()) {
        printf("onConnection(): tid %d new connection [%s] from %s\n",
                tid(),
                conn->name().c_str(),
                conn->peerAddress().toHostPort().c_str());
            conn->send(msg1);
            conn->send(msg2);
            conn->shutdown();
    } else {
        printf("onConnection(): tid=%d connection [%s] is down\n",
                tid(),
                conn->name().c_str());
    }
}

int main(int argc, char** argv)
{
    EventLoop loop;
    InetAddress listenAddr(9876);
    TcpServer server(&loop, listenAddr);
    int len1 = 500;
    int len2 = 100;
    
    if (argc > 2) {
        len1 = atoi(argv[1]);
        len2 = atoi(argv[2]);
    }
    if (argc > 3)
        server.setThreadNum(atoi(argv[1]));

    msg1.resize(len1);
    msg2.resize(len2);
    std::fill(msg1.begin(), msg1.end(), 'A');
    std::fill(msg2.begin(), msg2.end(), 'B');

    server.start();
    server.setConnectionCallback(onConnection);
    //server.setMessageCallback(onMessage);

    loop.loop();
    return 0;
}
