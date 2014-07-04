#include "EventLoop.h"
#include "Connector.h"
#include "InetAddress.h"

#include <stdio.h>
#include <unistd.h>
using namespace netfish;

EventLoop *g_loop;
void onConnection(int sockfd)
{
    printf("connected\n");
    g_loop->quit();
}

int main()
{
    EventLoop loop;
    g_loop = &loop;

    InetAddress serverAddr(9876);
    Connector connector(&loop, serverAddr);
    connector.setNewConnectionCallback(onConnection);
    connector.start();

    loop.loop();
}
