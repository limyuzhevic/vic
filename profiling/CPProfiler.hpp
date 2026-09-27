#pragma once

#include "CPProfiler.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <cmath>

namespace nlm {

class CPProfiler {
public:
    struct CPUProfile {
        std::chrono::high_resolution_clock::time_point timestamp;
        double cpuUsage;
        double userTime;
        double systemTime;
        std::vector<double> perCoreUsage;
        std::vector<size_t> threadCPUs;
        
        CPUProfile() : cpuUsage(0.0), userTime(0.0), systemTime(0.0) {}
    };

    CPProfiler();
    ~CPProfiler();

    // Disable copying, enable moving
    CPProfiler(const CPProfiler&) = delete;
    CPProfiler& operator=(const CPProfiler&) = delete;
    CPProfiler(CPProfiler&&) noexcept;
    CPProfiler& operator=(CPProfiler&&) noexcept;

    // CPU monitoring
    void startCPUProfiling();
    void stopCPUProfiling();
    bool isCPUProfiling() const;
    
    void updateCPUStats();
    CPUProfile getCurrentCPUStats() const;
    std::vector<CPUProfile> getCPUHistory(size_t count) const;

    // Bottleneck detection
    void detectCPUBottlenecks();
    std::vector<std::string> getCPUHotspots() const;
    double getAverageCPUUsage() const;

    // Integration
    void setBrain(Brain* brain);
    void setNeuralDynamics(NeuralDynamics* dynamics);

    // Statistics
    std::string getStatistics() const;
    std::string generateReport() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
