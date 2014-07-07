#include "TcpServer.h"
#include "Atomic.h"
#include "Logging.h"
#include "Thread.h"
#include "EventLoop.h"
#include "InetAddress.h"

#include <boost/bind.hpp>

#include <utility>

#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

using namespace netfish;

void onConnection(const TcpConnectionPtr& conn)
{
  if (conn->connected())
  {
    conn->setTcpNoDelay(true);
  }
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp)
{
  conn->send(buf->retrieveAsString());
}

int main(int argc, char* argv[])
{

  if (argc < 4)
  {
    fprintf(stderr, "Usage: server <address> <port> <threads>\n");
  }
  else
  {
    //max open file number
    int maxFile = 65536;
    struct rlimit rl;
    rl.rlim_cur = rl.rlim_max = maxFile;
    if (::setrlimit(RLIMIT_NOFILE, &rl) == -1)
    {
      perror("setrlimit");
    }

    LOG_INFO("tid = %d", tid());

    const char* ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    InetAddress listenAddr(ip, port);
    int threadCount = atoi(argv[3]);

    EventLoop loop;

    TcpServer server(&loop, listenAddr);

    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    if (threadCount > 1)
    {
      server.setThreadNum(threadCount);
    }

    server.start();

    loop.loop();
  }
}

