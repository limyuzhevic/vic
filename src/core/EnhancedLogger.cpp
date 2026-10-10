#include "EnhancedLogger.hpp"

namespace nlm {

EnhancedNLMLogger::EnhancedNLMLogger() {
}

EnhancedNLMLogger::~EnhancedNLMLogger() {
    // Generate final profiling report
    std::string report = generateProfilingReport();
    // Could save report to file or display it
}

void EnhancedNLMLogger::logStructured(const std::string& category, const std::string& event,
                                    const std::unordered_map<std::string, std::string>& data,
                                    LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Create structured log entry
    std::ostringstream oss;
    oss << "[" << category << "] " << event << ": ";
    bool first = true;
    for (const auto& pair : data) {
        if (!first) oss << ", ";
        oss << pair.first << "=" << pair.second;
        first = false;
    }
    
    // Use existing logger for output
    switch (level) {
        case LogLevel::Debug:
            NLM_LOG_DEBUG(oss.str());
            break;
        case LogLevel::Info:
            NLM_LOG_INFO(oss.str());
            break;
        case LogLevel::Warning:
            NLM_LOG_WARNING(oss.str());
            break;
        case LogLevel::Error:
            NLM_LOG_ERROR(oss.str());
            break;
        case LogLevel::Critical:
            NLM_LOG_CRITICAL(oss.str());
            break;
    }
    
    // Also record in structured logger
    structuredLogger_.logStructured(category, event, data, level);
}

void EnhancedNLMLogger::logPerformance(const std::string& operation,
                                      const std::unordered_map<std::string, double>& metrics,
                                      const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Log performance metrics
    performanceMonitor_.logPerformanceEvent("Performance", 0.0, operation);
    structuredLogger_.logPerformance(operation, metrics, context);
    
    std::ostringstream oss;
    oss << "Performance: " << operation << " ";
    for (const auto& pair : metrics) {
        oss << pair.first << "=" << pair.second << " ";
    }
    oss << "[" << context << "]";
    
    NLM_LOG_INFO(oss.str());
}

void EnhancedNLMLogger::logNeuralEvent(const std::string& event, const std::string& details,
                                      const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Record in neural profiler if active
    if (neuralProfiler_) {
        neuralProfiler_->recordStepTiming(0.0, 0, 0); // Simplified
    }
    
    // Log neural event
    std::ostringstream oss;
    oss << "Neural: " << event << " " << details;
    if (!context.empty()) oss << " [" << context << "]";
    
    NLM_LOG_INFO(oss.str());
    structuredLogger_.logStructured("Neural", event, {
        {"details", details},
        {"context", context}
    });
}

void EnhancedNLMLogger::logSystemResource(const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Record in system profiler if active
    if (systemProfiler_) {
        systemProfiler_->startProfiling(context);
        // Simulate recording current resource usage
        systemProfiler_->recordResourceUsage(50.0, 1024 * 1024, 1024, 512 * 1024, context);
        systemProfiler_->stopProfiling();
    }
    
    // Log system resource usage
    std::ostringstream oss;
    oss << "System: Resource usage [" << context << "]";
    
    NLM_LOG_INFO(oss.str());
}

void EnhancedNLMLogger::logMemoryAllocation(void* pointer, size_t size, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Record in memory leak detector
    if (memoryLeakDetector_) {
        memoryLeakDetector_->trackAllocation(pointer, size, context);
    }
    
    // Record in memory pool profiler
    memoryPoolProfiler_.trackAllocation("default", size, context);
    performanceMonitor_.recordMemoryAllocation(size, context);
    
    // Log memory allocation
    std::ostringstream oss;
    oss << "Memory: Allocated " << size << " bytes [" << context << "]";
    
    NLM_LOG_INFO(oss.str());
}

void EnhancedNLMLogger::logMemoryDeallocation(void* pointer, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Record in memory leak detector
    if (memoryLeakDetector_) {
        memoryLeakDetector_->trackDeallocation(pointer, context);
    }
    
    // Record in memory pool profiler
    memoryPoolProfiler_.trackDeallocation("default", 0, context);
    performanceMonitor_.recordMemoryDeallocation(0, context);
    
    // Log memory deallocation
    std::ostringstream oss;
    oss << "Memory: Freed memory [" << context << "]";
    
    NLM_LOG_INFO(oss.str());
}

void EnhancedNLMLogger::startNeuralSimulationProfiling(const std::string& simulationName) {
    std::lock_guard<std::mutex> lock(mutex_);
    neuralProfiler_ = std::make_unique<NeuralSimulationProfiler>(simulationName);
    neuralProfiler_->startProfiling();
}

void EnhancedNLMLogger::stopNeuralSimulationProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (neuralProfiler_) {
        neuralProfiler_->stopProfiling();
    }
}

void EnhancedNLMLogger::startSystemResourceProfiling(const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    systemProfiler_ = std::make_unique<SystemResourceProfiler>();
    systemProfiler_->startProfiling(context);
}

void EnhancedNLMLogger::stopSystemResourceProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (systemProfiler_) {
        systemProfiler_->stopProfiling();
    }
}

std::string EnhancedNLMLogger::generateProfilingReport() const {
    std::ostringstream oss;
    oss << "=== ENHANCED NLM LOGGING REPORT ===\n\n";
    
    oss << "Structured Logging:\n";
    oss << structuredLogger_.exportStructuredLogs() << "\n\n";
    
    oss << "Performance Metrics:\n";
    oss << performanceMonitor_.exportPerformanceAnalysis() << "\n\n";
    
    oss << "Memory Pool Analysis:\n";
    oss << memoryPoolProfiler_.exportMemoryPoolAnalysis() << "\n\n";
    
    oss << "Thread Profiling:\n";
    oss << threadProfiler_.exportThreadProfilingData() << "\n\n";
    
    return oss.str();
}

std::string EnhancedNLMLogger::exportAsText(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatTextReport(title, generateProfilingReport());
}

std::string EnhancedNLMLogger::exportAsHTML(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatHTMLDashboard(title, generateProfilingReport());
}

std::string EnhancedNLMLogger::exportAsCSV(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatCSVReport(title, generateProfilingReport());
}

std::string EnhancedNLMLogger::exportAsJSON(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatJSONReport(title, generateProfilingReport());
}

namespace {
    EnhancedNLMLogger globalEnhancedLogger;
}

EnhancedNLMLogger& getGlobalEnhancedLogger() {
    return globalEnhancedLogger;
}

void initializeProfiling(const std::string& outputDirectory) {
    NLMProfiling::getInstance().setupProfiling(outputDirectory);
}

std::string generateProfilingReport() {
    return NLMProfiling::getInstance().generateComprehensiveReport();
}

std::string exportProfilingAsText(const std::string& title) {
    return NLMProfiling::getInstance().exportAsText(title);
}

std::string exportProfilingAsHTML(const std::string& title) {
    return NLMProfiling::getInstance().exportAsHTML(title);
}

std::string exportProfilingAsCSV(const std::string& title) {
    return NLMProfiling::getInstance().exportAsCSV(title);
}

std::string exportProfilingAsJSON(const std::string& title) {
    return NLMProfiling::getInstance().exportAsJSON(title);
}

} // namespace nlm