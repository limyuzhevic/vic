#pragma once

#include <string>
#include <unordered_map>

namespace nlm {

class PerformanceBenchmark {
public:
    PerformanceBenchmark();
    ~PerformanceBenchmark() = default;
    
    // Start timing a specific operation
    void startTiming(const std::string& operationName);
    
    // Stop timing and record the result
    double stopTiming(const std::string& operationName);
    
    // Get timing for an operation
    double getTiming(const std::string& operationName) const;
    
    // Print benchmark results
    void printBenchmarkResults() const;
    
    // Get total benchmark time
    double getTotalTime() const;
    
    // Reset all benchmarks
    void reset();
    
private:
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> startTimes_;
    std::unordered_map<std::string, double> elapsedTimes_;
    std::chrono::high_resolution_clock::time_point sessionStart_;
};

} // namespace nlm