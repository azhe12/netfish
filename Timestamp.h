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

    //get and invalid time
    static Timestamp invalid();

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

//返回high和low相差的seconds
inline double timeDifference(Timestamp high, Timestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

inline Timestamp addTime(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}
#endif  // NETFISH_TIMESTAMP_H
