//azhe
//liuyuanzhe123@126.com
#ifndef NETFISH_TIMESTAMP_H
#define NETFISH_TIMESTAMP_H

#include "copyable.h"

#include <stdint.h>
#include <string>

namespace netfish {

class Timestamp : public copyable {
public:
    Timestamp();
    //从指定的时间构造Timestamp
    explicit Timestamp(int64_t microSecondsSineEpoch);

    void swap(Timestamp & that)
    {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }

    std::string toString() const;
    std::string toFormattedString() const;

    bool valid() const {return microSecondsSinceEpoch_ > 0;}

    int64_t microSecondsSinceEpoch() const {return microSecondsSinceEpoch_;}

    //get now
    static Timestamp now();

    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

}
#endif  // NETFISH_TIMESTAMP_H
