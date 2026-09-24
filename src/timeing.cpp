#include "timeing.h"
#include <chrono>

struct TimeStamp {
    std::chrono::time_point<std::chrono::steady_clock> time;
};

std::shared_ptr<TimeStamp> getNowTimeStamp() {
    return std::make_shared<TimeStamp>(std::chrono::steady_clock::now());
}

long long msSince(const std::shared_ptr<TimeStamp> &timeStamp) {
    std::chrono::time_point<std::chrono::steady_clock>  now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - timeStamp->time);
    return elapsed.count();
}
