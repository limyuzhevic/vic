#include "DebugLogger.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

struct DebugLogger::Impl {
    DebugLogConfig config;
    std::mutex mutex;
    
    // Debug information log
    std::vector<DebugInfo> debugLog;
    
    // Breakpoints and watchpoints
    std::map<std::string, std::function<bool(const std::string&, const std::string&)>> breakpoints;
    std::map<std::string, Watchpoint> watchpoints;
    
    // Performance tracking
    std::unordered_map<std::string, size_t> functionCallCounts;
    std::unordered_map<std::string, std::vector<double>> functionTimes;
    
    // Thread tracking
    std::map<std::thread::id, std::string> threadStates;
    std::map<std::thread::id, std::chrono::system_clock::time_point> threadStartTimes;
};

DebugLogger::DebugLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize default config
    pImpl->config = DebugLogConfig();
}

DebugLogger::~DebugLogger() = default;

void DebugLogger::configure(const DebugLogConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
}

DebugLogConfig DebugLogger::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config;
}

void DebugLogger::logStackTrace(const std::string& message,
                               const std::string& file, int line,
                               const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableStackTraceLogging) return;
    
    DebugInfo info;
    info.timestamp = std::chrono::system_clock::now();
    info.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    info.file = file;
    info.line = line;
    info.function = function;
    info.message = message;
    
    try {
        info.stackTrace = captureStackTrace();
    } catch (...) {
        info.stackTrace = {"Failed to capture stack trace"};
    }
    
    pImpl->debugLog.push_back(info);
    
    // Log using enhanced logger
    EnhancedLogger::getGlobal().log(LogLevel::Trace, "Stack trace: " + message, file, line, function);
}

void DebugLogger::logCurrentStackTrace(const std::string& context) {
    logStackTrace("Current stack trace", "", 0, context);
}

void DebugLogger::logThreadStackTrace(const std::thread::id& threadId,
                                     const std::string& context) {
    // In a real implementation, we would get the stack trace of another thread
    // For now, this is a placeholder
    EnhancedLogger::getGlobal().log(LogLevel::Info, "Thread stack trace requested", "", 0, "",
                                   {{ "thread_id", std::to_string(std::hash<std::thread::id>{}(threadId)) },
                                    { "context", context }});
}

void DebugLogger::logBreakpoint(const std::string& location,
                               const std::string& condition,
                               const std::string& message,
                               const std::string& file, int line,
                               const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableBreakpointLogging) return;
    
    DebugInfo info;
    info.timestamp = std::chrono::system_clock::now();
    info.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    info.file = file;
    info.line = line;
    info.function = function;
    info.message = "Breakpoint: " + location + ": " + message;
    if (!condition.empty()) {
        info.message += " (Condition: " + condition + ")";
    }
    
    pImpl->debugLog.push_back(info);
    
    EnhancedLogger::getGlobal().log(LogLevel::Breakpoint, info.message, file, line, function,
                                   {{ "location", location },
                                    { "condition", condition },
                                    { "message", message }});
}

void DebugLogger::logConditionalBreakpoint(const std::string& location,
                                          const std::string& condition,
                                          const std::string& message,
                                          const std::unordered_map<std::string, std::string>& variables,
                                          const std::string& file, int line,
                                          const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableBreakpointLogging) return;
    
    // Evaluate condition (simplified implementation)
    bool conditionMet = false;
    try {
        // In a real implementation, we would parse and evaluate the condition
        // For now, we'll just check if it's not empty
        conditionMet = !condition.empty();
    } catch (...) {
        conditionMet = false;
    }
    
    if (conditionMet) {
        logBreakpoint(location, condition, message, file, line, function);
        
        // Add variables to the log entry
        for (const auto& pair : variables) {
            EnhancedLogger::getGlobal().log(LogLevel::Breakpoint, 
                                          "Variable: " + pair.first + " = " + pair.second,
                                          file, line, function,
                                          {{ "variable_name", pair.first },
                                           { "variable_value", pair.second }});
        }
    }
}

