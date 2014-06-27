//对stdin的read事件进行关注并callback readFunc
#include "EventLoop.h"
#include "Timestamp.h"
#include "Channel.h"
#include <iostream>
#include <unistd.h>
#include "stdio.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int fd = 0; //stdin

int setnonblocking(int fd) 
{ 
    if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)|O_NONBLOCK) == -1) { 
        return -1; 
    } 
    return 0; 
}
void readFunc(netfish::Timestamp & time)
{
    char buf[10+1];
    bzero(buf, sizeof(buf));
    int numRead = read(fd, buf, sizeof(buf)-1);
    if (numRead > 0) {
        buf[numRead] = '\0';
        printf("read %d bytes from stdin at %s, [%s] \n", 
                numRead, time.toFormattedString().c_str(), buf);
    } else if (numRead == 0) {
        printf("read empty!\n");
    } else {
        printf("read error!\n");
    }

}

int main()
{
    netfish::EventLoop loop;
    //常规文件不支持epoll, poll, select
    //fd = open("t.txt", O_RDWR | O_CREAT | O_NONBLOCK);
    //if (fd < 0) {
        //printf("open failed!, errno=%s\n", strerror(errno));
        //return -1;
    //}
    setnonblocking(fd);

    netfish::Channel channel(&loop, fd);
    channel.setReadCallback(readFunc);
    channel.enableReading();

    loop.loop();
    return 0;
}
