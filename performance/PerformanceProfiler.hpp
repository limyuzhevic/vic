#pragma once

#include "PerformanceProfiler.hpp"
#include <unordered_map>
#include <string>
#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <mutex>
#include <thread>
#include <fstream>
#include <numeric>
#include <functional>

namespace nlm {

class PerformanceProfiler {
public:
    struct ProfileData {
        std::string functionName;
        uint64_t callCount{0};
        double totalTime{0.0};
        double minTime{std::numeric_limits<double>::max()};
        double maxTime{0.0};
        double avgTime{0.0};
        double stdDev{0.0};
        
        ProfileData(const std::string& name) : functionName(name) {}
        
        void addSample(double time) {
            ++callCount;
            totalTime += time;
            minTime = std::min(minTime, time);
            maxTime = std::max(maxTime, time);
        }
        
        void computeStats() {
            if (callCount > 0) {
                avgTime = totalTime / callCount;
                
                // Calculate standard deviation
                double sumSqDiff = 0.0;
                // Note: For simplicity, we'll recalculate in update function
                // A full implementation would need to store individual samples
                stdDev = std::sqrt(sumSqDiff / callCount);
            }
        }
    };

    struct ProfilingSession {
        uint64_t sessionId;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::string sessionName;
        bool active{false};
        
        ProfilingSession(uint64_t id, const std::string& name) 
            : sessionId(id), sessionName(name) {}
    };

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    PerformanceProfiler();
    ~PerformanceProfiler();

    // Disable copying, enable moving
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    PerformanceProfiler(PerformanceProfiler&&) noexcept;
    PerformanceProfiler& operator=(PerformanceProfiler&&) noexcept;

    // Session management
    uint64_t startSession(const std::string& sessionName);
    void endSession(uint64_t sessionId);
    bool isSessionActive(uint64_t sessionId) const;
    
    // Core profiling
    void startProfiling(const std::string& functionName);
    void stopProfiling(const std::string& functionName);
    void startProfiling(uint64_t sessionId, const std::string& functionName);
    void stopProfiling(uint64_t sessionId, const std::string& functionName);
    
    // Thread-local profiling
    void startThreadLocalProfiling(const std::string& functionName);
    void stopThreadLocalProfiling(const std::string& functionName);
    
    // Direct timing
    double measureTime(const std::string& functionName, std::function<void()> operation);
    
    // Statistics
    const ProfileData* getProfileData(const std::string& functionName) const;
    const std::vector<ProfileData>& getAllProfileData() const;
    double getTotalExecutionTime() const;
    size_t getTotalFunctionCalls() const;
    
    // Session statistics
    const ProfilingSession* getSession(uint64_t sessionId) const;
    std::vector<uint64_t> getActiveSessions() const;
    
    // Performance metrics
    double getAverageSpikeRate() const;
    size_t getTotalSpikeCount() const;
    size_t getTotalNeuronCount() const;
    
    // Analysis
    std::string generateReport() const;
    void exportReport(const std::string& filename) const;
    void clear();
    
    // Thread safety
    void lock() const;
    void unlock() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