void DebugLogger::logWatchpoint(const Watchpoint& watchpoint,
                               const std::string& oldValue,
                               const std::string& newValue,
                               const std::string& file, int line,
                               const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableWatchpointLogging) return;
    
    DebugInfo info;
    info.timestamp = std::chrono::system_clock::now();
    info.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    info.file = file;
    info.line = line;
    info.function = function;
    info.message = "Watchpoint triggered: " + watchpoint.expression;
    info.variables["old_value"] = oldValue;
    info.variables["new_value"] = newValue;
    
    pImpl->debugLog.push_back(info);
    
    EnhancedLogger::getGlobal().log(LogLevel::Breakpoint, info.message, file, line, function,
                                   {{ "expression", watchpoint.expression },
                                    { "old_value", oldValue },
                                    { "new_value", newValue },
                                    { "context", watchpoint.context }});
}

void DebugLogger::logMemoryAllocation(const void* ptr, size_t size,
                                     const std::string& file, int line,
                                     const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableMemoryDebug) return;
    
    EnhancedLogger::getGlobal().logMemoryAlloc(ptr, size, file + ":" + std::to_string(line));
}

void DebugLogger::logMemoryDeallocation(const void* ptr, size_t size,
                                       const std::string& file, int line,
                                       const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableMemoryDebug) return;
    
    EnhancedLogger::getGlobal().logMemoryDealloc(ptr, size, file + ":" + std::to_string(line));
}

void DebugLogger::logMemoryCorruption(const void* ptr, size_t expectedSize,
                                     const std::string& file, int line,
                                     const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableMemoryDebug) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Critical, "Memory corruption detected", file, line, function,
                                   {{ "pointer", std::to_string(reinterpret_cast<uintptr_t>(ptr)) },
                                    { "expected_size", std::to_string(expectedSize) }});
}

void DebugLogger::logMemoryLeak(const std::string& leakType,
                               size_t leakedBytes,
                               const std::vector<std::string>& stackTrace,
                               const std::string& file, int line,
                               const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableMemoryDebug) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Critical, "Memory leak detected", file, line, function,
                                   {{ "leak_type", leakType },
                                    { "leaked_bytes", std::to_string(leakedBytes) },
                                    { "stack_trace", "[" + join(stackTrace, ", ") + "]" }});
}

void DebugLogger::logFunctionEntry(const std::string& functionName,
                                  const std::unordered_map<std::string, std::string>& parameters,
                                  const std::string& file, int line,
                                  const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enablePerformanceDebug) return;
    
    pImpl->functionCallCounts[functionName]++;
    
    EnhancedLogger::getGlobal().log(LogLevel::Profile, "Function entry: " + functionName, file, line, function,
                                   {{ "function_name", functionName }});
    
    // Log parameters
    for (const auto& pair : parameters) {
        EnhancedLogger::getGlobal().log(LogLevel::Profile, "  Parameter: " + pair.first + " = " + pair.second,
                                      file, line, function,
                                      {{ "parameter_name", pair.first },
                                       { "parameter_value", pair.second }});
    }
}

void DebugLogger::logFunctionExit(const std::string& functionName,
                                 double executionTimeMs,
                                 const std::string& returnValue,
                                 const std::string& file, int line,
                                 const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enablePerformanceDebug) return;
    
    pImpl->functionTimes[functionName].push_back(executionTimeMs);
    
    EnhancedLogger::getGlobal().log(LogLevel::Profile, "Function exit: " + functionName, file, line, function,
                                   {{ "function_name", functionName },
                                    { "execution_time_ms", std::to_string(executionTimeMs) },
                                    { "return_value", returnValue }});
}

void DebugLogger::logFunctionException(const std::string& functionName,
                                      const std::string& exceptionType,
                                      const std::string& exceptionMessage,
                                      const std::vector<std::string>& stackTrace,
                                      const std::string& file, int line,
                                      const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enablePerformanceDebug) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Error, "Function exception: " + functionName, file, line, function,
                                   {{ "function_name", functionName },
                                    { "exception_type", exceptionType },
                                    { "exception_message", exceptionMessage },
                                    { "stack_trace", "[" + join(stackTrace, ", ") + "]" }});
}

