#pragma once

#include "Logger.hpp"
#include "AdvancedConfig.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <map>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>
#include <variant>

namespace nlm {

// Advanced debugging types
using BreakpointId = uint64_t;
using WatchVariableId = uint64_t;
using ProfileId = uint64_t;
using EventType = enum class EventType {
    FunctionEntry,
    FunctionExit,
    VariableChange,
    MemoryAllocation,
    MemoryDeallocation,
    ThreadStart,
    ThreadEnd,
    Exception,
    SystemCall,
    Custom
};

using PerformanceMetricType = enum class PerformanceMetricType {
    ExecutionTime,
    MemoryUsage,
    CPUUsage,
    NetworkTraffic,
    FileSystemAccess,
    SystemCalls,
    ContextSwitches,
    PageFaults
};

using ProfilingTechnique = enum class ProfilingTechnique {
    Sampling,
    Instruction,
    Memory,
    LineProfile,
    CallTree,
    FlatProfile,
    LockProfiling,
    ThreadAware
};

using DebugActionType = enum class DebugActionType {
    Breakpoint,
    WatchVariable,
    StepInto,
    StepOver,
    StepOut,
    Continue,
    Interrupt,
    Attach,
    Detach
};

using PerformanceAnalysisType = enum class PerformanceAnalysisType {
    BottleneckDetection,
    OptimizationSuggestions,
    ComparativeAnalysis,
    TrendAnalysis,
    RegressionDetection,
    Profiling,
    Benchmarking
};

// Breakpoint configuration
struct Breakpoint {
    BreakpointId id;
    std::string file;
    int line;
    std::string function;
    std::string condition;
    bool enabled;
    bool hitCount;
    int hitCountThreshold;
    std::string action;
    std::string message;
    bool persistent;
    std::chrono::system_clock::time_point creationTime;
    
    Breakpoint() : id(0), line(0), enabled(true), hitCount(false), 
                   hitCountThreshold(1), persistent(false) {
        creationTime = std::chrono::system_clock::now();
    }
};

// Watch variable configuration
struct WatchVariable {
    WatchVariableId id;
    std::string expression;
    std::string description;
    std::string type;
    std::string format;
    std::string category;
    bool enabled;
    std::chrono::system_clock::time_point lastEvaluated;
    double evaluationTime;
    std::string lastValue;
    std::vector<std::string> history;
    size_t maxHistorySize;
    std::function<std::string()> evaluator;
    
    WatchVariable() : id(0), enabled(true), evaluationTime(0.0), maxHistorySize(100) {}
};

// Debug event
struct DebugEvent {
    std::chrono::system_clock::time_point timestamp;
    EventType type;
    std::string source;
    std::string message;
    std::unordered_map<std::string, std::string> data;
    std::vector<std::string> stackTrace;
    
    DebugEvent() : timestamp(std::chrono::system_clock::now()) {}
    DebugEvent(EventType t, const std::string& s, const std::string& m) 
        : type(t), source(s), message(m) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Performance metric
struct PerformanceMetric {
    PerformanceMetricType type;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    std::string context;
    std::unordered_map<std::string, std::string> metadata;
    
