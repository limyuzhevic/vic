#pragma once

#include <chrono>
#include <string>

namespace nlm {

/**
 * Timer utility for performance measurement
 * Uses C++11 steady clock for high-resolution timing
 */
class Timer {
public:
    Timer() : startTime(std::chrono::steady_clock::now()) {}
    
    // Get elapsed time in milliseconds
    double elapsedMs() const {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        return static_cast<double>(duration.count());
    }
    
    // Get elapsed time in microseconds
    double elapsedUs() const {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return static_cast<double>(duration.count());
    }
    
    // Reset timer
    void reset() {
        startTime = std::chrono::steady_clock::now();
    }
    
private:
    std::chrono::steady_clock::time_point startTime;
};
}