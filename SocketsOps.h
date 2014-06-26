//azhe liuyuanzhe123@126.com
#ifndef NETFISH_SOCKETSOPS_H
#define NETFISH_SOCKETSOPS_H
#include <arpa/inet.h>
#include <endian.h>

namespace netfish {

namespace sockets {

inline uint64_t hostToNetwork64(uint64_t host64)
{
    return htobe64(host64);
}

inline uint32_t hostToNetwork32(uint32_t host32)
{
    return htonl(host32);
}

inline uint16_t hostToNetwork16(uint16_t host16)
{
    return htons(host16);
}

inline uint64_t networkToHost64(uint64_t net64)
{
    return be64toh(net64);
}

inline uint32_t networkToHost32(uint64_t net32)
{
    return ntohl(net32);
}

inline uint16_t networkToHost16(uint64_t net16)
{
    return ntohs(net16);
}

//create nonblocking Socket fd
//abort if failed.
int createNonBlockingOrDie();

int connect(int sockfd, const struct sockaddr_in & addr);
void bindOrDie(int sockfd, const struct sockaddr_in & addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in * addr);
void close(int sockfd);
int shutdownWrite(int sockfd);

//根据addr生成形如"ip:port" 的string
void toHostPort(char * buf, size_t size, 
                const struct sockaddr_in & addr);
//根据ip和port生成sockaddr_in
void fromHostPort(const char* ip, uint16_t port,
                  struct sockaddr_in* addr);

struct sockaddr_in getLocalAddr(int sockfd);
struct sockaddr_in getPeerAddr(int sockfd);

int getSocketError(int sockfd);
bool isSelfConnect(int sockfd);

}
}
#endif  //NETFISH_SOCKETSOPS_H
