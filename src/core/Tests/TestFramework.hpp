#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class ConsoleLogger;
class Config;
class SimulationClock;
class Logger;

// Test result structure
struct TestResult {
    std::string name;
    double executionTime;
    bool success;
    std::map<std::string, std::string> metrics;
};

// Progress reporting
class ProgressReporter {
public:
    ProgressReporter();
    
    void update(const std::string& message, int current, int total);
    void logTestResults(const std::vector<TestResult>& results);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Output formatting
class OutputFormatter {
public:
    static std::string formatDuration(double seconds);
    static std::string formatMemory(size_t bytes);
    static std::string formatPercentage(double value);
};

} // namespace nlm