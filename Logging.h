//azhe
//liuyuanzhe123@126.com
#ifndef NETFISH_LOGGING_H
#define NETFISH_LOGGING_H
#include <stdio.h>
#include "Thread.h"
#include "Timestamp.h"

namespace netfish {
extern int g_logLevel;

#define LOG_TRACE(fmt, args...) if (g_logLevel < 1) printf(  "%s %d TRACE %s " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(),  __func__, ##args, __FILE__, __LINE__)
#define LOG_DEBUG(fmt, args...) if (g_logLevel < 2) printf(  "%s %d DEBUG %s" fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), __func__, ##args, __FILE__, __LINE__)
#define LOG_INFO(fmt, args...) if (g_logLevel < 3) printf(  "%s %d INFO " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(),  ##args, __FILE__, __LINE__)
#define LOG_WARN(fmt, args...) (printf(  "%s %d WARN " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), ##args, __FILE__, __LINE__) )
#define LOG_ERROR(fmt, args...) (printf(  "%s %d ERROR " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), ##args, __FILE__, __LINE__) )
#define LOG_FATAL(fmt, args...) {printf(  "%s %d FATAL " fmt " - [%s:%d]\n", netfish::Timestamp::now().toFormattedString().c_str(), netfish::tid(), ##args, __FILE__, __LINE__);abort(); }

}
#endif  //NETFISH_LOGGING_H
