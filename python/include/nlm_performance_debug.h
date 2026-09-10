#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <numeric>
#include <limits>
#include <cmath>
#include <algorithm>

// Forward declarations to avoid including all brain headers
namespace nlm {
    class Brain;
    class NeuronId;
}

namespace performance {
    // Performance timing utilities
    class PerformanceTimer {
    public:
        PerformanceTimer() : start_time(std::chrono::high_resolution_clock::now()) {}
        
        double elapsed() const {
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double, std::milli>(end - start_time).count();
        }
        
        void reset() {
            start_time = std::chrono::high_resolution_clock::now();
        }
        
        // Python-friendly methods
        double getElapsed() const { return elapsed(); }
        void resetTimer() { reset(); }
        
    private:
        std::chrono::high_resolution_clock::time_point start_time;
    };
    
    // Memory monitoring utilities
    class MemoryMonitor {
    public:
        static size_t getAvailableMemory() {
            // Platform-specific implementation would go here
            return std::numeric_limits<size_t>::max();
        }
        
        static size_t getProcessMemoryUsage() {
            // Platform-specific implementation would go here
            return 0;
        }
        
        static double getCPUUtilization() {
            // Platform-specific implementation would go here
            return 0.0;
        }
        
        // Python-friendly methods
        static size_t getAvailable() { return getAvailableMemory(); }
        static size_t getProcessMemory() { return getProcessMemoryUsage(); }
        static double getCPUUsage() { return getCPUUtilization(); }
    };
    
    // Profiler for timing operations
    class Profiler {
    public:
        Profiler() = default;
        
        void start(const std::string& operation) {
            timers[operation] = std::make_unique<PerformanceTimer>();
            timers[operation]->reset();
        }
        
        double stop(const std::string& operation) {
            if (timers.find(operation) != timers.end()) {
                double elapsed = timers[operation]->elapsed();
                accumulators[operation].push_back(elapsed);
                timers.erase(operation);
                return elapsed;
            }
            return 0.0;
        }
        
        std::unordered_map<std::string, double> getAverageTimes() const {
            std::unordered_map<std::string, double> averages;
            for (const auto& pair : accumulators) {
                if (!pair.second.empty()) {
                    double sum = std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
                    averages[pair.first] = sum / pair.second.size();
                }
            }
            return averages;
        }
        
        void reset() {
            timers.clear();
            accumulators.clear();
        }
        
        size_t getSampleCount(const std::string& operation) const {
            auto it = accumulators.find(operation);
            return (it != accumulators.end()) ? it->second.size() : 0;
        }
        
        // Python-friendly methods
        void startTimer(const std::string& operation) { start(operation); }
        double stopTimer(const std::string& operation) { return stop(operation); }
        std::unordered_map<std::string, double> getAverages() const { return getAverageTimes(); }
        void resetProfiler() { reset(); }
        size_t getSamples(const std::string& operation) const { return getSampleCount(operation); }
        
    private:
        std::unordered_map<std::string, std::unique_ptr<PerformanceTimer>> timers;
        std::unordered_map<std::string, std::vector<double>> accumulators;
    };

    // Performance statistics collector
    class PerformanceStats {
    public:
        PerformanceStats() = default;
        
        void recordOperation(const std::string& operation, double elapsed) {
            operationTimes[operation].push_back(elapsed);
            totalOperations++;
            totalTime += elapsed;
        }
        
        std::unordered_map<std::string, std::vector<double>> getOperationTimes() const {
            return operationTimes;
        }
        
        double getAverageTime(const std::string& operation) const {
            auto it = operationTimes.find(operation);
            if (it != operationTimes.end() && !it->second.empty()) {
                double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
                return sum / it->second.size();
            }
            return 0.0;
        }
        
        size_t getOperationCount(const std::string& operation) const {
            auto it = operationTimes.find(operation);
            return (it != operationTimes.end()) ? it->second.size() : 0;
        }
        
        size_t getTotalOperations() const { return totalOperations; }
        double getTotalTime() const { return totalTime; }
        double getAverageOverall() const { return totalOperations ? totalTime / totalOperations : 0.0; }
        
        void reset() {
            operationTimes.clear();
            totalOperations = 0;
            totalTime = 0.0;
        }
        
        // Python-friendly methods
        void record(const std::string& operation, double time) { recordOperation(operation, time); }
        std::unordered_map<std::string, std::vector<double>> getTimes() const { return getOperationTimes(); }
        double getAverage(const std::string& operation) const { return getAverageTime(operation); }
        size_t getCount(const std::string& operation) const { return getOperationCount(operation); }
        size_t getTotalOps() const { return getTotalOperations(); }
        double getTotalTime() const { return getTotalTime(); }
        double getOverallAverage() const { return getAverageOverall(); }
        void resetStats() { reset(); }
        
    private:
        std::unordered_map<std::string, std::vector<double>> operationTimes;
        size_t totalOperations = 0;
        double totalTime = 0.0;
    };

