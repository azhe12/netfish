//azhe liuyuanzhe123@126.com
#ifndef NETFISH_CALLBACKS_H
#define NETFISH_CALLBACKS_H
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "Timestamp.h"

namespace netfish {
class Buffer;

typedef boost::function<void ()> TimerCallback;
}
#endif  //NETFISH_CALLBACKS_H
