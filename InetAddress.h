//azhe liuyuanzhe123@126.com
#ifndef NETFISH_INETADDR_H
#define NETFISH_INETADDR_H
#include "copyable.h"
#include <string>
#include <netinet/in.h>

namespace netfish {
//sockaddr_in的封装
class InetAddress {
public:
    //使用port来构造, 默认使用INADDR_ANY
    explicit InetAddress(uint16_t port);


    //使用ip和port构造
    //@ip 形如"127.0.0.1"
    InetAddress(const std::string& ip, uint16_t port);

    //使用sockaddr_in构造
    InetAddress(struct sockaddr_in addr)
        : addr_(addr)
    { }

    //转换为string， 形如"1.2.3.4:1000"
    std::string toHostPort() const;

    //默认copy/assignment即可

    const struct sockaddr_in& getSockAddrInet() const
    {
        return addr_;
    }

    void setSockAddrInet(const struct sockaddr_in& addr)
    {
        addr_ = addr;
    }
private:
    struct sockaddr_in addr_;
};

}
#endif  //NETFISH_INETADDR_H