    // Error recovery mechanism
    class ErrorRecovery {
    public:
        ErrorRecovery() = default;
        
        bool recordError(const std::string& operation, const std::string& errorMessage, 
                        double severity = 1.0) {
            errors.push_back({operation, errorMessage, severity, std::chrono::high_resolution_clock::now()});
            errorCount++;
            return true;
        }
        
        size_t getErrorCount() const { return errorCount; }
        double getErrorRate(const std::string& operation) const {
            auto it = operationErrors.find(operation);
            return (it != operationErrors.end()) ? (double)it->second / errorCount : 0.0;
        }
        
        std::vector<std::string> getRecentErrors(size_t limit = 10) const {
            if (errors.size() <= limit) {
                std::vector<std::string> all;
                for (const auto& e : errors) {
                    all.push_back(e.operation + ": " + e.message);
                }
                return all;
            }
            auto start = errors.end() - limit;
            std::vector<std::string> recent;
            for (auto it = start; it != errors.end(); ++it) {
                recent.push_back(it->operation + ": " + it->message);
            }
            return recent;
        }
        
        void clear() {
            errors.clear();
            errorCount = 0;
            operationErrors.clear();
        }
        
        // Python-friendly methods
        bool record(const std::string& operation, const std::string& message, double severity = 1.0) {
            return recordError(operation, message, severity);
        }
        size_t getErrorCount() const { return errorCount; }
        double getRate(const std::string& operation) const { return getErrorRate(operation); }
        std::vector<std::string> getRecent(size_t limit = 10) const { return getRecentErrors(limit); }
        void clearErrors() { clear(); }
        
    private:
        struct ErrorRecord {
            std::string operation;
            std::string message;
            double severity;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        std::vector<ErrorRecord> errors;
        std::unordered_map<std::string, size_t> operationErrors;
        size_t errorCount = 0;
    };

    // Configuration monitoring
    class ConfigMonitor {
    public:
        ConfigMonitor() = default;
        
        void recordConfigChange(const std::string& key, const std::string& oldValue, 
                               const std::string& newValue) {
            configChanges.push_back({key, oldValue, newValue, std::chrono::high_resolution_clock::now()});
        }
        
        size_t getConfigChangeCount() const { return configChanges.size(); }
        std::vector<std::string> getConfigChanges(size_t limit = 100) const {
            if (configChanges.size() <= limit) {
                std::vector<std::string> all;
                for (const auto& c : configChanges) {
                    all.push_back(c.key + ": " + c.oldValue + " -> " + c.newValue);
                }
                return all;
            }
            auto start = configChanges.end() - limit;
            std::vector<std::string> recent;
            for (auto it = start; it != configChanges.end(); ++it) {
                recent.push_back(it->key + ": " + it->oldValue + " -> " + it->newValue);
            }
            return recent;
        }
        
        // Python-friendly methods
        void record(const std::string& key, const std::string& oldValue, const std::string& newValue) {
            recordConfigChange(key, oldValue, newValue);
        }
        size_t getChangeCount() const { return getConfigChangeCount(); }
        std::vector<std::string> getChanges(size_t limit = 100) const { return getConfigChanges(limit); }
        
    private:
        struct ConfigChange {
            std::string key;
            std::string oldValue;
            std::string newValue;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        std::vector<ConfigChange> configChanges;
    };

    // Visualization utilities
    class VisualizationTools {
    public:
        static std::string createPerformanceChart(const PerformanceStats& stats);
        static std::string createMemoryUsageChart(const Brain& brain);
        static std::string createNetworkActivityGraph(const Brain& brain);
        static std::string createErrorTimeline(const ErrorRecovery& errors);
        static std::string createConfigHistory(const ConfigMonitor& configMonitor);
        
        // Python-friendly methods
        static std::string createPerformanceChart(const PerformanceStats& stats) {
            return createPerformanceChart(stats);
        }
        static std::string createMemoryUsageChart(const Brain& brain) {
            return createMemoryUsageChart(brain);
        }
        static std::string createNetworkActivityGraph(const Brain& brain) {
            return createNetworkActivityGraph(brain);
        }
        static std::string createErrorTimeline(const ErrorRecovery& errors) {
            return createErrorTimeline(errors);
        }
        static std::string createConfigHistory(const ConfigMonitor& configMonitor) {
            return createConfigHistory(configMonitor);
        }
    };
}

namespace debug {
    // Step-by-step execution tracer
    class ExecutionTracer {
    public:
        ExecutionTracer() = default;
        
        void start(const std::string& operation) {
            traceLog.push_back({"START", operation, std::chrono::high_resolution_clock::now()});
        }
        
        void stop(const std::string& operation) {
            traceLog.push_back({"STOP", operation, std::chrono::high_resolution_clock::now()});
        }
        
