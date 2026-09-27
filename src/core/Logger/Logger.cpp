#include "Logger.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <cmath>
#include <ctime>
#include <cstdint>
#include <algorithm>
#include <map>

namespace nlm {

// ConsoleSink implementation
std::string ConsoleSink::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        case LogLevel::NeuralEvent: return "NEURAL";
        case LogLevel::Spike: return "SPIKE";
        case LogLevel::SynapseChange: return "SYNAPSE";
        case LogLevel::Plasticity: return "PLASTICITY";
        case LogLevel::MemoryUpdate: return "MEMORY";
        case LogLevel::Development: return "DEVELOPMENT";
        case LogLevel::PerformanceStart: return "PERF_START";
        case LogLevel::PerformanceEnd: return "PERF_END";
        case LogLevel::Throughput: return "THROUGHPUT";
        case LogLevel::ResourceUsage: return "RESOURCE";
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Profile: return "PROFILE";
        case LogLevel::MemoryAlloc: return "MEM_ALLOC";
        case LogLevel::MemoryDealloc: return "MEM_DEALLOC";
        case LogLevel::Breakpoint: return "BREAKPOINT";
        default: return "UNKNOWN";
    }
}

std::string ConsoleSink::levelToColor(LogLevel level) const {
    if (!useColors_) return "";
    
    switch (level) {
        case LogLevel::Debug: return "\033[36m";    // Cyan
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[35m"; // Magenta
        case LogLevel::NeuralEvent: return "\033[34m";  // Blue
        case LogLevel::Spike: return "\033[32m";     // Green
        case LogLevel::SynapseChange: return "\033[33m"; // Yellow
        case LogLevel::Plasticity: return "\033[35m"; // Magenta
        case LogLevel::MemoryUpdate: return "\033[31m"; // Red
        case LogLevel::Development: return "\033[34m";  // Blue
        case LogLevel::Trace: return "\033[90m";     // Bright Black
        case LogLevel::Profile: return "\033[36m";   // Cyan
        default: return "\033[0m";
    }
}

// FileSink implementation
void FileSink::writeStructuredLog(const LogEntry& entry) {
    std::string indent = "    ";
    file_ << indent << "{\"timestamp\": \"" 
          << std::chrono::system_clock::to_time_t(entry.timestamp) << "\",\"";
    file_ << "level\": \"" << levelToString(entry.level) << "\",\"";
    file_ << "message\": \"" << entry.message << "\",\"";
    file_ << "neuronId\": \"" << entry.neuronId << "\",\"";
    file_ << "regionId\": \"" << entry.regionId << "\",\"";
    file_ << "metadata\": ";
    
    // Write metadata as JSON
    file_ << "{";
    bool first = true;
    for (const auto& pair : entry.metadata) {
        if (!first) file_ << ",";
        file_ << "\"" << escapeJson(pair.first) << "\": \"" 
              << escapeJson(pair.second) << "\"";
        first = false;
    }
    file_ << "}}" << std::endl;
}

std::string FileSink::escapeJson(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    return result;
}

std::string FileSink::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRIT";
        default: return "UNKNOWN";
    }
}

// NeuralSink implementation
void NeuralSink::writeEntry(const LogEntry& entry) {
    // Write neural-specific format
    std::cout << "[NEURAL] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::cout << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%N");
    
    std::cout << " [" << std::this_thread::get_id() << "]";
    
    if (!entry.neuronId.empty()) {
        std::cout << " Neuron:" << entry.neuronId;
    }
    if (!entry.synapseId.empty()) {
        std::cout << " Synapse:" << entry.synapseId;
    }
    if (!entry.regionId.empty()) {
        std::cout << " Region:" << entry.regionId;
    }
    
    std::cout << " " << entry.message;
    
    if (entry.durationMs > 0) {
        std::cout << " (duration: " << entry.durationMs << "ms)";
    }
    
    std::cout << "\n";
}

// PerformanceSink implementation
void PerformanceSink::writeEntry(const LogEntry& entry) {
    std::cout << "[PERF] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::cout << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%N");
    
    std::cout << " [" << std::this_thread::get_id() << "]";
    
    // Add timing if present
    if (entry.durationMs > 0) {
        std::cout << " Duration: " << entry.durationMs << "ms";
    }
    
    std::cout << " " << entry.message << "\n";
}

// DebugSink implementation
void DebugSink::writeEntry(const LogEntry& entry) {
    std::cout << "[DEBUG] ";
    
    auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
    std::cout << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S.%N");
    
    std::cout << " [" << std::this_thread::get_id() << "]";
    
    if (!entry.function.empty()) {
        std::cout << " " << entry.function;
    }
    
    if (!entry.stackTrace.empty() && entry.stackTrace.size() > 0 && 
        entry.stackTrace[0] != "Failed to capture stack trace") {
        std::cout << "\n  Stack trace:";
        for (size_t i = 0; i < entry.stackTrace.size() && i < 10; ++i) {  // Limit to 10 frames
            std::cout << "\n    " << entry.stackTrace[i];
        }
    }
    
    std::cout << "\n  " << entry.message << "\n";
}

