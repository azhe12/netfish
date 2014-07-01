//azhe liuyuanzhe123@126.com
#ifndef NETFISH_CALLBACKS_H
#define NETFISH_CALLBACKS_H
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include "Timestamp.h"

namespace netfish {
class Buffer;
class TcpConnection;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef boost::function<void ()> TimerCallback;
typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef boost::function<void (const TcpConnectionPtr&,
                              Buffer*,
                              Timestamp)> MessageCallback;
typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;


}
#endif  //NETFISH_CALLBACKS_H
