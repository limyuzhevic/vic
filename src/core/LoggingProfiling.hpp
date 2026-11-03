#ifndef NLM_LOGGING_PROFILING_HPP
#define NLM_LOGGING_PROFILING_HPP

#include <string>
#include <iostream>
#include <chrono>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace debug {

// Performance timing information
struct PerformanceMetrics {
    std::string operationName;
    std::chrono::duration<double> duration;
    uint64_t calls;
    double averageDuration() const { return duration.count() / calls; }
    double totalDuration() const { return duration.count(); }
};

// Memory usage information
struct MemoryInfo {
    size_t totalAllocated;
    size_t totalFreed;
    size_t currentUsage;
    size_t peakUsage;
    std::map<std::string, size_t> allocationsByType;
};

// Step-by-step execution trace
struct StepTrace {
    uint64_t stepNumber;
    double timestamp;
    std::string eventType;
    std::string description;
    std::map<std::string, std::string> parameters;
    std::chrono::duration<double> duration;
    size_t memoryUsage;
};

// Comprehensive logging and profiling system
class DebugLogger {
private:
    std::ofstream logFile;
    std::stringstream consoleBuffer;
    bool enableConsoleOutput;
    bool enableFileOutput;
    bool enablePerformanceProfiling;
    bool enableMemoryTracking;
    bool enableStepTracing;
    bool enableNeuronActivityLogging;
    
    std::vector<PerformanceMetrics> performanceHistory;
    std::vector<StepTrace> stepHistory;
    MemoryInfo memoryInfo;
    
    std::chrono::high_resolution_clock::time_point sessionStartTime;
    
public:
    DebugLogger() {
        enableConsoleOutput = false;
        enableFileOutput = false;
        enablePerformanceProfiling = false;
        enableMemoryTracking = false;
        enableStepTracing = false;
        enableNeuronActivityLogging = false;
        
        sessionStartTime = std::chrono::high_resolution_clock::now();
        memoryInfo.totalAllocated = 0;
        memoryInfo.totalFreed = 0;
        memoryInfo.currentUsage = 0;
        memoryInfo.peakUsage = 0;
    }
    
    void enableConsole(bool enable) { enableConsoleOutput = enable; }
    void enableFile(bool enable, const std::string& filepath) {
        enableFileOutput = enable;
        if (enable) {
            logFile.open(filepath, std::ios::app);
        }
    }
    
    void enablePerformanceProfiling(bool enable) { enablePerformanceProfiling = enable; }
    void enableMemoryTracking(bool enable) { enableMemoryTracking = enable; }
    void enableStepTracing(bool enable) { enableStepTracing = enable; }
    void enableNeuronActivityLogging(bool enable) { enableNeuronActivityLogging = enable; }
    
    void log(const std::string& message) {
        std::string timestamp = getTimestamp();
        std::string formatted = "[" + timestamp + "] " + message;
        
        if (enableConsoleOutput) {
            std::cout << formatted << std::endl;
        }
        
        if (enableFileOutput && logFile.is_open()) {
            logFile << formatted << std::endl;
        }
    }
    
    void logPerformance(const std::string& operation, double duration) {
        if (!enablePerformanceProfiling) return;
        
        // Find existing metric
        bool found = false;
        for (auto& metric : performanceHistory) {
            if (metric.operationName == operation) {
                metric.duration += std::chrono::duration<double>(duration);
                metric.calls++;
                found = true;
                break;
            }
        }
        
        if (!found) {
            PerformanceMetrics metric;
            metric.operationName = operation;
            metric.duration = std::chrono::duration<double>(duration);
            metric.calls = 1;
            performanceHistory.push_back(metric);
        }
        
        std::string message = "PERF: " + operation + " took " + std::to_string(duration * 1000) + "ms (avg: " + 
                             std::to_string(getMetricAverage(operation) * 1000) + "ms)";
        log(message);
    }
    
    void logStep(uint64_t step, const std::string& event, const std::string& description,
                 const std::map<std::string, std::string>& parameters = {}) {
        if (!enableStepTracing) return;
        
        StepTrace trace;
        trace.stepNumber = step;
        trace.timestamp = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - sessionStartTime).count();
        trace.eventType = event;
        trace.description = description;
        trace.parameters = parameters;
        trace.memoryUsage = memoryInfo.currentUsage;
        
        stepHistory.push_back(trace);
        
        std::stringstream ss;
        ss << "STEP " << step << ": " << event << " - " << description;
        if (!parameters.empty()) {
            ss << " (";
            bool first = true;
            for (const auto& param : parameters) {
                if (!first) ss << ", ";
                ss << param.first << "=" << param.second;
                first = false;
            }
            ss << ")";
        }
        