std::vector<std::string> DebugSink::captureStackTrace() {
    std::vector<std::string> trace;
    try {
        // Try to get stack trace using stacktrace library
        auto trace_obj = std::stacktrace::current();
        for (size_t i = 0; i < trace_obj.size(); ++i) {
            trace.push_back(trace_obj[i].description());
        }
    } catch (...) {
        trace = {"Failed to capture stack trace"};
    }
    return trace;
}

// EnhancedLogger implementation
struct EnhancedLogger::Impl {
    std::vector<std::shared_ptr<ILogSink>> sinks;
    std::mutex mutex;
    std::string name;
    
    // Performance tracking
    std::chrono::system_clock::time_point startTime;
    std::unordered_map<std::string, double> metrics;
    
    // Memory tracking
    std::unordered_map<void*, size_t> allocations;
    size_t totalAllocated = 0;
    
    // Pattern filters
    std::vector<LogPattern> filters;
    
    Impl() : startTime(std::chrono::system_clock::now()) {}
};

EnhancedLogger::EnhancedLogger() : pImpl(std::make_unique<Impl>()) {
    // Configure default sinks
    auto consoleSink = std::make_shared<ConsoleSink>();
    consoleSink->setLevel(LogLevel::Debug);
    addSink(consoleSink);
    
    auto fileSink = std::make_shared<FileSink>();
    fileSink->setLevel(LogLevel::Info);
    addSink(fileSink);
}

EnhancedLogger::~EnhancedLogger() = default;

void EnhancedLogger::configure(const LogConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& sink : pImpl->sinks) {
        sink->configure(config);
    }
}

LogConfig EnhancedLogger::getConfig() const {
    if (!pImpl->sinks.empty()) {
        return pImpl->sinks[0]->getLevel(); // Note: This is not ideal, but we need to get config from sinks
    }
    LogConfig config;
    return config;
}

void EnhancedLogger::addSink(std::shared_ptr<ILogSink> sink) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sinks.push_back(sink);
}

void EnhancedLogger::removeSink(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sinks.erase(std::remove_if(pImpl->sinks.begin(), pImpl->sinks.end(),
                                     [&name](const std::shared_ptr<ILogSink>& sink) {
                                         return sink->getLevel() < LogLevel::Debug; // This is a simplified version
                                     }), pImpl->sinks.end());
}

void EnhancedLogger::clearSinks() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->sinks.clear();
}

