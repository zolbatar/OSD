#include <chrono>
#include <ctime>
#include <cstring>

std::chrono::steady_clock::time_point t1;
#ifndef CLION
#include <circle/timer.h>
extern CTimer *timer;
#endif

void ClockInit() {
    t1 = std::chrono::steady_clock::now();
}

int64_t GetClock() {
    auto t2 = std::chrono::steady_clock::now();
    auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    double d = time_span.count() / 10.0;
    return static_cast<int64_t>(d);
}

void CurrentTime(char *buffer) {
    std::time_t t = std::time(nullptr);
    std::strftime(buffer, 64, "%c", std::localtime(&t));
}
