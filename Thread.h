//azhe
//liuyuanzhe123@126.com
#ifndef NETFISH_THREAD_H
#define NETFISH_THREAD_H
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <pthread.h>

namespace netfish {

class Thread : public boost::noncopyable {
public:
    typedef boost::function<void ()> ThreadFunc;
    explicit Thread(const ThreadFunc& , const std::string& name = std::string());
    ~Thread();
    void start();
    void join();
    void quit();

private:
    //void * runInThread(void * obj);
    ThreadFunc func_;
    std::string name_;
    boost::shared_ptr<pid_t> tid_;
    bool started_;
    bool joined_;
    pthread_t pthreadId_;
};

pid_t getpid();
pid_t tid();
}
#endif  //NETFISH_THREAD_H
