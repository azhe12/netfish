//azhe liuyuanzhe123@126.com
#ifndef NETFISH_ATOMIC_H
#define NETFISH_ATOMIC_H
#include <boost/noncopyable.hpp>
#include <stdint.h>

namespace netfish {
template <typename T>
class AtomicInteger : boost::noncopyable {
public:
    AtomicInteger() : val_(0) {}

    T get()
    {
        return __sync_fetch_and_add(&val_, 0);
        //return __sync_val_compare_and_swap(&val_, 0, 0);
    }

    T getAndAdd(T x)
    {
        return __sync_fetch_and_add(&val_, x);
    }

    T addAndGet(T x)
    {
        return __sync_add_and_fetch(&val_, x);
    }

    //先增
    T incrementAndGet()
    {
        return addAndGet(1);
    }

    //先减
    T decrementAndGet()
    {
        return addAndGet(-1);
    }

    //后增
    T getAndIncrement()
    {
        return getAndAdd(1);
    }

    //后减
    T getAndDecrement()
    {
        return getAndAdd(-1);
    }

    void increment()
    {
        getAndIncrement();
    }

    void decrement()
    {
        getAndDecrement();
    }

private:
    T val_;
};

typedef AtomicInteger<int32_t> AtomicInt32;
typedef AtomicInteger<int64_t> AtomicInt64;

}
#endif  //NETFISH_ATOMIC_H
