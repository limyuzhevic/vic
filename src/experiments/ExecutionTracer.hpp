#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

    class ExecutionTracer {
    public:
        struct ExecutionStep {
            std::chrono::system_clock::time_point timestamp;
            std::string operation;
            std::string threadId;
            std::string file;
            int line;
            std::string function;
            double executionTimeMs;
            double memoryUsageMB;
            std::unordered_map<std::string, std::string> parameters;
            std::unordered_map<std::string, std::string> results;
            bool success;
            std::string errorMessage;
        };

        struct MemoryCheckpoint {
            std::string name;
            std::chrono::system_clock::time_point timestamp;
            double memoryUsageMB;
            double memoryGrowthMB;
            std::string systemState;
            std::unordered_map<std::string, std::string> metadata;
        };

        struct SystemState {
            std::chrono::system_clock::time_point timestamp;
            std::string operation;
            std::unordered_map<std::string, std::string> stateData;
            double cpuUsagePercent;
            double memoryUsagePercent;
            std::vector<std::string> activeComponents;
            std::string threadId;
        };

    public:
        ExecutionTracer();
        ~ExecutionTracer();

        // Step-by-step execution tracing
        void traceExecution(const std::string& operation,
                           const std::unordered_map<std::string, std::string>& parameters = {},
                           const std::string& file = "", int line = 0,
                           const std::string& function = "");

        void traceExecutionWithResult(const std::string& operation,
                                     const std::unordered_map<std::string, std::string>& parameters,
                                     const std::unordered_map<std::string, std::string>& results,
                                     bool success,
                                     const std::string& errorMessage = "",
                                     const std::string& file = "", int line = 0,
                                     const std::string& function = "");

        // Memory checkpoint analysis
        void takeMemoryCheckpoint(const std::string& name = "default",
                                  const std::unordered_map<std::string, std::string>& metadata = {});
        std::vector<MemoryCheckpoint> getMemoryCheckpoints(const std::string& name = "") const;
        double compareMemoryUsage(const std::string& checkpoint1, const std::string& checkpoint2) const;
        std::string analyzeMemoryGrowth(const std::string& checkpointName = "") const;

        // System state comparison
        void traceSystemState(const std::string& operation,
                            const std::unordered_map<std::string, std::string>& stateData,
                            double cpuUsagePercent = 0.0,
                            double memoryUsagePercent = 0.0,
                            const std::vector<std::string>& activeComponents = {});

        std::vector<SystemState> getSystemStateHistory(const std::string& operation = "") const;
        std::string compareSystemStates(const std::string& operation) const;

        // Performance bottleneck detection
        void analyzePerformanceBottlenecks();
        std::string detectBottlenecks() const;
        std::string getBottleneckReport() const;

        // Execution flow analysis
        std::vector<std::string> getExecutionSequence() const;
        std::unordered_map<std::string, int> getOperationFrequency() const;
        std::string analyzeExecutionFlow() const;

        // Query methods
        std::vector<ExecutionStep> getRecentSteps(size_t limit = 100) const;
        std::vector<ExecutionStep> getStepsByOperation(const std::string& operation, size_t limit = 100) const;
        std::vector<ExecutionStep> getFailedSteps(size_t limit = 50) const;
        std::string getExecutionSummary() const;

        // Data management
        void clear();
        void clearOldSteps(std::chrono::hours olderThanHours = std::chrono::hours(24));
        size_t getStepCount() const;

        // Export
        std::string exportToJSON() const;
        std::string exportToCSV() const;
        std::string generateExecutionReport() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;

        // Thread safety
        mutable std::mutex mutex_;

        // Helper methods
        std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) const;
        std::string getCurrentThreadId() const;
        double getCurrentMemoryUsageMB() const;
    };

} // namespace nlm
