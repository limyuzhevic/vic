#pragma once

#include "core/Logger/Logger.hpp"
#include "profiling/ProfilingTimer.hpp"
#include "debug/LoggingEnhancement.hpp"
#include "profiling/NLMProfilingOutputFormatter.hpp"

namespace nlm {

// Enhanced logging interface that combines existing Logger with new profiling capabilities
class EnhancedNLMLogger {
public:
    EnhancedNLMLogger();
    ~EnhancedNLMLogger();
    
    // Log with structured data (JSON-like format)
    void logStructured(const std::string& category, const std::string& event,
                      const std::unordered_map<std::string, std::string>& data,
                      LogLevel level = LogLevel::Info);
    
    // Log performance metrics
    void logPerformance(const std::string& operation,
                       const std::unordered_map<std::string, double>& metrics,
                       const std::string& context = "");
    
    // Log neural simulation events
    void logNeuralEvent(const std::string& event, const std::string& details,
                       const std::string& context = "");
    
    // Log system resource usage
    void logSystemResource(const std::string& context = "");
    
    // Log memory allocation/deallocation
    void logMemoryAllocation(void* pointer, size_t size, const std::string& context);
    void logMemoryDeallocation(void* pointer, const std::string& context);
    
    // Get structured logger
    StructuredLogger& getStructuredLogger() { return structuredLogger_; }
    
    // Get debug visualizer
    DebugVisualizer& getDebugVisualizer() { return debugVisualizer_; }
    
    // Get advanced performance monitor
    AdvancedPerformanceMonitor& getPerformanceMonitor() { return performanceMonitor_; }
    
    // Get memory pool profiler
    MemoryPoolProfiler& getMemoryPoolProfiler() { return memoryPoolProfiler_; }
    
    // Get thread profiler
    ThreadProfiler& getThreadProfiler() { return threadProfiler_; }
    
    // Start neural simulation profiling
    void startNeuralSimulationProfiling(const std::string& simulationName);
    
    // Stop neural simulation profiling
    void stopNeuralSimulationProfiling();
    
    // Start system resource profiling
    void startSystemResourceProfiling(const std::string& context = "");
    
    // Stop system resource profiling
    void stopSystemResourceProfiling();
    
    // Generate comprehensive profiling report
    std::string generateProfilingReport() const;
    
    // Export profiling data in different formats
    std::string exportAsText(const std::string& title) const;
    std::string exportAsHTML(const std::string& title) const;
    std::string exportAsCSV(const std::string& title) const;
    std::string exportAsJSON(const std::string& title) const;
    
private:
    StructuredLogger structuredLogger_;
    DebugVisualizer debugVisualizer_;
    AdvancedPerformanceMonitor performanceMonitor_;
    MemoryPoolProfiler memoryPoolProfiler_;
    ThreadProfiler threadProfiler_;
    
    std::unique_ptr<NeuralSimulationProfiler> neuralProfiler_;
    std::unique_ptr<SystemResourceProfiler> systemProfiler_;
    std::unique_ptr<AdvancedMemoryLeakDetector> memoryLeakDetector_;
    
    mutable std::mutex mutex_;
};

// Convenience macros for enhanced logging
#define NLM_LOG_STRUCTURED(category, event, data) \
    nlm::EnhancedNLMLogger::getGlobal().logStructured(category, event, data)

#define NLM_LOG_PERFORMANCE(operation, metrics) \
    nlm::EnhancedNLMLogger::getGlobal().logPerformance(operation, metrics)

#define NLM_LOG_NEURAL(event, details) \
    nlm::EnhancedNLMLogger::getGlobal().logNeuralEvent(event, details)

#define NLM_LOG_SYSTEM_RESOURCE(context) \
    nlm::EnhancedNLMLogger::getGlobal().logSystemResource(context)

#define NLM_LOG_MEMORY_ALLOC(ptr, size, context) \
    nlm::EnhancedNLMLogger::getGlobal().logMemoryAllocation(ptr, size, context)

#define NLM_LOG_MEMORY_DEALLOC(ptr, context) \
    nlm::EnhancedNLMLogger::getGlobal().logMemoryDeallocation(ptr, context)

// Global enhanced logger instance
EnhancedNLMLogger& getGlobalEnhancedLogger();

// Initialize profiling with output directory
void initializeProfiling(const std::string& outputDirectory = "./profiles");

// Generate profiling report
std::string generateProfilingReport();

// Export profiling data in different formats
std::string exportProfilingAsText(const std::string& title);
std::string exportProfilingAsHTML(const std::string& title);
std::string exportProfilingAsCSV(const std::string& title);
std::string exportProfilingAsJSON(const std::string& title);

} // namespace nlm