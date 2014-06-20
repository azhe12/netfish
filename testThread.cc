#include "Thread.h"
#include "Logging.h"
#include <unistd.h>
#include <stdio.h>

void func()
{
    LOG_INFO("Pid: %d run func", netfish::getpid());
}
int main()
{
    LOG_INFO("Pid: %d main", netfish::getpid());
    netfish::Thread thread(func);
    thread.start();
    thread.join();
}
