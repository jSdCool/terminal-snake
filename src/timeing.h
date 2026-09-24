#pragma once
#include <memory>

struct TimeStamp;

std::shared_ptr<TimeStamp> getNowTimeStamp();

long long msSince(const std::shared_ptr<TimeStamp> &timeStamp);