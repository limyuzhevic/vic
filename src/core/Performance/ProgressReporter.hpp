#pragma once

#include <string>
#include <map>

namespace nlm {

class ProgressReporter {
public:
    ProgressReporter();
    
    void update(const std::string& message, int current, int total);
    void logTestResults(const std::vector<TestResult>& results);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class OutputFormatter {
public:
    static std::string formatDuration(double seconds);
    static std::string formatMemory(size_t bytes);
    static std::string formatPercentage(double value);
};

} // namespace nlm