void DebugLogger::logDeadlockDetection(const std::string& deadlockType,
                                      const std::vector<std::string>& threads,
                                      const std::string& file, int line,
                                      const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enablePerformanceDebug) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Critical, "Deadlock detected", file, line, function,
                                   {{ "deadlock_type", deadlockType },
                                    { "threads", "[" + join(threads, ", ") + "]" }});
}

void DebugLogger::logThreadCreation(const std::string& threadName,
                                   const std::thread::id& threadId,
                                   const std::string& file, int line,
                                   const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enablePerformanceDebug) return;
    
    pImpl->threadStates[threadId] = "running";
    pImpl->threadStartTimes[threadId] = std::chrono::system_clock::now();
    
    EnhancedLogger::getGlobal().log(LogLevel::Profile, "Thread created: " + threadName, file, line, function,
                                   {{ "thread_name", threadName },
                                    { "thread_id", std::to_string(std::hash<std::thread::id>{}(threadId)) }});
}

void DebugLogger::logThreadExit(const std::thread::id& threadId,
                               const std::string& reason,
                               const std::string& file, int line,
                               const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enablePerformanceDebug) return;
    
    auto it = pImpl->threadStates.find(threadId);
    if (it != pImpl->threadStates.end()) {
        EnhancedLogger::getGlobal().log(LogLevel::Profile, "Thread exited: " + reason, file, line, function,
                                       {{ "thread_id", std::to_string(std::hash<std::thread::id>{}(threadId)) },
                                        { "reason", reason }});
        
        pImpl->threadStates.erase(it);
    }
}

void DebugLogger::logThreadStateChange(const std::thread::id& threadId,
                                      const std::string& newState,
                                      const std::string& oldState,
                                      const std::string& file, int line,
                                      const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enablePerformanceDebug) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Profile, "Thread state change", file, line, function,
                                   {{ "thread_id", std::to_string(std::hash<std::thread::id>{}(threadId)) },
                                    { "old_state", oldState },
                                    { "new_state", newState }});
}

void DebugLogger::logFileOpen(const std::string& filepath,
                             std::ios_base::openmode mode,
                             bool success,
                             const std::string& file, int line,
                             const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableFileDebug) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Debug, "File operation: " + filepath, file, line, function,
                                   {{ "filepath", filepath },
                                    { "mode", std::to_string(static_cast<int>(mode)) },
                                    { "success", success ? "true" : "false" }});
}

void DebugLogger::logFileWrite(const std::string& filepath,
                              size_t bytesWritten,
                              bool success,
                              const std::string& file, int line,
                              const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableFileDebug) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Debug, "File write: " + filepath, file, line, function,
                                   {{ "filepath", filepath },
                                    { "bytes_written", std::to_string(bytesWritten) },
                                    { "success", success ? "true" : "false" }});
}

void DebugLogger::logFileRead(const std::string& filepath,
                             size_t bytesRead,
                             bool success,
                             const std::string& file, int line,
                             const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableFileDebug) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Debug, "File read: " + filepath, file, line, function,
                                   {{ "filepath", filepath },
                                    { "bytes_read", std::to_string(bytesRead) },
                                    { "success", success ? "true" : "false" }});
}

void DebugLogger::logSocketCreation(const std::string& endpoint,
                                   int port,
                                   bool success,
                                   const std::string& file, int line,
                                   const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogger::getGlobal().log(LogLevel::Debug, "Socket operation: " + endpoint, file, line, function,
                                   {{ "endpoint", endpoint },
                                    { "port", std::to_string(port) },
                                    { "success", success ? "true" : "false" }});
}

