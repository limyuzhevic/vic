// Utilities.hpp
// Common utility functions and helper classes for NLM

#ifndef NLM_UTILITIES_HPP
#define NLM_UTILITIES_HPP

#include <string>
#include <chrono>

namespace nlm {

class PerformanceTimer;

class Utilities {
public:
    // String formatting
    static std::string formatNumber(size_t number);
    static std::string formatFloat(float value, int precision = 3);
    
    // Progress reporting
    static void printProgress(int current, int total, const std::string& label);
    
    // Timing utilities
    static PerformanceTimer startTimer();
    static double getElapsedSeconds(const PerformanceTimer& timer);
    
    // System information
    static std::string getSystemInfo();
    static std::string getVersionString();
};

// Performance timer class
class PerformanceTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool running;
    
public:
    PerformanceTimer();
    void start();
    void stop();
    double elapsed() const;
    bool isRunning() const;
    
    friend class Utilities;
};

} // namespace nlm

#endif // NLM_UTILITIES_HPP