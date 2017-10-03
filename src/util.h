#ifndef HS_UTIL_H
#define HS_UTIL_H

#include <stdint.h>
#include <ctime>

namespace SS
{

    struct noncopyable {
        noncopyable() {};
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };

    struct util {
        static std::string format(const char* fmt, ...);
        static int64_t timeMicro();
        static int64_t timeMilli() { return timeMicro()/1000; }
        static int64_t steadyMicro();
        static int64_t steadyMilli() { return steadyMicro()/1000; }
        static std::string readableTime(time_t t);
    };

}

#endif //HS_UTIL_H
