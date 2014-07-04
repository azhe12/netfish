//azhe
//liuyuanzhe123@126.com
#ifndef NETFISH_LOGGING_H
#define NETFISH_LOGGING_H
#include <stdio.h>
#include "Thread.h"
#include "Timestamp.h"

namespace netfish {

#define LOG_TRACE(fmt, args...) (printf(  "%s %d TRACE %s " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(),  __func__, ##args, __FILE__, __LINE__) )
#define LOG_DEBUG(fmt, args...) (printf(  "%s %d DEBUG %s" fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), __func__, ##args, __FILE__, __LINE__) )
#define LOG_INFO(fmt, args...) (printf(  "%s %d INFO " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(),  ##args, __FILE__, __LINE__) )
#define LOG_WARN(fmt, args...) (printf(  "%s %d WARN " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), ##args, __FILE__, __LINE__) )
#define LOG_ERROR(fmt, args...) (printf(  "%s %d ERROR " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), ##args, __FILE__, __LINE__) )
#define LOG_FATAL(fmt, args...) {printf(  "%s %d FATAL " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), ##args, __FILE__, __LINE__);abort(); }

}
#endif  //NETFISH_LOGGING_H