        std::vector<std::tuple<std::string, std::string, std::chrono::high_resolution_clock::time_point>> 
        getTrace() const { return traceLog; }
        
        void clear() { traceLog.clear(); }
        
        // Python-friendly methods
        void startStep(const std::string& operation) { start(operation); }
        void endStep(const std::string& operation) { stop(operation); }
        std::vector<std::vector<std::string>> getTrace() const {
            std::vector<std::vector<std::string>> result;
            for (const auto& entry : traceLog) {
                result.push_back({entry.action, entry.operation});
            }
            return result;
        }
        void clearTrace() { clear(); }
        
    private:
        struct TraceEntry {
            std::string action;
            std::string operation;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        std::vector<TraceEntry> traceLog;
    };
    
    // Memory inspection utilities
    class MemoryInspector {
    public:
        static std::vector<uint64_t> getNeuronMemoryUsage(const Brain& brain);
        static std::vector<uint64_t> getSynapseMemoryUsage(const Brain& brain);
        static std::vector<float> getMemoryHeatMap(const Brain& brain);
        
        // Python-friendly methods
        static std::vector<uint64_t> getNeuronMemoryUsage(const Brain& brain) {
            return getNeuronMemoryUsage(brain);
        }
        static std::vector<uint64_t> getSynapseMemoryUsage(const Brain& brain) {
            return getSynapseMemoryUsage(brain);
        }
        static std::vector<float> getMemoryHeatMap(const Brain& brain) {
            return getMemoryHeatMap(brain);
        }
    };
    
    // Debug logger
    class DebugLogger {
    public:
        DebugLogger() = default;
        
        void log(const std::string& message, const std::string& level = "INFO", 
                const std::string& component = "") {
            debugMessages.push_back({message, level, component, std::chrono::high_resolution_clock::now()});
        }
        
        std::vector<std::string> getMessages(size_t limit = 100) const {
            if (debugMessages.size() <= limit) {
                std::vector<std::string> all;
                for (const auto& m : debugMessages) {
                    all.push_back("[" + m.level + "] " + m.component + ": " + m.message);
                }
                return all;
            }
            auto start = debugMessages.end() - limit;
            std::vector<std::string> recent;
            for (auto it = start; it != debugMessages.end(); ++it) {
                recent.push_back("[" + it->level + "] " + it->component + ": " + it->message);
            }
            return recent;
        }
        
        void clear() { debugMessages.clear(); }
        
        // Python-friendly methods
        void logMessage(const std::string& message, const std::string& level = "INFO", 
                       const std::string& component = "") { log(message, level, component); }
        std::vector<std::string> getMessages(size_t limit = 100) const { return getMessages(limit); }
        void clearMessages() { clear(); }
        
    private:
        struct DebugMessage {
            std::string message;
            std::string level;
            std::string component;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        std::vector<DebugMessage> debugMessages;
    };
    
    // Step-by-step execution controller
    class StepController {
    public:
        StepController() : currentStep(0), paused(false), stepMode(false) {}
        
        void step(const std::string& operation) {
            if (stepMode) {
                currentStep++;
                stepLog.push_back({"STEP", operation, currentStep, std::chrono::high_resolution_clock::now()});
            }
        }
        
        void pause() { paused = true; }
        void resume() { paused = false; }
        bool isPaused() const { return paused; }
        
        void enableStepMode() { stepMode = true; }
        void disableStepMode() { stepMode = false; }
        bool isStepMode() const { return stepMode; }
        
        std::vector<std::string> getStepLog(size_t limit = 50) const {
            if (stepLog.size() <= limit) {
                std::vector<std::string> all;
                for (const auto& s : stepLog) {
                    all.push_back(s.operation + " (step " + std::to_string(s.stepNumber) + ")");
                }
                return all;
            }
            auto start = stepLog.end() - limit;
            std::vector<std::string> recent;
            for (auto it = start; it != stepLog.end(); ++it) {
                recent.push_back(it->operation + " (step " + std::to_string(it->stepNumber) + ")");
            }
            return recent;
        }
        
        void clear() { stepLog.clear(); }
        
        // Python-friendly methods
        void executeStep(const std::string& operation) { step(operation); }
        void pauseExecution() { pause(); }
        void resumeExecution() { resume(); }
        bool isPaused() const { return isPaused(); }
        void enableStepMode() { enableStepMode(); }
        void disableStepMode() { disableStepMode(); }
        bool isStepMode() const { return isStepMode(); }
        std::vector<std::string> getStepLog(size_t limit = 50) const { return getStepLog(limit); }
        void clearStepLog() { clear(); }
        
    private:
        struct StepLogEntry {
            std::string action;
            std::string operation;
            size_t stepNumber;
            std::chrono::high_resolution_clock::time_point timestamp;
        };
        
        std::vector<StepLogEntry> stepLog;
        size_t currentStep;
        bool paused;
        bool stepMode;
    };
}
