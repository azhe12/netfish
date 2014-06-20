//azhe
//liuyuanzhe123@126.com
#ifndef NETFISH_LOGGING_H
#define NETFISH_LOGGING_H
#include <stdio.h>
#include "Timestamp.h"

namespace netfish {

#define LOG_INFO(fmt, args...) (printf(  "%s INFO " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), ##args, __FILE__, __LINE__) )
#define LOG_DEBUG(fmt, args...) (printf(  "%s DEBUG " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), ##args, __FILE__, __LINE__) )

}
#endif  //NETFISH_LOGGING_H
