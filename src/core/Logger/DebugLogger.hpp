#pragma once

#include "Logger.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <set>

namespace nlm {

// Debug logging configuration
struct DebugLogConfig {
    bool enableStackTraceLogging = true;
    bool enableBreakpointLogging = true;
    bool enableWatchpointLogging = true;
    bool enableMemoryDebug = true;
    bool enablePerformanceDebug = true;
    bool enableNetworkDebug = false;
    bool enableFileDebug = true;
    
    // Debug levels
    bool enableDebugLevel = true;
    bool enableTraceLevel = true;
    bool enableProfileLevel = true;
    
    // Breakpoint options
    bool enableConditionalBreakpoints = true;
    bool enableTemporaryBreakpoints = true;
    bool enableWatchExpressions = true;
    
    // Memory debugging
    bool trackAllocations = true;
    bool trackDeallocations = true;
    bool detectMemoryLeaks = true;
    bool trackMemoryPatterns = true;
    
    // Performance debugging
    bool trackFunctionCallCounts = true;
    bool trackExecutionTimes = true;
    bool detectDeadlocks = true;
    bool trackThreadStates = true;
    
    // Output format
    bool enableDetailedOutput = true;
    bool enableCompactOutput = false;
    bool enableStructuredOutput = true;
};

// Watchpoint for monitoring variable changes
struct Watchpoint {
    std::string expression;
    std::string variableName;
    std::string context;
    std::function<bool(const std::string&, const std::string&)> condition;
    bool enabled = true;
    std::chrono::system_clock::time_point lastTriggered;
    size_t triggerCount = 0;
};

// Debug information structure
struct DebugInfo {
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string file;
    int line;
    std::string function;
    std::string message;
    std::vector<std::string> stackTrace;
    std::unordered_map<std::string, std::string> variables;
    std::unordered_map<std::string, std::string> metadata;
};

// Debug logger with advanced debugging capabilities
class DebugLogger {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    DebugLogger();
    ~DebugLogger();
    
    // Disable copying
    DebugLogger(const DebugLogger&) = delete;
    DebugLogger& operator=(const DebugLogger&) = delete;
    
    // Configuration
    void configure(const DebugLogConfig& config);
    DebugLogConfig getConfig() const;
    