void DebugLogger::logNetworkPacket(const std::string& source,
                                  const std::string& destination,
                                  size_t packetSize,
                                  const std::string& protocol,
                                  const std::string& file, int line,
                                  const std::string& function) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogger::getGlobal().log(LogLevel::Debug, "Network packet: " + source, file, line, function,
                                   {{ "source", source },
                                    { "destination", destination },
                                    { "packet_size", std::to_string(packetSize) },
                                    { "protocol", protocol }});
}

void DebugLogger::setWatchpoint(const std::string& expression,
                              const std::function<bool(const std::string&, const std::string&)>& condition,
                              const std::string& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Watchpoint watchpoint;
    watchpoint.expression = expression;
    watchpoint.condition = condition;
    watchpoint.context = context;
    watchpoint.enabled = true;
    
    pImpl->watchpoints[expression] = watchpoint;
}

void DebugLogger::removeWatchpoint(const std::string& expression) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->watchpoints.erase(expression);
}

void DebugLogger::triggerWatchpoint(const std::string& expression,
                                  const std::string& variableName,
                                  const std::string& oldValue,
                                  const std::string& newValue) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->watchpoints.find(expression);
    if (it != pImpl->watchpoints.end()) {
        logWatchpoint(it->second, oldValue, newValue, "", 0, "");
    }
}

void DebugLogger::setBreakpoint(const std::string& location,
                               const std::string& condition) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->breakpoints[location] = [condition](const std::string& varName, const std::string& varValue) {
        // In a real implementation, we would evaluate the condition
        return !condition.empty();
    };
}

void DebugLogger::removeBreakpoint(const std::string& location) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->breakpoints.erase(location);
}

std::vector<DebugInfo> DebugLogger::getDebugLog() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->debugLog;
}

void DebugLogger::clearDebugLog() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->debugLog.clear();
}

void DebugLogger::exportToFile(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        return;
    }
    
    for (const auto& info : pImpl->debugLog) {
        outFile << "[" << std::chrono::system_clock::to_time_t(info.timestamp) << "] "
                << info.threadId << " " << info.function << "(): "
                << info.message << std::endl;
    }
    
    outFile.close();
}

void DebugLogger::exportToHTML(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        return;
    }
    
    outFile << "<!DOCTYPE html>\n";
    outFile << "<html><head><title>Debug Log</title></head><body>\n";
    outFile << "<h1>Debug Log</h1>\n";
    
    for (const auto& info : pImpl->debugLog) {
        outFile << "<div><strong>" << info.timestamp << "</strong> "
                << info.threadId << " " << info.function << "(): "
                << info.message << "</div>\n";
    }
    
    outFile << "</body></html>\n";
    outFile.close();
}

std::string DebugLogger::exportToString(const std::string& format) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (format == "text") {
        std::ostringstream oss;
        for (const auto& info : pImpl->debugLog) {
            oss << "[" << std::chrono::system_clock::to_time_t(info.timestamp) << "] "
                << info.threadId << " " << info.function << "(): "
                << info.message << std::endl;
        }
        return oss.str();
    } else if (format == "json") {
        std::ostringstream oss;
        oss << "[\n";
        for (size_t i = 0; i < pImpl->debugLog.size(); ++i) {
            const auto& info = pImpl->debugLog[i];
            if (i > 0) oss << ",\n";
            oss << "  {\"timestamp\": \"" << std::chrono::system_clock::to_time_t(info.timestamp) << "\",
           \"threadId\": \"" << info.threadId << "\",
           \"function\": \"" << info.function << "\",
           \"message\": \"" << info.message << "\"\n}";
        }
        oss << "\n]";
        return oss.str();
    }
    
    return "Unsupported format: " + format;
}

std::vector<std::string> DebugLogger::captureStackTrace() {
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

std::string DebugLogger::join(const std::vector<std::string>& vec, const std::string& delimiter) {
    if (vec.empty()) return "";
    std::ostringstream oss;
    oss << vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        oss << delimiter << vec[i];
    }
    return oss.str();
}

DebugLogger& DebugLogger::getGlobal() {
    static DebugLogger globalLogger;
    return globalLogger;
}

void DebugLogger::setGlobal(std::shared_ptr<DebugLogger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

} // namespace nlm
