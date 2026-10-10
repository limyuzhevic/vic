#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <vector>
#include <unordered_map>
#include <functional>
#include "NLMExceptions.hpp"
#include "Logger.hpp"

namespace nlm {

// Debug severity levels (expanded from existing LogLevel)
enum class DebugLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

// Debug entry with additional metadata
struct DebugEntry {
    DebugLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::map<std::string, std::string> metadata;
    bool isBreakpointHit = false;
};

// Debug condition for breakpoints
class DebugCondition {
public:
    virtual ~DebugCondition() = default;
    virtual bool evaluate(const DebugEntry& entry) const = 0;
    virtual std::string getDescription() const = 0;
};

// Variable information for inspection
struct VariableInfo {
    std::string name;
    std::string type;
    std::string value;
    std::string address;
    std::map<std::string, std::string> properties;
};

// Stack frame information
struct StackFrame {
    std::string function;
    std::string file;
    int line;
    std::vector<VariableInfo> locals;
    std::vector<VariableInfo> parameters;
};

// Full stack trace
struct StackTrace {
    int id;
    std::vector<StackFrame> frames;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::string description;
};

// Breakpoint information
struct Breakpoint {
    int id;
    std::string file;
    int line;
    std::string condition; // Expression string
    DebugLevel triggerLevel;
    bool enabled = true;
    int hitCount = 0;
    std::chrono::system_clock::time_point lastHit;
};

// Debug session state
struct DebugSessionState {
    bool isPaused = false;
    bool isStepping = false;
    bool isRunning = false;
    DebugLevel currentLevel = DebugLevel::DEBUG;
    std::string currentFile;
    int currentLine = 0;
    int stepCount = 0;
    std::vector<StackTrace> callStack;
    std::vector<DebugEntry> logHistory;
    std::map<std::string, std::string> variableInspection;
    std::chrono::system_clock::time_point sessionStart;
};

// DebugLogger - Enhanced debugging with multiple levels and features
class DebugLogger {
public:
    DebugLogger();
    ~DebugLogger();
    
    // Log with debug level
    void log(DebugLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "", const std::string& threadId = "");
    
    // Convenience methods
    void trace(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& threadId = "");
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& threadId = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& threadId = "");
    void warn(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& threadId = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& threadId = "");
    void fatal(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "", const std::string& threadId = "");
    
    // Set logging level
    void setLevel(DebugLevel level);
    DebugLevel getLevel() const;
    
    // Add metadata to log entries
    void addMetadata(const std::string& key, const std::string& value);
    void removeMetadata(const std::string& key);
    std::string getMetadata(const std::string& key) const;
    
    // Get log history
    std::vector<DebugEntry> getLogHistory(size_t maxEntries = 1000) const;
    void clearLogHistory();
    
    // Enable/disable debugging
    void enable(bool enabled = true);
    bool isEnabled() const;
    
    // Get global debug logger instance
    static DebugLogger& getGlobal();
    
    // Set global debug logger
    static void setGlobal(std::shared_ptr<DebugLogger> logger);
    