    // Stack trace logging
    void logStackTrace(const std::string& message = "",
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    void logCurrentStackTrace(const std::string& context = "");
    void logThreadStackTrace(const std::thread::id& threadId,
                           const std::string& context = "");
    
    // Breakpoint logging
    void logBreakpoint(const std::string& location,
                      const std::string& condition = "",
                      const std::string& message = "",
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    void logConditionalBreakpoint(const std::string& location,
                                 const std::string& condition,
                                 const std::string& message,
                                 const std::unordered_map<std::string, std::string>& variables,
                                 const std::string& file = "", int line = 0,
                                 const std::string& function = "");
    
    void logWatchpoint(const Watchpoint& watchpoint,
                      const std::string& oldValue,
                      const std::string& newValue,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    // Memory debugging
    void logMemoryAllocation(const void* ptr, size_t size,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "");
    
    void logMemoryDeallocation(const void* ptr, size_t size,
                              const std::string& file = "", int line = 0,
                              const std::string& function = "");
    
    void logMemoryCorruption(const void* ptr, size_t expectedSize,
                            const std::string& file = "", int line = 0,
                            const std::string& function = "");
    
    void logMemoryLeak(const std::string& leakType,
                     size_t leakedBytes,
                     const std::vector<std::string>& stackTrace,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "");
    
    // Performance debugging
    void logFunctionEntry(const std::string& functionName,
                         const std::unordered_map<std::string, std::string>& parameters,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    
    void logFunctionExit(const std::string& functionName,
                        double executionTimeMs,
                        const std::string& returnValue,
                        const std::string& file = "", int line = 0,
                        const std::string& function = "");
    
    void logFunctionException(const std::string& functionName,
                             const std::string& exceptionType,
                             const std::string& exceptionMessage,
                             const std::vector<std::string>& stackTrace,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "");
    
    void logDeadlockDetection(const std::string& deadlockType,
                             const std::vector<std::string>& threads,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "");
    
    // Thread debugging
    void logThreadCreation(const std::string& threadName,
                          const std::thread::id& threadId,
                          const std::string& file = "", int line = 0,
                          const std::string& function = "");
    
    void logThreadExit(const std::thread::id& threadId,
                      const std::string& reason,
                      const std::string& file = "", int line = 0,
                      const std::string& function = "");
    
    void logThreadStateChange(const std::thread::id& threadId,
                             const std::string& newState,
                             const std::string& oldState,
                             const std::string& file = "", int line = 0,
                             const std::string& function = "");
    
    // File system debugging
    void logFileOpen(const std::string& filepath,
                    std::ios_base::openmode mode,
                    bool success,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "");
    
    void logFileWrite(const std::string& filepath,
                     size_t bytesWritten,
                     bool success,
                     const std::string& file = "", int line = 0,
                     const std::string& function = "");
    
    void logFileRead(const std::string& filepath,
                    size_t bytesRead,
                    bool success,
                    const std::string& file = "", int line = 0,
                    const std::string& function = "");
    
    // Network debugging
    void logSocketCreation(const std::string& endpoint,
                         int port,
                         bool success,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    
    void logNetworkPacket(const std::string& source,
                         const std::string& destination,
                         size_t packetSize,
                         const std::string& protocol,
                         const std::string& file = "", int line = 0,
                         const std::string& function = "");
    
    // Debug utilities
    void setWatchpoint(const std::string& expression,
                      const std::function<bool(const std::string&, const std::string&)>& condition,
                      const std::string& context = "");
    
    void removeWatchpoint(const std::string& expression);
    
    void triggerWatchpoint(const std::string& expression,
                          const std::string& variableName,
                          const std::string& oldValue,
                          const std::string& newValue);
    
    void setBreakpoint(const std::string& location,
                      const std::string& condition = "");
    
    void removeBreakpoint(const std::string& location);
    
    std::vector<DebugInfo> getDebugLog() const;
    void clearDebugLog();
    
    // Export functionality
    void exportToFile(const std::string& filepath) const;
    void exportToHTML(const std::string& filepath) const;
    std::string exportToString(const std::string& format = "text") const;
    
    // Get global debug logger instance
    static DebugLogger& getGlobal();
    
    // Set global debug logger
    static void setGlobal(std::shared_ptr<DebugLogger> logger);
};

// Convenience macros for debug logging
#define NLM_LOG_STACK_TRACE() \
    nlm::DebugLogger::getGlobal().logStackTrace(__func__, __FILE__, __LINE__, __func__)

#define NLM_LOG_BREAKPOINT(location, condition) \
    nlm::DebugLogger::getGlobal().logBreakpoint(location, condition, "", __FILE__, __LINE__, __func__)

#define NLM_LOG_WATCHPOINT(expr, var, old, new) \
    nlm::DebugLogger::getGlobal().logWatchpoint(expr, var, old, new, __FILE__, __LINE__, __func__)

#define NLM_LOG_FUNCTION_ENTRY(funcName) \
    nlm::DebugLogger::getGlobal().logFunctionEntry(funcName, {}, __FILE__, __LINE__, __func__)

#define NLM_LOG_FUNCTION_EXIT(funcName, timeMs, returnValue) \
    nlm::DebugLogger::getGlobal().logFunctionExit(funcName, timeMs, returnValue, __FILE__, __LINE__, __func__)

#define NLM_LOG_EXCEPTION(funcName, excType, excMsg) \
    nlm::DebugLogger::getGlobal().logFunctionException(funcName, excType, excMsg, {}, __FILE__, __LINE__, __func__)

} // namespace nlm

#endif // DEBUG_LOGGER_HPP