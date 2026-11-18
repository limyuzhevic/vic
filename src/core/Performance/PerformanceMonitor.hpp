#pragma once

#include <string>
#include <map>

namespace nlm {

class PerformanceMonitor {
public:
    PerformanceMonitor();
    
    void start(const std::string& operation);
    void stop(const std::string& operation);
    double getDuration(const std::string& operation) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

struct TestResult {
    std::string name;
    double executionTime;
    bool success;
    std::map<std::string, std::string> metrics;
};

class ProgressReporter;
class OutputFormatter;

} // namespace nlm