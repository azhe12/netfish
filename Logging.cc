//azhe liuyuanzhe123@126.com

#include "Logging.h"
using namespace netfish;

namespace netfish
{
int initLogLevel()
{
    char* levelStr = ::getenv("NETFISH_LOG_LEVEL");
    int level = 2;
    if (levelStr) {
        int newLevel = atoi(levelStr);
        //TRACE:0 DEBUG:1 INFO:2
        if (newLevel >= 0 && newLevel <= 2)
            level = newLevel;
    }
    return level;
}
int g_logLevel = initLogLevel();
}
