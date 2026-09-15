#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <chrono>

namespace nlm {

// Debug level enum
enum class DebugLevel {
    Off = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Trace = 5
};

// Debug event types
enum class DebugEventType {
    SystemStart,
    SystemStop,
    StepCompleted,
    ActionExecuted,
    ErrorOccurred,
    WarningIssued,
    MemoryAllocated,
    MemoryFreed,
    SpikeEmitted,
    SynapseUpdated,
    RegionCreated,
    RegionDestroyed,
    PluginLoaded,
    PluginUnloaded,
    Custom
};

// Debug event structure
struct DebugEvent {
    DebugEventType type;
    std::string message;
    std::string source;
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    DebugEvent() : type(DebugEventType::SystemStart), timestamp(std::chrono::system_clock::now()) {}
    DebugEvent(DebugEventType t, const std::string& msg, const std::string& src = "")
        : type(t), message(msg), source(src), timestamp(std::chrono::system_clock::now()) {}
};

// Debug hook function type
using DebugHook = std::function<void(const DebugEvent&)>;

// Debug utility for logging, profiling, and visualization
class DebugUtils {
public:
    DebugUtils();
    ~DebugUtils();
    
    // Disable copying
    DebugUtils(const DebugUtils&) = delete;
    DebugUtils& operator=(const DebugUtils&) = delete;
    
    // Set global debug level
    static void setDebugLevel(DebugLevel level);
    
    // Get current debug level
    static DebugLevel getDebugLevel();
    
    // Log a message at a specific level
    static void log(DebugLevel level, const std::string& message, const std::string& source = "");
    
    // Log an error
    static void logError(const std::string& message, const std::string& source = "");
    
    // Log a warning
    static void logWarning(const std::string& message, const std::string& source = "");
    
    // Log info
    static void logInfo(const std::string& message, const std::string& source = "");
    
    // Log debug message
    static void logDebug(const std::string& message, const std::string& source = "");
    
    // Log trace
    static void logTrace(const std::string& message, const std::string& source = "");
    
    // Emit a debug event
    static void emitEvent(DebugEventType type, const std::string& message, 
                         const std::string& source = "", 
                         const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Add a debug hook
    static void addDebugHook(DebugHook hook);
    
    // Remove a debug hook
    static void removeDebugHook(DebugHook hook);
    
    // Clear all debug hooks
    static void clearDebugHooks();
    
    // Get recent debug events
    static std::vector<DebugEvent> getRecentEvents(size_t maxEvents = 100);
    
    // Get events by type
    static std::vector<DebugEvent> getEventsByType(DebugEventType type, size_t maxEvents = 100);
    
    // Get events by source
    static std::vector<DebugEvent> getEventsBySource(const std::string& source, size_t maxEvents = 100);
    
    // Get event count
    static size_t getEventCount() { return getRecentEvents().size(); }
    
    // Enable/disable debug logging
    static void setEnabled(bool enabled) { enabled_ = enabled; }
    
    // Check if debug is enabled
    static bool isEnabled() { return enabled_; }
    
    // Start/stop system profiling
    static void startSystemProfile();
    static void stopSystemProfile();
    static bool isSystemProfiling();
    
    // Get system profiling statistics
    static std::string getSystemProfileStats();
    
    // Clear debug history
    static void clearHistory();
    
    // Save debug history to file
    static bool saveHistory(const std::string& filepath) const;
    
    // Load debug history from file
    static bool loadHistory(const std::string& filepath);
    
    // Start performance profiling
    static void startPerformanceProfile(const std::string& operationName);
    static void stopPerformanceProfile(const std::string& operationName);
    
    // Get performance profile summary
    static std::string getPerformanceProfileSummary();
    
    // Enable/disable visualization
    static void setVisualizationEnabled(bool enabled) { visualizationEnabled_ = enabled; }
    static bool isVisualizationEnabled() { return visualizationEnabled_; }
    
    // Export debug data for external visualization
    static std::string exportForVisualization();
    
    // Check for specific condition and log if true
    static void checkCondition(bool condition, DebugLevel level, 
                              const std::string& message, 
                              const std::string& source = "",
                              const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Set log file
    static void setLogFile(const std::string& filepath);
    
    // Get current log file
    static std::string getLogFile();
    
private:
    struct Impl;
    static std::unique_ptr<Impl> pImpl;
    static bool enabled_;
    static bool visualizationEnabled_;
    static std::string logFile_;
    
    // Helper functions
    static std::string getTimestamp() noexcept;
};

// Convenience macros
#define DEBUG_LOG(level, message) nlm::DebugUtils::log(level, message, __FILE__)
#define DEBUG_LOG_ERROR(message) nlm::DebugUtils::logError(message, __FILE__)
#define DEBUG_LOG_WARNING(message) nlm::DebugUtils::logWarning(message, __FILE__)
#define DEBUG_LOG_INFO(message) nlm::DebugUtils::logInfo(message, __FILE__)
#define DEBUG_LOG_DEBUG(message) nlm::DebugUtils::logDebug(message, __FILE__)
#define DEBUG_LOG_TRACE(message) nlm::DebugUtils::logTrace(message, __FILE__)

#define DEBUG_CHECK(condition, level, message) \
    if (condition) { \
        nlm::DebugUtils::checkCondition(true, level, message, __FILE__); \
    }

// System event macros
#define DEBUG_SYSTEM_START() nlm::DebugUtils::emitEvent(nlm::DebugEventType::SystemStart, "System started")
#define DEBUG_SYSTEM_STOP() nlm::DebugUtils::emitEvent(nlm::DebugEventType::SystemStop, "System stopped")

// Performance profiling macros
#define PERF_PROFILE(operationName) \
    nlm::DebugUtils::startPerformanceProfile(operationName); \
    auto __perf_timer##__LINE__ = nlm::Timer(); \
    __attribute__((destructor)) void __perf_cleanup##__LINE__() { \
        nlm::DebugUtils::stopPerformanceProfile(operationName); \
    }

} // namespace nlm