    PerformanceMetric() : type(PerformanceMetricType::ExecutionTime), value(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Profiling session
struct ProfilingSession {
    ProfileId id;
    std::string name;
    ProfilingTechnique technique;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::vector<PerformanceMetric> metrics;
    std::string targetProcess;
    bool isActive;
    double samplingRate;
    std::unordered_map<std::string, double> customSettings;
    
    ProfilingSession() : id(0), isActive(false), samplingRate(1000.0) {
        startTime = std::chrono::system_clock::now();
    }
};

// Debug session state
struct DebugSessionState {
    std::chrono::system_clock::time_point sessionStartTime;
    std::vector<DebugEvent> events;
    std::vector<std::string> currentStackTrace;
    std::unordered_map<std::string, std::string> localVariables;
    std::unordered_map<std::string, std::string> globalVariables;
    BreakpointId currentBreakpoint;
    DebugActionType currentAction;
    bool isPaused;
    std::string currentFile;
    int currentLine;
    std::string currentFunction;
    
    DebugSessionState() : currentBreakpoint(0), currentAction(DebugActionType::Continue), 
                         isPaused(false), currentLine(0) {
        sessionStartTime = std::chrono::system_clock::now();
    }
};

// Advanced debugger with breakpoints, watch variables, profiling, and event system
class AdvancedDebugger {
public:
    AdvancedDebugger();
    AdvancedDebugger(const AdvancedConfig& config);
    ~AdvancedDebugger();
    
    // Debugger control
    void start();
    void stop();
    bool isRunning() const;
    void pause();
    void resume();
    bool isPaused() const;
    
    // Breakpoint management
    BreakpointId addBreakpoint(const Breakpoint& bp);
    bool removeBreakpoint(BreakpointId id);
    bool enableBreakpoint(BreakpointId id);
    bool disableBreakpoint(BreakpointId id);
    Breakpoint getBreakpoint(BreakpointId id) const;
    std::vector<Breakpoint> getAllBreakpoints() const;
    std::vector<Breakpoint> getBreakpoints(const std::string& file) const;
    
    // Watch variable management
    WatchVariableId addWatchVariable(const WatchVariable& watch);
    bool removeWatchVariable(WatchVariableId id);
    bool updateWatchVariable(WatchVariableId id, const WatchVariable& watch);
    WatchVariable getWatchVariable(WatchVariableId id) const;
    std::vector<WatchVariable> getAllWatchVariables() const;
    std::vector<std::pair<WatchVariableId, std::string>> evaluateAllWatchVariables();
    
    // Event handling
    void addEventHandler(const std::string& eventType, const std::function<void(const DebugEvent&)>& handler);
    void removeEventHandler(const std::string& eventType);
    void emitEvent(EventType type, const std::string& source, const std::string& message,
                  const std::unordered_map<std::string, std::string>& data = {});
    std::vector<DebugEvent> getRecentEvents(size_t limit = 100) const;
    
    // Profiling management
    ProfileId startProfiling(const ProfilingSession& session);
    bool stopProfiling(ProfileId id);
    ProfilingSession getProfilingSession(ProfileId id) const;
    std::vector<ProfilingSession> getAllProfilingSessions() const;
    bool addPerformanceMetric(ProfileId id, const PerformanceMetric& metric);
    std::vector<PerformanceMetric> getPerformanceMetrics(ProfileId id) const;
    
    // Debugging actions
    void executeAction(DebugActionType action, const std::string& target = "");
    void stepInto();
    void stepOver();
    void stepOut();
    void continueExecution();
    void interrupt();
    
    // Stack and variable inspection
    std::vector<std::string> getCurrentStackTrace() const;
    std::unordered_map<std::string, std::string> getLocalVariables() const;
    std::unordered_map<std::string, std::string> getGlobalVariables() const;
    std::string evaluateExpression(const std::string& expression) const;
    
    // Session management
    void saveSession(const std::string& filename) const;
    bool loadSession(const std::string& filename);
    void clearSession();
    DebugSessionState getSessionState() const;
    
    // Configuration
    void setConfig(const AdvancedConfig& config);
    const AdvancedConfig& getConfig() const;
    void updateConfig(const std::string& key, const std::string& value);
    
    // Performance analysis
    std::string analyzePerformance(const std::string& analysisType,
                                   const std::unordered_map<std::string, std::string>& options = {});
    std::string detectBottlenecks() const;
    std::string suggestOptimizations() const;
    std::string performComparativeAnalysis(const std::string& baselineId) const;
    
    // Memory debugging
    void trackMemoryAllocation(void* pointer, size_t size, const std::string& context);
    void trackMemoryDeallocation(void* pointer);
    std::vector<std::string> getMemoryLeaks() const;
    double getCurrentMemoryUsage() const;
    
    // Thread debugging
    void trackThreadStart(std::thread::id threadId, const std::string& name);
    void trackThreadEnd(std::thread::id threadId);
    std::vector<std::string> getActiveThreads() const;
    std::string getThreadStackTrace(std::thread::id threadId) const;
    
    // File and system monitoring
    void trackFileAccess(const std::string& path, bool read, bool write);
    void trackSystemCall(const std::string& call, const std::string& arguments);
    std::vector<std::string> getSystemCallLog() const;
    std::vector<std::string> getFileAccessLog() const;
    
    // Error handling and reporting
    std::string getLastError() const;
    std::vector<std::string> getAllErrors() const;
    void clearErrors();
    bool hasErrors() const;
    
    // Export and import
    bool exportDebugData(const std::string& filepath) const;
    bool importDebugData(const std::string& filepath);
    std::string generateDebugReport() const;
    
    // Integration with existing systems
    void attachToProcess(const std::string& processName);
    void detachFromProcess();
    bool isAttached() const;
    void setBreakpointInSource(const std::string& file, int line, const std::string& condition = "");
    
    // Utility functions
    static std::string getDebuggerVersion();
    static std::vector<std::string> getSupportedFeatures();
    static std::string getCurrentStackTraceAsString();
    static std::string formatTime(std::chrono::system_clock::time_point time);
    
    // Statistics and monitoring
    struct DebuggerStats {
        size_t totalBreakpoints;
        size_t enabledBreakpoints;
        size_t totalWatchVariables;
        size_t activeWatchVariables;
        size_t eventsProcessed;
        size_t profilingSessions;
        std::chrono::system_clock::time_point lastUpdate;
        double memoryUsage;
        double cpuUsage;
    };
    
    DebuggerStats getStats() const;
    std::string getStatsReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void updateSessionState();
    void checkBreakpoints();
    void evaluateWatchVariables();
    bool shouldBreak(const Breakpoint& bp, const std::unordered_map<std::string, std::string>& variables) const;
    void applyBreakpointAction(const Breakpoint& bp);
    void generateStackTrace();
    void logDebugEvent(EventType type, const std::string& source, const std::string& message,
                      const std::unordered_map<std::string, std::string>& data = {});
};

// Advanced profiler with sampling, instruction-level, and memory profiling
class AdvancedProfiler {
public:
    AdvancedProfiler();
    AdvancedProfiler(const AdvancedConfig& config);
    ~AdvancedProfiler();
    
    // Profiling control
    bool startProfiling(const std::string& name, ProfilingTechnique technique);
    void stopProfiling();
    bool isProfiling() const;
    
    // Configuration
    void setConfig(const AdvancedConfig& config);
    const AdvancedConfig& getConfig() const;
    void updateConfig(const std::string& key, const std::string& value);
    
    // Data collection
    void collectSample();
    void addMetric(PerformanceMetricType type, double value, const std::string& context = "");
    void setTargetProcess(const std::string& processName);
    
    // Analysis
    std::string analyzePerformance(PerformanceAnalysisType analysisType,
                                   const std::unordered_map<std::string, std::string>& options = {});
    std::string generateProfileReport() const;
    std::string exportProfileData(const std::string& filepath, const std::string& format = "CSV") const;
    
    // Comparison and benchmarking
    void addBenchmarkResult(const std::string& benchmarkName, double executionTime,
                           const std::unordered_map<std::string, double>& metrics);
    std::string compareProfiles(const std::string& profile1, const std::string& profile2) const;
    
    // Memory profiling
    void trackMemoryAllocation(void* pointer, size_t size, const std::string& context);
    void trackMemoryDeallocation(void* pointer);
    std::vector<std::string> getMemoryProfile() const;
    
    // Thread-aware profiling
    void trackThreadStart(std::thread::id threadId);
    void trackThreadEnd(std::thread::id threadId);
    std::string getThreadProfile(std::thread::id threadId) const;
    
    // Statistics
    struct ProfilerStats {
        double totalExecutionTime;
        double totalMemoryUsage;
        double averageCpuUsage;
        size_t totalSamples;
        size_t activeThreads;
        std::chrono::system_clock::time_point lastSample;
    };
    
    ProfilerStats getStats() const;
    std::string getStatsReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void updatePerformanceMetrics();
    void generateProfileData();
    void cleanupOldData();
};

// System call tracer and interceptor
class SystemCallTracer {
public:
    SystemCallTracer();
    ~SystemCallTracer();
    
    // Control
    void start();
    void stop();
    bool isRunning() const;
    
    // Configuration
    void setConfig(const AdvancedConfig& config);
    const AdvancedConfig& getConfig() const;
    
    // System call interception
    std::string interceptSystemCall(const std::string& call, const std::string& args);
    void addSystemCallFilter(const std::string& pattern, bool shouldIntercept);
    void removeSystemCallFilter(const std::string& pattern);
    std::vector<std::string> getInterceptedSystemCalls() const;
    
    // File system monitoring
    void trackFileAccess(const std::string& path, const std::string& operation);
    std::vector<std::string> getFileAccessLog() const;
    
    // Network monitoring
    void trackNetworkTraffic(const std::string& endpoint, double bytes, const std::string& protocol);
    std::vector<std::string> getNetworkLog() const;
    
    // Error handling
    std::string getLastError() const;
    std::vector<std::string> getAllErrors() const;
    void clearErrors();
    
    // Export and import
    bool exportTraceData(const std::string& filepath) const;
    bool importTraceData(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Exception handler with stack unwinding and analysis
class AdvancedExceptionHandler {
public:
    AdvancedExceptionHandler();
    ~AdvancedExceptionHandler();
    
    // Configuration
    void setConfig(const AdvancedConfig& config);
    const AdvancedConfig& getConfig() const;
    
    // Exception handling
    template<typename T>
    T handleException(const std::string& context, const T& defaultValue = T()) {
        try {
            // Simulate exception handling
            throw std::runtime_error("Simulated exception");
        } catch (const std::exception& e) {
            logException(context, e);
            return defaultValue;
        }
    }
    
    void registerExceptionHandler(const std::string& exceptionType,
                                 const std::function<void(const std::exception&)>& handler);
    void unregisterExceptionHandler(const std::string& exceptionType);
    
    // Stack unwinding
    std::string getCurrentStackTrace() const;
    std::vector<std::string> getStackTrace(void* context = nullptr) const;
    
    // Exception analysis
    std::string analyzeException(const std::exception& e) const;
    std::string getExceptionStatistics() const;
    
    // Recovery mechanisms
    void addRecoveryAction(const std::string& condition, const std::function<void()>& action);
    void removeRecoveryAction(const std::string& condition);
    void attemptRecovery(const std::exception& e);
    
    // Error reporting
    std::string generateExceptionReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void logException(const std::string& context, const std::exception& e);
};

// Memory debugger with leak detection and allocation tracking
class AdvancedMemoryDebugger {
public:
    AdvancedMemoryDebugger();
    ~AdvancedMemoryDebugger();
    
    // Configuration
    void setConfig(const AdvancedConfig& config);
    const AdvancedConfig& getConfig() const;
    
    // Memory tracking
    void* trackAllocation(size_t size, const std::string& context, const std::string& file = "", int line = 0);
    void trackDeallocation(void* pointer);
    void trackReallocation(void* oldPointer, size_t newSize, const std::string& context);
    
    // Leak detection
    std::vector<std::string> detectMemoryLeaks() const;
    std::vector<std::string> detectMemoryLeaks(const std::string& context) const;
    double getTotalMemoryLeaked() const;
    
    // Heap analysis
    std::string analyzeHeap() const;
    std::string getAllocationHistory(size_t limit = 100) const;
    std::unordered_map<std::string, size_t> getAllocationStatsByContext() const;
    
    // Bounds checking
    bool enableBoundsChecking(void* pointer, size_t size);
    bool disableBoundsChecking(void* pointer);
    bool checkBounds(void* pointer, size_t accessSize) const;
    
    // Performance impact analysis
    double getMemoryUsage() const;
    double getPeakMemoryUsage() const;
    std::string getMemoryPerformanceReport() const;
    
    // Error handling
    std::string getLastError() const;
    std::vector<std::string> getAllErrors() const;
    void clearErrors();
    
    // Export and import
    bool exportMemoryData(const std::string& filepath) const;
    bool importMemoryData(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    void updateMemoryStats();
    bool isValidPointer(void* pointer) const;
    std::string formatAllocationInfo(void* pointer, size_t size, const std::string& context) const;
};

// Debug utilities
namespace DebugUtils {
    // Stack trace utilities
    std::string getCurrentStackTrace();
    std::vector<std::string> getStackTrace(void* context = nullptr);
    std::string formatStackTrace(const std::vector<std::string>& stackTrace);
    
    // Time and performance utilities
    double getElapsedTime(std::chrono::high_resolution_clock::time_point startTime);
    std::string formatTime(double seconds);
    std::string formatMemorySize(size_t bytes);
    
    // Process utilities
    std::string getProcessId();
    std::string getProcessName();
    std::vector<std::string> getActiveThreads();
    
    // File and system utilities
    std::string getCurrentWorkingDirectory();
    bool fileExists(const std::string& path);
    std::string getFileContent(const std::string& path);
    bool createDirectory(const std::string& path);
    
    // JSON utilities for debug data
    std::string serializeToJSON(const std::vector<DebugEvent>& events);
    std::string serializeToJSON(const std::vector<PerformanceMetric>& metrics);
    bool parseFromJSON(const std::string& jsonData, std::vector<DebugEvent>& events);
    
    // Error utilities
    std::string formatError(const std::exception& e);
    std::vector<std::string> splitErrorMessage(const std::string& message);
    
    // Profiling utilities
    std::string generateProfileHeader(const std::string& name, ProfilingTechnique technique);
    std::string generateProfileFooter();
    std::string formatMetric(const PerformanceMetric& metric);
}

} // namespace nlm
