//azhe
//liuyuanzhe123@126.com
#ifndef NETFISH_LOGGING_H
#define NETFISH_LOGGING_H
#include <stdio.h>
#include "Thread.h"
#include "Timestamp.h"

namespace netfish {

#define LOG_INFO(fmt, args...) (printf(  "%s %d INFO " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(),  ##args, __FILE__, __LINE__) )
#define LOG_DEBUG(fmt, args...) (printf(  "%s %d DEBUG " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), ##args, __FILE__, __LINE__) )
#define LOG_WARN(fmt, args...) (printf(  "%s %d WARN " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), ##args, __FILE__, __LINE__) )

}
#endif  //NETFISH_LOGGING_H
