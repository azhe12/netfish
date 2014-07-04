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
double g_totalSize = 0;       //kBytes
double g_sizePerInterval = 0;   //Bytes
double g_interval = 3;        //s
double g_totalTime = 0;

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

void onMessage(const TcpConnectionPtr& conn,
               Buffer* buf,
               Timestamp time)
{
#if 0
    printf("onMessage(): tid=%d receive %0.2f KBytes from connection [%s] at %s\n",
            tid(),
            buf->readableBytes() / 1024.0,
            conn->name().c_str(),
            time.toFormattedString().c_str());
#endif

    g_sizePerInterval += buf->readableBytes();
    conn->send(buf->retrieveAsString());
}

void calThroughPut()
{
    printf("calThroughPut(): ThroughPut %0.2f MB/S\n",
            g_sizePerInterval / (g_interval * 1024 * 1024));
    g_totalTime += g_interval;
    g_totalSize += g_sizePerInterval;
    g_sizePerInterval = 0;
}
//arg[1]: message size , KBytes
//arg[2]: server thread num
int main(int argc, char** argv)
{
    EventLoop loop;
    InetAddress listenAddr(9876);
    TcpServer server(&loop, listenAddr);
    size_t len = 16; //kBytes

    if (argc >= 2) {    //len
        len = atoi(argv[1]);
    }

    msg.resize(len * 1024);
    std::fill(msg.begin(), msg.end(), 'A');

    printf("message size %lu\n", msg.size());

    if (argc >= 3) {    //thread num
        server.setThreadNum(atoi(argv[2]));
    }

    server.start();
    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    loop.runEvery(g_interval, calThroughPut);

    loop.loop();
    return 0;
}
