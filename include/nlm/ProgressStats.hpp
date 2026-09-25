#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include <unordered_map>

namespace nlm {

class ProgressTracker {
public:
    ProgressTracker();
    void startProgress(const std::string& operation, size_t total);
    void updateProgress(size_t current, const std::string& message = "");
    void finishProgress(const std::string& finalMessage = "");
    void setSpeed(double speed);
    std::string getProgressDisplay() const;
    
private:
    std::string operationName;
    size_t totalSteps;
    size_t currentStep;
    std::chrono::steady_clock::time_point startTime;
    std::atomic<double> speed;
    mutable std::mutex mutex;
};

class StatisticsCollector {
public:
    StatisticsCollector();
    void recordExecutionTime(const std::string& operation, std::chrono::nanoseconds time);
    void recordMemoryUsage(size_t memoryBytes);
    void recordResourceUsage(double cpuPercent, double memoryPercent);
    void recordTestResult(const std::string& testName, bool passed, std::chrono::nanoseconds time);
    std::string generateStatistics(const std::string& format = "console") const;
    std::string generateJSONStatistics() const;
    std::string generateXMLStatistics() const;
    std::string generateCSVStatistics() const;
    
private:
    struct OperationStats {
        std::string name;
        std::chrono::nanoseconds totalTime;
        std::chrono::nanoseconds minTime;
        std::chrono::nanoseconds maxTime;
        size_t callCount;
        double averageTime;
    };
    
    struct SystemStats {
        size_t peakMemory;
        double peakCPU;
        double peakMemoryPercent;
        std::chrono::steady_clock::time_point startTime;
        std::vector<OperationStats> operations;
        std::vector<std::pair<std::string, bool>> testResults;
    };
    
    SystemStats stats_;
    mutable std::mutex mutex;
};

} // namespace nlm
