//azhe liuyuanzhe123@126.com
#include <string>
#include <string.h>
#include <iostream>
#include <stdio.h>

#include "Buffer.h"

using namespace netfish;
int main()
{
    Buffer buf;
    //int fd = open(, );
    int err = 0;
    ssize_t bytes = buf.readFd(0, &err);
    if (bytes < 0) {
        printf("readFd error %d:%s\n", err, strerror(err));
    }
    std::cout<<buf.retrieveAsString()<<std::endl;
}