void EnhancedLogger::log(LogLevel level, const std::string& message,
                        const std::string& file, int line,
                        const std::string& function,
                        const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.metadata = metadata;
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logNeuralEvent(const std::string& message, const std::string& neuronId,
                                   const std::string& synapseId, const std::string& regionId,
                                   const std::string& populationId, double duration) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = LogLevel::NeuralEvent;
    entry.message = message;
    entry.neuronId = neuronId;
    entry.synapseId = synapseId;
    entry.regionId = regionId;
    entry.populationId = populationId;
    entry.durationMs = duration;
    entry.metadata["type"] = "neural_event";
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logSpike(const std::string& neuronId, float voltage,
                             const std::string& regionId, const std::string& populationId,
                             double duration) {
    std::string message = "Spike detected in neuron " + neuronId + " (V=" + std::to_string(voltage) + "V)";
    
    LogEntry entry;
    entry.level = LogLevel::Spike;
    entry.message = message;
    entry.neuronId = neuronId;
    entry.regionId = regionId;
    entry.populationId = populationId;
    entry.durationMs = duration;
    entry.metadata["type"] = "spike";
    entry.metadata["voltage"] = std::to_string(voltage);
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logSynapseChange(const std::string& synapseId, const std::string& preNeuronId,
                                     const std::string& postNeuronId, float weightChange,
                                     const std::string& plasticityType, double duration) {
    std::string message = "Synapse " + synapseId + " changed: " + preNeuronId + " -> " + postNeuronId
                        + " weight change: " + std::to_string(weightChange);
    
    LogEntry entry;
    entry.level = LogLevel::SynapseChange;
    entry.message = message;
    entry.synapseId = synapseId;
    entry.neuronId = preNeuronId;
    entry.metadata["post_neuron"] = postNeuronId;
    entry.metadata["weight_change"] = std::to_string(weightChange);
    entry.metadata["plasticity_type"] = plasticityType;
    entry.durationMs = duration;
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logPlasticity(const std::string& neuronId, const std::string& regionId,
                                  const std::string& plasticityType, float magnitude,
                                  double duration) {
    std::string message = "Plasticity update in neuron " + neuronId + ": " + plasticityType
                        + " magnitude: " + std::to_string(magnitude);
    
    LogEntry entry;
    entry.level = LogLevel::Plasticity;
    entry.message = message;
    entry.neuronId = neuronId;
    entry.regionId = regionId;
    entry.metadata["plasticity_type"] = plasticityType;
    entry.metadata["magnitude"] = std::to_string(magnitude);
    entry.durationMs = duration;
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logMemoryUpdate(const std::string& neuronId, size_t memoryDelta,
                                    const std::string& memoryType, double duration) {
    std::string message = "Memory update for neuron " + neuronId + ": " + std::to_string(memoryDelta) + " bytes";
    
    LogEntry entry;
    entry.level = LogLevel::MemoryUpdate;
    entry.message = message;
    entry.neuronId = neuronId;
    entry.metadata["memory_type"] = memoryType;
    entry.metadata["memory_delta"] = std::to_string(memoryDelta);
    entry.durationMs = duration;
    entry.memoryDelta = memoryDelta;
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logDevelopment(const std::string& neuronId, const std::string& regionId,
                                    const std::string& stage, double duration) {
    std::string message = "Development stage " + stage + " for neuron " + neuronId
                        + " in region " + regionId;
    
    LogEntry entry;
    entry.level = LogLevel::Development;
    entry.message = message;
    entry.neuronId = neuronId;
    entry.regionId = regionId;
    entry.metadata["stage"] = stage;
    entry.durationMs = duration;
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::startTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->metrics[operation] = std::chrono::duration<double, std::milli>(
        std::chrono::system_clock::now() - pImpl->startTime).count();
}

void EnhancedLogger::endTiming(const std::string& operation) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto endTime = std::chrono::system_clock::now();
    double duration = std::chrono::duration<double, std::milli>(endTime - pImpl->metrics[operation]).count();
    
    LogEntry entry;
    entry.level = LogLevel::PerformanceEnd;
    entry.message = "Operation completed: " + operation + " (" + std::to_string(duration) + "ms)";
    entry.metadata["operation"] = operation;
    entry.metadata["duration_ms"] = std::to_string(duration);
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logThroughput(const std::string& operation, size_t count, double timeMs) {
    double throughput = count > 0 ? count / (timeMs / 1000.0) : 0.0;
    
    LogEntry entry;
    entry.level = LogLevel::Throughput;
    entry.message = "Throughput: " + operation + " - " + std::to_string(count) 
                   + " operations in " + std::to_string(timeMs) + "ms ("
                   + std::to_string(throughput) + " ops/sec)";
    entry.metadata["operation"] = operation;
    entry.metadata["count"] = std::to_string(count);
    entry.metadata["time_ms"] = std::to_string(timeMs);
    entry.metadata["throughput"] = std::to_string(throughput);
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logResourceUsage(double cpuUsage, size_t memoryMB, size_t memoryAllocated) {
    LogEntry entry;
    entry.level = LogLevel::ResourceUsage;
    entry.message = "Resource usage - CPU: " + std::to_string(cpuUsage) + "%, "
                   + std::to_string(memoryMB) + "MB memory (" 
                   + std::to_string(memoryAllocated) + " bytes allocated)";
    entry.metadata["cpu_percent"] = std::to_string(cpuUsage);
    entry.metadata["memory_mb"] = std::to_string(memoryMB);
    entry.metadata["memory_bytes"] = std::to_string(memoryAllocated);
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logWithStackTrace(const std::string& message, const std::string& file,
                                      int line, const std::string& function) {
    LogEntry entry;
    entry.level = LogLevel::Trace;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    
    try {
        entry.stackTrace = DebugSink::captureStackTrace();
    } catch (...) {
        entry.stackTrace = {"Failed to capture stack trace"};
    }
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logBreakpoint(const std::string& location, const std::string& condition,
                                  const std::string& message) {
    LogEntry entry;
    entry.level = LogLevel::Breakpoint;
    entry.message = "Breakpoint at " + location + ": " + message;
    if (!condition.empty()) {
        entry.message += " (Condition: " + condition + ")";
    }
    entry.metadata["location"] = location;
    entry.metadata["condition"] = condition;
    entry.metadata["message"] = message;
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logProfile(const std::string& operation, double timeMs,
                               size_t memoryDelta, int callCount) {
    LogEntry entry;
    entry.level = LogLevel::Profile;
    entry.message = "Profile: " + operation + " - " + std::to_string(timeMs) + "ms";
    if (callCount > 1) {
        entry.message += " (avg: " + std::to_string(timeMs / callCount) + "ms per call)";
    }
    entry.metadata["operation"] = operation;
    entry.metadata["time_ms"] = std::to_string(timeMs);
    entry.metadata["memory_delta"] = std::to_string(memoryDelta);
    entry.metadata["call_count"] = std::to_string(callCount);
    entry.durationMs = timeMs;
    entry.memoryDelta = memoryDelta;
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logMemoryAlloc(const void* ptr, size_t size, const std::string& location) {
    LogEntry entry;
    entry.level = LogLevel::MemoryAlloc;
    entry.message = "Memory allocation: " + std::to_string(size) + " bytes at " 
                   + std::to_string(reinterpret_cast<uintptr_t>(ptr));
    if (!location.empty()) {
        entry.message += " (" + location + ")";
    }
    entry.metadata["pointer"] = std::to_string(reinterpret_cast<uintptr_t>(ptr));
    entry.metadata["size"] = std::to_string(size);
    entry.metadata["location"] = location;
    entry.memoryDelta = size;
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::logMemoryDealloc(const void* ptr, size_t size, const std::string& location) {
    LogEntry entry;
    entry.level = LogLevel::MemoryDealloc;
    entry.message = "Memory deallocation: " + std::to_string(size) + " bytes at " 
                   + std::to_string(reinterpret_cast<uintptr_t>(ptr));
    if (!location.empty()) {
        entry.message += " (" + location + ")";
    }
    entry.metadata["pointer"] = std::to_string(reinterpret_cast<uintptr_t>(ptr));
    entry.metadata["size"] = std::to_string(size);
    entry.metadata["location"] = location;
    entry.memoryDelta = -size;
    
    for (auto& sink : pImpl->sinks) {
        sink->log(entry);
    }
}

void EnhancedLogger::debug(const std::string& message, const std::string& file, int line,
                          const std::string& function, const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Debug, message, file, line, function, metadata);
}

void EnhancedLogger::info(const std::string& message, const std::string& file, int line,
                         const std::string& function, const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Info, message, file, line, function, metadata);
}

void EnhancedLogger::warning(const std::string& message, const std::string& file, int line,
                            const std::string& function, const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Warning, message, file, line, function, metadata);
}

void EnhancedLogger::error(const std::string& message, const std::string& file, int line,
                          const std::string& function, const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Error, message, file, line, function, metadata);
}

void EnhancedLogger::critical(const std::string& message, const std::string& file, int line,
                             const std::string& function, const std::unordered_map<std::string, std::string>& metadata) {
    log(LogLevel::Critical, message, file, line, function, metadata);
}

void EnhancedLogger::neuralEvent(const std::string& message, const std::string& neuronId,
                                const std::string& synapseId, const std::string& regionId,
                                const std::string& populationId, double duration) {
    logNeuralEvent(message, neuronId, synapseId, regionId, populationId, duration);
}

void EnhancedLogger::spike(const std::string& neuronId, float voltage,
                         const std::string& regionId, const std::string& populationId,
                         double duration) {
    logSpike(neuronId, voltage, regionId, populationId, duration);
}

void EnhancedLogger::synapseChange(const std::string& synapseId, const std::string& preNeuronId,
                                  const std::string& postNeuronId, float weightChange,
                                  const std::string& plasticityType, double duration) {
    logSynapseChange(synapseId, preNeuronId, postNeuronId, weightChange, plasticityType, duration);
}

void EnhancedLogger::plasticity(const std::string& neuronId, const std::string& regionId,
                               const std::string& plasticityType, float magnitude,
                               double duration) {
    logPlasticity(neuronId, regionId, plasticityType, magnitude, duration);
}

void EnhancedLogger::memoryUpdate(const std::string& neuronId, size_t memoryDelta,
                                 const std::string& memoryType, double duration) {
    logMemoryUpdate(neuronId, memoryDelta, memoryType, duration);
}

void EnhancedLogger::development(const std::string& neuronId, const std::string& regionId,
                                const std::string& stage, double duration) {
    logDevelopment(neuronId, regionId, stage, duration);
}

bool EnhancedLogger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (const auto& sink : pImpl->sinks) {
        if (sink->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

EnhancedLogger& EnhancedLogger::getGlobal() {
    static EnhancedLogger globalLogger;
    return globalLogger;
}

void EnhancedLogger::setGlobal(std::shared_ptr<EnhancedLogger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function,
                   const std::unordered_map<std::string, std::string>& metadata)
    : level_(level), file_(file), line_(line), function_(function), metadata_(metadata) {}

LogStream::~LogStream() {
    EnhancedLogger::getGlobal().log(level_, oss_.str(), file_, line_, function_, metadata_);
}

LogStream& LogStream::withMetadata(const std::string& key, const std::string& value) {
    metadata_[key] = value;
    return *this;
}

} // namespace nlm
