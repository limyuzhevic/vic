#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <map>
#include <set>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <random>
#include <cmath>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <stdexcept>
#include <cstring>
#include <ctime>

namespace nlm {

// Memory debugging utilities
struct MemoryBlockInfo {
    void* pointer;
    size_t size;
    std::string allocationSite;
    std::string functionName;
    std::string fileName;
    int lineNumber;
    std::chrono::system_clock::time_point allocationTime;
    std::string threadId;
    bool isActive;
    std::unordered_map<std::string, std::string> metadata;
};

class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    // Track memory allocations
    void trackAllocation(void* ptr, size_t size, const std::string& function, 
                        const std::string& file, int line, const std::string& threadId = "");
    
    // Track memory deallocations
    void trackDeallocation(void* ptr, const std::string& function,
                          const std::string& file, int line, const std::string& threadId = "");
    
    // Get memory statistics
    size_t getTotalAllocated() const;
    size_t getActiveAllocations() const;
    size_t getPeakMemoryUsage() const;
    std::vector<MemoryBlockInfo> getActiveAllocations(const std::string& filter = "") const;
    std::vector<MemoryBlockInfo> getAllocationHistory(size_t limit = 100) const;
    
    // Memory analysis
    void analyzeLeaks();
    void analyzeFragmentation();
    double calculateFragmentationRatio() const;
    
    // Debugging utilities
    void setMemoryDebugLevel(int level);
    void enableLeakDetection(bool enable);
    void setAutoCleanupThreshold(size_t threshold);
    
    // Export data
    void exportMemoryReport(const std::string& filename) const;
    
    // Clear tracking
    void clearHistory();
    
    // Singleton pattern
    static MemoryTracker& getInstance();
    static void setGlobalMemoryTracker(std::shared_ptr<MemoryTracker> tracker);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void* trackAllocationInternal(void* ptr, size_t size, 
                                 const std::string& function, const std::string& file,
                                 int line, const std::string& threadId);
    void trackDeallocationInternal(void* ptr, const std::string& function,
                                  const std::string& file, int line,
                                  const std::string& threadId);
};

// CPU profiling utilities
struct CPUProfileData {
    std::string functionName;
    std::string fileName;
    int lineNumber;
    double cpuTime;  // in milliseconds
    double memoryUsage;  // in bytes
    int threadId;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<CPUProfileData> children;
    double cumulativeTime() const;
    double averageTime() const;
};

class CPUProfiler {
public:
    CPUProfiler();
    ~CPUProfiler();
    
    // Start/stop profiling
    void startProfiling(const std::string& functionName, const std::string& fileName = "",
                       int lineNumber = 0, const std::unordered_map<std::string, std::string>& metadata = {});
    void stopProfiling();
    
    // Get profile data
    CPUProfileData getCurrentProfile() const;
    std::vector<CPUProfileData> getProfileHistory(size_t limit = 100) const;
    std::vector<CPUProfileData> getTopFunctions(size_t count = 10) const;
    
    // Analysis
    void analyzeBottlenecks();
    std::vector<std::string> identifySlowFunctions(double thresholdMs = 100.0) const;
    
    // Configuration
    void setSamplingInterval(double interval);  // in seconds
    void setMaxProfileDepth(int depth);
    void enableRealTimeProfiling(bool enable);
    
    // Export
    void exportProfileReport(const std::string& filename) const;
    void clearProfileHistory();
    
