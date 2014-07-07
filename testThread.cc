#include "Logging.h"
#include "Thread.h"
#include <iostream>
#include "Atomic.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <vector>
//int sum = 0;

using namespace netfish;
netfish::AtomicInt32 sum;

void func()
{
    int loop = 2;
    for (int i = 0; i < loop; i++) {
        LOG_INFO("sum = %d", sum.getAndIncrement());
        //LOG_INFO("i = %d ", i);
    }
}

int main(int argc, char ** argv)
{
    int numT = 1000;
    if (argc > 1)
        numT = atoi(argv[1]);

    std::vector<netfish::Thread *> queue;

    for (int i = 0; i < numT; i++) {
        netfish::Thread * t = new netfish::Thread(func);
        queue.push_back(t);
        t->start();
    }
    for (int i = 0; i < numT; i++) {
        queue[i]->join();
        delete queue[i];
    }
}
