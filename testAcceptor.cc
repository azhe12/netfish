#include <stdio.h>
#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"

using namespace netfish;
void newConnection(int sockfd, const InetAddress& addr)
{
    printf("new connection (fd:%d) from  %s\n", sockfd, addr.toHostPort().c_str());
    //close(sockfd);
}

int main()
{
    EventLoop loop;
    InetAddress listenAddr(9981);
    Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();
    loop.loop();
    return 0;
}