    // Export logs to file
    bool exportLogs(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// DebugSession - Manages debugging state and control
class DebugSession {
public:
    DebugSession();
    ~DebugSession();
    
    // Session control
    void start();
    void stop();
    void pause();
    void resume();
    void step();
    
    // Check session state
    bool isRunning() const;
    bool isPaused() const;
    bool isStepping() const;
    
    // Get session state
    DebugSessionState getState() const;
    
    // Set current debugging location
    void setLocation(const std::string& file, int line, const std::string& function = "");
    
    // Get call stack
    std::vector<StackTrace> getCallStack() const;
    void captureCallStack();
    
    // Variable inspection
    bool inspectVariable(const std::string& name, VariableInfo& info);
    std::map<std::string, VariableInfo> inspectAllVariables() const;
    
    // Memory debugging
    void startMemoryTracking();
    void stopMemoryTracking();
    bool isMemoryTracking() const;
    std::map<uintptr_t, std::string> getMemoryAllocations() const;
    
    // Performance debugging
    void startPerformanceTracking(const std::string& operation);
    void stopPerformanceTracking();
    std::map<std::string, double> getPerformanceMetrics() const;
    
    // Thread debugging
    void logThreadEnter(const std::string& threadName, const std::string& function);
    void logThreadExit(const std::string& threadName, const std::string& function);
    std::vector<std::string> getActiveThreads() const;
    
    // Simulation state inspection
    void captureSimulationState(std::map<std::string, std::string>& state) const;
    
    // Get global debug session instance
    static DebugSession& getGlobal();
    
    // Set global debug session
    static void setGlobal(std::shared_ptr<DebugSession> session);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// BreakpointManager - Manages breakpoints and conditional execution
class BreakpointManager {
public:
    BreakpointManager();
    ~BreakpointManager();
    
    // Add breakpoint
    int addBreakpoint(const std::string& file, int line,
                     const std::string& condition = "",
                     DebugLevel triggerLevel = DebugLevel::DEBUG);
    
    // Remove breakpoint
    bool removeBreakpoint(int id);
    bool removeBreakpoint(const std::string& file, int line);
    
    // Get breakpoint
    std::optional<Breakpoint> getBreakpoint(int id) const;
    std::vector<Breakpoint> getBreakpoints() const;
    
    // Enable/disable breakpoint
    void enableBreakpoint(int id, bool enabled = true);
    void enableBreakpoint(const std::string& file, int line, bool enabled = true);
    
    // Check if breakpoint should trigger
    bool shouldBreakpointTrigger(const DebugEntry& entry);
    
    // Get hit count for breakpoint
    int getBreakpointHitCount(int id) const;
    
    // Clear all breakpoints
    void clearBreakpoints();
    
    // Get global breakpoint manager instance
    static BreakpointManager& getGlobal();
    
    // Set global breakpoint manager
    static void setGlobal(std::shared_ptr<BreakpointManager> manager);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// VariableInspector - Inspects variables and memory
class VariableInspector {
public:
    VariableInspector();
    ~VariableInspector();
    
    // Register variable for inspection
    void registerVariable(const std::string& name, const std::string& type,
                         const std::string& value, const std::string& address = "");
    
    // Unregister variable
    void unregisterVariable(const std::string& name);
    
    // Get variable info
    std::optional<VariableInfo> getVariableInfo(const std::string& name) const;
    std::map<std::string, VariableInfo> getAllVariables() const;
    
    // Update variable value
    void updateVariable(const std::string& name, const std::string& value);
    
    // Add variable property
    void addVariableProperty(const std::string& name, const std::string& key,
                           const std::string& value);
    
    // Clear all variables
    void clearVariables();
    
    // Memory inspection
    bool inspectMemoryRegion(uintptr_t address, size_t size,
                           std::string& output) const;
    
    // Search variables
    std::vector<std::string> searchVariables(const std::string& pattern) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PerformanceProfiler - Performance profiling capabilities
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start profiling an operation
    void startProfiling(const std::string& operationName,
                       const std::string& category = "default");
    
    // Stop profiling
    double stopProfiling(const std::string& operationName);
    
    // Get profiling results
    std::map<std::string, double> getProfileResults(const std::string& category = "") const;
    double getLastResult(const std::string& operationName) const;
    
    // Clear profiling data
    void clearProfile(const std::string& operationName = "");
    void clearAllProfiles();
    
    // Performance metrics
    struct PerformanceMetrics {
        std::string operationName;
        double duration;           // in microseconds
        double cpuTime;            // CPU time in microseconds
        size_t memoryAllocated;    // bytes
        size_t memoryFreed;        // bytes
        int threadId;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> metadata;
    };
    
    // Get detailed metrics
    PerformanceMetrics getDetailedMetrics(const std::string& operationName) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// MemoryProfiler - Memory profiling and leak detection
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Track memory allocation
    void trackAllocation(void* address, size_t size, const std::string& type = "",
                       const std::string& file = "", int line = 0);
    
    // Track memory deallocation
    void trackDeallocation(void* address);
    
    // Get memory statistics
    struct MemoryStats {
        size_t totalAllocated;     // bytes
        size_t totalFreed;         // bytes
        size_t currentUsage;       // bytes
        size_t allocationCount;    // number of allocations
        size_t deallocationCount;  // number of deallocations
        std::map<std::string, size_t> allocationsByType;
        std::chrono::system_clock::time_point startTime;
    };
    
    MemoryStats getMemoryStats() const;
    
    // Check for memory leaks
    std::vector<void*> getMemoryLeaks() const;
    bool hasMemoryLeaks() const;
    
    // Enable/disable memory tracking
    void enableTracking(bool enabled = true);
    bool isTracking() const;
    
    // Clear memory tracking
    void clearTracking();
    
    // Generate memory report
    std::string generateMemoryReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// CPUProfiler - CPU profiling with instruction-level analysis
class CPUProfiler {
public:
    CPUProfiler();
    ~CPUProfiler();
    
    // Start CPU profiling for thread
    void startCPUProfiling(int threadId = -1);
    
    // Stop CPU profiling
    void stopCPUProfiling(int threadId = -1);
    
    // Get CPU usage statistics
    struct CPUStats {
        int threadId;
        double cpuUsage;           // percentage (0-100)
        double userTime;           // user CPU time in seconds
        double systemTime;         // system CPU time in seconds
        size_t instructionCount;   // number of instructions executed
        std::chrono::system_clock::time_point timestamp;
    };
    
    CPUStats getCPUStats(int threadId = -1) const;
    std::map<int, CPUStats> getAllCPUStats() const;
    
    // Enable/disable CPU profiling
    void enableProfiling(bool enabled = true);
    bool isProfiling() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// I/OProfiler - I/O operation profiling
class I/OProfiler {
public:
    I/OProfiler();
    ~I/OProfiler();
    
    // Track read operation
    void trackRead(const std::string& file, size_t bytes, double duration,
                  const std::string& operation = "read");
    
    // Track write operation
    void trackWrite(const std::string& file, size_t bytes, double duration,
                   const std::string& operation = "write");
    
    // Track open/close operations
    void trackOpen(const std::string& file, const std::string& mode);
    void trackClose(const std::string& file);
    
    // Get I/O statistics
    struct IOStats {
        size_t reads;              // number of read operations
        size_t writes;             // number of write operations
        size_t opens;              // number of open operations
        size_t closes;             // number of close operations
        size_t totalBytesRead;     // bytes read
        size_t totalBytesWritten;  // bytes written
        double totalReadTime;      // total time spent reading
        double totalWriteTime;     // total time spent writing
        std::map<std::string, size_t> readsByFile;
        std::map<std::string, size_t> writesByFile;
    };
    
    IOStats getIOStats() const;
    
    // Clear I/O tracking
    void clearIOStats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// NetworkProfiler - Network activity profiling
class NetworkProfiler {
public:
    NetworkProfiler();
    ~NetworkProfiler();
    
    // Track connection
    void trackConnect(const std::string& host, int port, double duration);
    void trackDisconnect(const std::string& host, int port);
    
    // Track send/receive
    void trackSend(const std::string& host, int port, size_t bytes, double duration);
    void trackReceive(const std::string& host, int port, size_t bytes, double duration);
    
    // Track request/response
    void trackRequest(const std::string& method, const std::string& url,
                     double duration, int statusCode = 0);
    void trackResponse(const std::string& method, const std::string& url,
                      double duration, int statusCode = 0);
    
    // Get network statistics
    struct NetworkStats {
        size_t connections;         // number of connections
        size_t disconnects;         // number of disconnects
        size_t sends;               // number of send operations
        size_t receives;            // number of receive operations
        size_t requests;            // number of requests
        size_t responses;           // number of responses
        size_t totalBytesSent;      // bytes sent
        size_t totalBytesReceived;  // bytes received
        double totalConnectTime;    // total connection time
        double totalSendTime;       // total send time
        double totalReceiveTime;   // total receive time
        std::map<std::string, size_t> connectionsByHost;
        std::map<std::string, size_t> requestsByMethod;
    };
    
    NetworkStats getNetworkStats() const;
    
    // Clear network tracking
    void clearNetworkStats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// AlgorithmProfiler - Algorithm performance analysis
class AlgorithmProfiler {
public:
    AlgorithmProfiler();
    ~AlgorithmProfiler();
    
    // Start algorithm profiling
    void startAlgorithmProfiling(const std::string& algorithmName,
                                const std::string& category = "default");
    
    // Stop algorithm profiling
    double stopAlgorithmProfiling(const std::string& algorithmName);
    
    // Get algorithm statistics
    struct AlgorithmStats {
        std::string algorithmName;
        double executionTime;       // in seconds
        size_t inputSize;           // size of input data
        size_t outputSize;          // size of output data
        double cpuUtilization;      // average CPU utilization
        size_t memoryPeak;          // peak memory usage in bytes
        int complexity;             // estimated complexity score
        std::string category;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::map<std::string, double> metrics;
    };
    
    AlgorithmStats getAlgorithmStats(const std::string& algorithmName) const;
    std::map<std::string, AlgorithmStats> getAllAlgorithmStats() const;
    
    // Compare algorithm performance
    double compareAlgorithmPerformance(const std::string& algorithm1,
                                       const std::string& algorithm2,
                                       const std::string& metric = "time") const;
    
    // Clear algorithm profiling
    void clearAlgorithmProfiling(const std::string& algorithmName = "");
    void clearAllAlgorithmProfiling();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
