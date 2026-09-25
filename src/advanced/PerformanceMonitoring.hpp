#ifndef NLM_PERFORMANCE_MONITORING_HPP
#define NLM_PERFORMANCE_MONITORING_HPP

#include <string>
#include <vector>
#include <chrono>
#include <memory>

namespace nlm {

class PerformanceMonitor {
private:
    struct PerformanceData {
        double cpuUsage;
        double memoryUsage;
        size_t totalSpikes;
        size_t totalSteps;
        std::chrono::steady_clock::time_point timestamp;
    };
    
    std::vector<PerformanceData> history;
    size_t maxHistorySize;
    std::chrono::steady_clock::time_point startTime;
    
public:
    PerformanceMonitor(size_t maxHistory = 100);
    
    void recordPerformance(double cpu, double memory, size_t spikes, size_t steps);
    void printPerformanceReport() const;
    double calculateAverageCPU() const;
    double calculateAverageMemory() const;
    size_t getCurrentStep() const;
    size_t getTotalSpikeCount() const;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_HPP