// PerformanceProfiler.hpp - Performance profiling and optimization tools
// Provides performance monitoring, bottleneck analysis, and optimization suggestions

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <algorithm>

namespace nlm {

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Static profiling methods
    static std::map<std::string, double> profileSimulation(
        std::shared_ptr<class Brain> brain,
        std::shared_ptr<class SimpleWorld> world,
        std::shared_ptr<class AgentBrain> agent,
        size_t steps = 1000);
    
    static std::map<std::string, double> optimizeConfiguration(
        std::shared_ptr<class Config> config);
    
    static std::map<std::string, double> benchmarkMemoryUsage(
        std::shared_ptr<class Brain> brain,
        std::shared_ptr<class SimpleWorld> world,
        std::shared_ptr<class AgentBrain> agent);
    
    static std::map<std::string, std::string> analyzePerformanceBottlenecks(
        std::shared_ptr<class Brain> brain,
        std::shared_ptr<class SimpleWorld> world,
        std::shared_ptr<class AgentBrain> agent);
    
    // Instance methods
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return profiling_; }
    
    std::map<std::string, double> getProfileResults() const;
    std::string getBottleneckAnalysis() const;
    
    // Configuration
    void setProfilingInterval(size_t interval) { profilingInterval_ = interval; }
    void enableDetailedAnalysis(bool enable) { detailedAnalysis_ = enable; }
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    bool profiling_ = false;
    size_t profilingInterval_ = 100;
    bool detailedAnalysis_ = false;
};

} // namespace nlm