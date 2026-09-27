#pragma once

#include "CallProfiler.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <cmath>

namespace nlm {

class CallProfiler {
public:
    struct CallRecord {
        std::string functionName;
        std::string moduleName;
        std::chrono::high_resolution_clock::time_point timestamp;
        double duration;
        size_t callStackDepth;
        std::vector<std::string> callStack;
        uint64_t threadId;
        
        CallRecord(const std::string& name, const std::string& module, double duration)
            : functionName(name), moduleName(module), duration(duration), 
              callStackDepth(0), threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())) {
            timestamp = std::chrono::high_resolution_clock::now();
        }
    };

    CallProfiler();
    ~CallProfiler();

    // Disable copying, enable moving
    CallProfiler(const CallProfiler&) = delete;
    CallProfiler& operator=(const CallProfiler&) = delete;
    CallProfiler(CallProfiler&&) noexcept;
    CallProfiler& operator=(CallProfiler&&) noexcept;

    // Profiling control
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const;

    // Record function calls
    void recordFunctionCall(const std::string& functionName, const std::string& moduleName,
                           double duration, const std::vector<std::string>& callStack);

    // Query
    std::vector<CallRecord> getRecentCalls(size_t count) const;
    std::vector<CallRecord> getCallsByFunction(const std::string& functionName) const;
    std::vector<CallRecord> getCallsByModule(const std::string& moduleName) const;

    // Analysis
    double getAverageCallTime(const std::string& functionName) const;
    size_t getCallCount(const std::string& functionName) const;
    double getTotalExecutionTime() const;

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