        log(ss.str());
    }
    
    void logNeuronActivity(uint64_t step, int neuronId, double firingRate, double membranePotential) {
        if (!enableNeuronActivityLogging) return;
        
        std::string message = "NEURON: step=" << step << ", id=" << neuronId 
                             << ", rate=" << firingRate << ", voltage=" << membranePotential;
        log(message);
    }
    
    void updateMemoryUsage(size_t allocated, size_t freed) {
        if (!enableMemoryTracking) return;
        
        memoryInfo.totalAllocated += allocated;
        memoryInfo.totalFreed += freed;
        memoryInfo.currentUsage = memoryInfo.totalAllocated - memoryInfo.totalFreed;
        
        if (memoryInfo.currentUsage > memoryInfo.peakUsage) {
            memoryInfo.peakUsage = memoryInfo.currentUsage;
        }
        
        std::string message = "MEM: allocated=" << allocated << ", freed=" << freed 
                             << ", current=" << memoryInfo.currentUsage 
                             << ", peak=" << memoryInfo.peakUsage;
        log(message);
    }
    
    double getMetricAverage(const std::string& operation) {
        double total = 0.0;
        uint64_t count = 0;
        
        for (const auto& metric : performanceHistory) {
            if (metric.operationName == operation) {
                total += metric.duration.count();
                count += metric.calls;
            }
        }
        
        return count > 0 ? total / count : 0.0;
    }
    
    void printSessionSummary() {
        double totalTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - sessionStartTime).count();
        
        log("\n=== SESSION SUMMARY ===");
        log("Total time: " + std::to_string(totalTime) + "s");
        log("Performance metrics: " + std::to_string(performanceHistory.size()));
        log("Step traces: " + std::to_string(stepHistory.size()));
        log("Peak memory usage: " + std::to_string(memoryInfo.peakUsage) + " bytes");
        
        // Print performance summary
        if (enablePerformanceProfiling) {
            log("\nPERFORMANCE SUMMARY:");
            for (const auto& metric : performanceHistory) {
                log("  " + metric.operationName + ": " + std::to_string(metric.calls) + " calls, " +
                    std::to_string(metric.averageDuration() * 1000) + "ms avg, " +
                    std::to_string(metric.totalDuration() * 1000) + "ms total");
            }
        }
        
        // Print memory summary
        if (enableMemoryTracking) {
            log("\nMEMORY SUMMARY:");
            log("  Total allocated: " + std::to_string(memoryInfo.totalAllocated) + " bytes");
            log("  Total freed: " + std::to_string(memoryInfo.totalFreed) + " bytes");
            log("  Current usage: " + std::to_string(memoryInfo.currentUsage) + " bytes");
            log("  Peak usage: " + std::to_string(memoryInfo.peakUsage) + " bytes");
        }
        
        if (enableFileOutput && logFile.is_open()) {
            logFile << "\n=== SESSION SUMMARY ===\n";
            logFile << "Total time: " << totalTime << "s\n";
            logFile << "Performance metrics: " << performanceHistory.size() << "\n";
            logFile << "Step traces: " << stepHistory.size() << "\n";
        }
    }
    
    void saveStepTrace(const std::string& filename) {
        if (!enableStepTracing) return;
        
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        file << "Step Trace Export" << std::endl;
        file << "====================" << std::endl;
        
        for (const auto& trace : stepHistory) {
            file << "Step " << trace.stepNumber << ": " << trace.timestamp << "s "
                 << trace.eventType << " - " << trace.description;
            
            if (!trace.parameters.empty()) {
                file << " (";
                bool first = true;
                for (const auto& param : trace.parameters) {
                    if (!first) file << ", ";
                    file << param.first << "=" << param.second;
                    first = false;
                }
                file << ")";
            }
            
            file << " | Duration: " << trace.duration.count() * 1000 << "ms "
                 << "| Memory: " << trace.memoryUsage << " bytes" << std::endl;
        }
        
        file.close();
    }
    
    std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void clear() {
        performanceHistory.clear();
        stepHistory.clear();
        sessionStartTime = std::chrono::high_resolution_clock::now();
    }
    
    static DebugLogger& getInstance() {
        static DebugLogger instance;
        return instance;
    }
};

// Global debug logger instance
extern DebugLogger globalDebugLogger;

// Convenience logging functions
#define NLM_LOG_DEBUG(message) nlm::debug::globalDebugLogger.log(message)
#define NLM_LOG_PERF(operation, duration) nlm::debug::globalDebugLogger.logPerformance(operation, duration)
#define NLM_LOG_STEP(step, event, description) nlm::debug::globalDebugLogger.logStep(step, event, description)
#define NLM_LOG_NEURON(step, id, rate, voltage) nlm::debug::globalDebugLogger.logNeuronActivity(step, id, rate, voltage)

// Performance measurement macro
#define NLM_PERF_MEASURE(operation) \
    auto start = std::chrono::high_resolution_clock::now(); \
    operation; \
    auto end = std::chrono::high_resolution_clock::now(); \
    nlm::debug::globalDebugLogger.logPerformance(#operation, \
        std::chrono::duration<double>(end - start).count())

// Step tracing macro
#define NLM_STEP_TRACE(step, event, description) \
    nlm::debug::globalDebugLogger.logStep(step, event, description)

} // namespace debug
} // namespace nlm

#endif // NLM_LOGGING_PROFILING_HPP