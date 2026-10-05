// Stress testing test mode
// Extreme condition testing for NLM

#pragma once

#include "experiments/Experiment.hpp"
#include "core/PerformanceProfiler.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace nlm {

// Stress test metrics
struct StressTestMetrics {
    double cpuUsage; // percentage (0-100)
    double memoryUsage; // in bytes
    double memoryPressure; // percentage (0-100)
    uint64_t spikeCount; // number of spikes generated
    double simulationSpeed; // simulation steps per second
    double resourceExhaustionTime; // when resources reached limit
    bool memoryLeakDetected;
    bool cpuSaturation; // CPU fully utilized
    std::chrono::system_clock::time_point timestamp;
    
    StressTestMetrics() : cpuUsage(0), memoryUsage(0), memoryPressure(0),
                         spikeCount(0), simulationSpeed(0), resourceExhaustionTime(0),
                         memoryLeakDetected(false), cpuSaturation(false) {}
};

// Stress test experiment
class StressTestExperiment : public Experiment {
public:
    StressTestExperiment(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~StressTestExperiment() = default;
    
    // Experiment interface
    bool setup() override;
    bool run() override;
    bool cleanup() override;
    
    // Stress test specific methods
    bool runMemoryStressTest(float loadFactor = 0.9, std::chrono::seconds duration = std::chrono::seconds(300));
    bool runCpuStressTest(float loadFactor = 0.8, std::chrono::seconds duration = std::chrono::seconds(300));
    bool runCombinedStressTest(float cpuLoad = 0.7, float memoryLoad = 0.8, 
                               std::chrono::seconds duration = std::chrono::seconds(300));
    
    // Results management
    std::vector<StressTestMetrics> getMetrics() const { return metrics_; }
    StressTestMetrics getLatestMetrics() const { return metrics_.empty() ? StressTestMetrics() : metrics_.back(); }
    
    // Output generation
    std::string generateReport() const;
    bool exportResults(const std::string& filepath, const std::string& format = "json") const;
    
protected:
    std::vector<StressTestMetrics> metrics_;
    std::shared_ptr<PerformanceProfiler> profiler_;
    StressTestConfig stressConfig_;
};

} // namespace nlm