    // Singleton pattern
    static CPUProfiler& getInstance();
    static void setGlobalCPUProfiler(std::shared_ptr<CPUProfiler> profiler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network debugging utilities
struct NetworkConnectionInfo {
    std::string connectionId;
    std::string remoteAddress;
    uint16_t remotePort;
    std::chrono::system_clock::time_point connectionTime;
    double totalBytesSent;
    double totalBytesReceived;
    double averageLatency;
    std::string protocol;
    std::unordered_map<std::string, std::string> metadata;
};

struct NetworkPacketInfo {
    std::string packetId;
    std::chrono::system_clock::time_point timestamp;
    size_t size;
    std::string sourceAddress;
    std::string destinationAddress;
    std::string protocol;
    std::unordered_map<std::string, std::string> metadata;
};

class NetworkDebugger {
public:
    NetworkDebugger();
    ~NetworkDebugger();
    
    // Track network connections
    void logConnection(const std::string& connectionId, const std::string& remoteAddress,
                      uint16_t remotePort, const std::string& protocol,
                      const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Track network packets
    void logPacket(const std::string& packetId, const std::string& sourceAddress,
                  const std::string& destinationAddress, size_t size,
                  const std::string& protocol, const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Get network statistics
    std::vector<NetworkConnectionInfo> getActiveConnections() const;
    std::vector<NetworkPacketInfo> getRecentPackets(size_t limit = 100) const;
    double getAverageLatency(const std::string& connectionId = "") const;
    
    // Analysis
    void analyzeTrafficPatterns();
    std::vector<std::string> identifyHighTrafficEndpoints(double thresholdMB = 100.0) const;
    
    // Configuration
    void setCapturePackets(bool enable);
    void setPacketCaptureLimit(size_t limit);
    void setLatencyThreshold(double thresholdMs);
    
    // Export
    void exportNetworkReport(const std::string& filename) const;
    void clearNetworkHistory();
    
    // Singleton pattern
    static NetworkDebugger& getInstance();
    static void setGlobalNetworkDebugger(std::shared_ptr<NetworkDebugger> debugger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// GUI debugging utilities
struct GUIElementInfo {
    std::string elementId;
    std::string elementType;
    std::string parentId;
    std::chrono::system_clock::time_point creationTime;
    std::unordered_map<std::string, std::string> properties;
    std::unordered_map<std::string, std::string> metadata;
};

struct GUIInteractionInfo {
    std::string interactionId;
    std::string elementId;
    std::chrono::system_clock::time_point timestamp;
    std::string interactionType;
    std::unordered_map<std::string, std::string> properties;
    std::unordered_map<std::string, std::string> metadata;
};

class GUITracker {
public:
    GUITracker();
    ~GUITracker();
    
    // Track GUI elements
    void logElementCreation(const std::string& elementId, const std::string& elementType,
                           const std::string& parentId = "",
                           const std::unordered_map<std::string, std::string>& properties = {});
    
    // Track GUI interactions
    void logInteraction(const std::string& interactionId, const std::string& elementId,
                       const std::string& interactionType,
                       const std::unordered_map<std::string, std::string>& properties = {});
    
    // Get GUI state
    std::vector<GUIElementInfo> getActiveElements() const;
    std::vector<GUIInteractionInfo> getRecentInteractions(size_t limit = 100) const;
    GUIElementInfo getElementInfo(const std::string& elementId) const;
    
    // Analysis
    void analyzeElementUsage();
    std::vector<std::string> identifyUnusedElements(double thresholdHours = 24.0) const;
    
    // Configuration
    void setElementCapture(bool enable);
    void setInteractionCapture(bool enable);
    void setMaxElementHistory(size_t limit);
    void setMaxInteractionHistory(size_t limit);
    
    // Export
    void exportGUIState(const std::string& filename) const;
    void clearGUIHistory();
    
    // Singleton pattern
    static GUITracker& getInstance();
    static void setGlobalGUITracker(std::shared_ptr<GUITracker> tracker);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced debugging utilities
class DebugUtils {
public:
    DebugUtils();
    ~DebugUtils();
    
    // Memory debugging utilities
    template<typename T>
    T* debugAlloc(const std::string& context, size_t count = 1, 
                 const std::unordered_map<std::string, std::string>& metadata = {});
    
    template<typename T>
    void debugDealloc(T* ptr, const std::string& context);
    
    // CPU debugging utilities
    void debugProfileFunction(const std::function<void()>& func,
                             const std::string& name,
                             const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Network debugging utilities
    void debugLogNetwork(const std::string& operation, const std::string& endpoint,
                        double latency, size_t bytesSent = 0, size_t bytesReceived = 0,
                        const std::unordered_map<std::string, std::string>& metadata = {});
    
    // GUI debugging utilities
    void debugLogClick(const std::string& elementId, const std::string& elementType);
    void debugLogUpdate(const std::string& elementId, const std::string& updateType);
    
    // Simulation debugging utilities
    void debugLogSimulationStep(int step, double simulationTime,
                               int activeNeurons, int totalNeurons,
                               const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Error recovery utilities
    template<typename T>
    bool attemptRecovery(const std::function<T()>& operation,
                        const std::string& errorContext,
                        int maxRetries = 3,
                        double retryDelay = 1.0,
                        const std::unordered_map<std::string, std::string>& recoveryMetadata = {});
    
    // System state utilities
    void captureSystemState(const std::string& stateName,
                           const std::unordered_map<std::string, std::string>& metadata = {});
    std::string compareSystemStates(const std::string& state1, const std::string& state2);
    void rollbackSystemState(const std::string& targetState);
    
    // Trace utilities
    void startTrace(const std::string& traceName,
                   const std::unordered_map<std::string, std::string>& metadata = {});
    void endTrace(const std::string& traceName, bool success = true,
                 const std::string& result = "");
    std::string getCurrentTraceId();
    
    // Performance analysis utilities
    void analyzePerformance(const std::string& analysisType = "comprehensive",
                           const std::unordered_map<std::string, std::string>& parameters = {});
    double calculatePerformanceScore(const std::string& metric, double currentValue,
                                    double baselineValue = 0.0) const;
    
    // Visualization utilities
    void createMemoryVisualization(const std::string& filename = "memory_report.html");
    void createPerformanceVisualization(const std::string& filename = "performance_report.html");
    void createTraceVisualization(const std::string& filename = "trace_report.html");
    
    // Integration with StructuredLogger
    void setStructuredLogger(std::shared_ptr<StructuredLogger> logger);
    std::shared_ptr<StructuredLogger> getStructuredLogger() const;
    
    // Singleton pattern
    static DebugUtils& getInstance();
    static void setGlobalDebugUtils(std::shared_ptr<DebugUtils> debugUtils);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    std::string generateDebugId() const;
    std::chrono::system_clock::time_point getCurrentTime() const;
};

// Debug breakpoint utilities
void setDebugBreakpoint(bool condition, const std::string& message = "Breakpoint hit",
                       const std::string& correlationId = "");

void setConditionalDebugBreakpoint(const std::string& conditionExpression,
                                  const std::string& message = "Conditional breakpoint",
                                  const std::string& correlationId = "");

// Performance monitoring utilities
double getPerformanceClock();
double getSystemCPULoad();
double getSystemMemoryUsage();
double getSystemDiskUsage();

// Error context utilities
void logErrorContext(const std::string& errorType, const std::string& errorMessage,
                    const std::unordered_map<std::string, std::string>& context = {});
void addErrorRecoveryStep(const std::string& recoveryStep,
                         const std::function<bool()>& recoveryFunction,
                         const std::unordered_map<std::string, std::string>& metadata = {});

// Incident management utilities
void logSystemIncident(const std::string& incidentType, const std::string& severity,
                      const std::string& description,
                      const std::unordered_map<std::string, std::string>& details = {});
void createIncidentReport(const std::string& incidentId, const std::string& title,
                         const std::string& description,
                         const std::unordered_map<std::string, std::string>& metadata = {});

// Visualization utilities
void visualizeMemoryAllocation(const std::string& pointerId, size_t size,
                              const std::string& filename = "memory_allocation.html");
void visualizePerformanceMetrics(const std::string& metricsId,
                                 const std::string& filename = "performance_metrics.html");
void visualizeTraceData(const std::string& traceId,
                       const std::string& filename = "trace_analysis.html");

// Configuration utilities
void configureDebugOptions(const std::unordered_map<std::string, std::string>& options);
std::unordered_map<std::string, std::string> getDebugConfiguration();

// Utility functions
std::string formatMemorySize(size_t bytes);
std::string formatTime(std::chrono::system_clock::time_point time);

} // namespace nlm