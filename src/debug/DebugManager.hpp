// NLM Advanced Debugging and Profiling Tools - Core Debug Manager

#pragma once

#include "core/Logger/Logger.hpp"
#include "core/Error/ErrorManager.hpp"
#include "experiments/Metrics.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>

namespace nlm {

// Forward declarations
class BreakpointDebugger;
class WatchpointDebugger;
class TraceDebugger;
class MemoryDebugger;
class ConcurrencyDebugger;
class PerformanceProfiler;
class SystemMonitor;
class PerformanceDashboard;
class BottleneckAnalyzer;
class MemoryOptimizer;
class CPUMOptimizer;
class AlgorithmAnalyzer;
class PerformanceVisualizer;
class DebugVisualizer;
class ArchitectureVisualizer;
class ReportGenerator;

// Debug configuration
struct DebugConfig {
    bool enableBreakpointDebugging = true;
    bool enableWatchpointDebugging = true;
    bool enableTraceDebugging = true;
    bool enableMemoryDebugging = true;
    bool enableConcurrencyDebugging = true;
    bool enablePerformanceProfiling = true;
    bool enableSystemMonitoring = true;
    bool enableBottleneckAnalysis = true;
    bool enableOptimizationAnalysis = true;
    bool enableVisualization = true;
    bool enableReportGeneration = true;
    
    double profilingInterval = 0.5;  // seconds
    size_t maxTraceDepth = 1000;
    size_t maxMemoryRecords = 10000;
    bool realTimeAlerts = true;
    std::string alertThresholdsFile = "alert_thresholds.json";
    bool nonIntrusiveMode = true;  // Minimal performance impact
    
    // Load from configuration
    static DebugConfig loadFromConfig(const std::string& filename = "debug_config.json");
    void saveToConfig(const std::string& filename) const;
    
    // Validate configuration
    bool validate() const;
    std::string getValidationErrors() const;
};

// Debug event types
enum class DebugEventType {
    BreakpointTriggered,
    WatchpointTriggered,
    TraceEvent,
    MemoryAllocation,
    MemoryDeallocation,
    MemoryLeakDetected,
    PerformanceMetric,
    SystemAlert,
    OptimizationRecommendation,
    ExportComplete,
    Error
};

// Debug event structure
struct DebugEvent {
    DebugEventType type;
    std::chrono::system_clock::time_point timestamp;
    std::string component;
    std::string source;
    std::string message;
    std::map<std::string, std::string> data;
    std::string errorId;
    
    DebugEvent(DebugEventType t, const std::string& comp, const std::string& msg)
        : type(t), component(comp), source("system"), message(msg), timestamp(std::chrono::system_clock::now()) {}
    
    DebugEvent(DebugEventType t, const std::string& comp, const std::string& src, const std::string& msg)
        : type(t), component(comp), source(src), message(msg), timestamp(std::chrono::system_clock::now()) {}
};

// Debug statistics
struct DebugStatistics {
    size_t breakpointsTriggered = 0;
    size_t watchpointsTriggered = 0;
    size_t traceEventsRecorded = 0;
    size_t memoryAllocations = 0;
    size_t memoryDeallocations = 0;
    size_t memoryLeaksDetected = 0;
    double totalCPUTime = 0.0;
    size_t peakMemoryUsage = 0;
    size_t totalMemoryAllocated = 0;
    size_t systemAlerts = 0;
    size_t optimizationRecommendations = 0;
    double averagePerformanceImpact = 0.0;
    
    std::string getStatisticsSummary() const;
    std::string getPerformanceReport() const;
    void reset();
};

// Main debug manager class
class DebugManager {
public:
    DebugManager(const DebugConfig& config = DebugConfig());
    ~DebugManager();
    
    // Initialization and configuration
    void initialize(const DebugConfig& config);
    void updateConfig(const DebugConfig& config);
    const DebugConfig& getConfig() const { return config_; }
    
    // Event handling
    void logEvent(const DebugEvent& event);
    void registerEventHandler(DebugEventType type, std::function<void(const DebugEvent&)> handler);
    
    // Debugging tools access
    BreakpointDebugger& getBreakpointDebugger() { return *breakpointDebugger_; }
    WatchpointDebugger& getWatchpointDebugger() { return *watchpointDebugger_; }
    TraceDebugger& getTraceDebugger() { return *traceDebugger_; }
    MemoryDebugger& getMemoryDebugger() { return *memoryDebugger_; }
    ConcurrencyDebugger& getConcurrencyDebugger() { return *concurrencyDebugger_; }
    
    // Profiling tools access
    PerformanceProfiler& getPerformanceProfiler() { return *performanceProfiler_; }
    SystemMonitor& getSystemMonitor() { return *systemMonitor_; }
    PerformanceDashboard& getPerformanceDashboard() { return *performanceDashboard_; }
    
    // Optimization tools access
    BottleneckAnalyzer& getBottleneckAnalyzer() { return *bottleneckAnalyzer_; }
    MemoryOptimizer& getMemoryOptimizer() { return *memoryOptimizer_; }
    CPUMOptimizer& getCPUMOptimizer() { return *cpuOptimizer_; }
    AlgorithmAnalyzer& getAlgorithmAnalyzer() { return *algorithmAnalyzer_; }
    
    // Visualization and reporting
    PerformanceVisualizer& getPerformanceVisualizer() { return *performanceVisualizer_; }
    DebugVisualizer& getDebugVisualizer() { return *debugVisualizer_; }
    ArchitectureVisualizer& getArchitectureVisualizer() { return *architectureVisualizer_; }
    ReportGenerator& getReportGenerator() { return *reportGenerator_; }
    
    // Global instance management
    static DebugManager& getInstance(const DebugConfig& config = DebugConfig());
    static void shutdown();
    
    // Main control methods
    void startAllDebuggers();
    void stopAllDebuggers();
    void pauseAllDebuggers();
    void resumeAllDebuggers();
    bool isRunning() const { return running_; }
    
    // Profiling control
    void startProfiling(const std::string& operation);
    void stopProfiling(const std::string& operation);
    void startSystemMonitoring();
    void stopSystemMonitoring();
    
    // Debug session management
    std::string createDebugSession(const std::string& name);
    void endDebugSession(const std::string& sessionId);
    void snapshotDebugState(const std::string& sessionId);
    std::string getDebugSessionReport(const std::string& sessionId, const std::string& format = "text");
    
    // Export functionality
    bool exportDebugData(const std::string& filename, const std::string& format = "json");
    bool exportPerformanceData(const std::string& filename, const std::string& format = "csv");
    bool exportDebugEvents(const std::string& filename, const std::string& format = "xml");
    
    // Statistics and reporting
    DebugStatistics getStatistics() const { return statistics_; }
    std::string generateComprehensiveReport() const;
    std::string generatePerformanceReport() const;
    std::string generateDebugReport() const;
    
    // Configuration management
    bool loadDebugConfig(const std::string& filename);
    bool saveDebugConfig(const std::string& filename) const;
    bool validateDebugConfig() const;
    
    // Convenience methods
    void attachToBrain(const std::string& brainId);
    void attachToExperiment(const std::string& experimentName);
    void attachToSimulation(const std::string& simulationId);
    
    // Status and monitoring
    std::string getStatus() const;
    bool isHealthy() const;
    std::vector<std::string> getActiveComponents() const;
    
    // Event processing
    void processPendingEvents();
    void flushEvents();
    
private:
    DebugConfig config_;
    DebugStatistics statistics_;
    
    // Debuggers
    std::unique_ptr<BreakpointDebugger> breakpointDebugger_;
    std::unique_ptr<WatchpointDebugger> watchpointDebugger_;
    std::unique_ptr<TraceDebugger> traceDebugger_;
    std::unique_ptr<MemoryDebugger> memoryDebugger_;
    std::unique_ptr<ConcurrencyDebugger> concurrencyDebugger_;
    
    // Profiling
    std::unique_ptr<PerformanceProfiler> performanceProfiler_;
    std::unique_ptr<SystemMonitor> systemMonitor_;
    std::unique_ptr<PerformanceDashboard> performanceDashboard_;
    
    // Optimization
    std::unique_ptr<BottleneckAnalyzer> bottleneckAnalyzer_;
    std::unique_ptr<MemoryOptimizer> memoryOptimizer_;
    std::unique_ptr<CPUMOptimizer> cpuOptimizer_;
    std::unique_ptr<AlgorithmAnalyzer> algorithmAnalyzer_;
    
    // Visualization and reporting
    std::unique_ptr<PerformanceVisualizer> performanceVisualizer_;
    std::unique_ptr<DebugVisualizer> debugVisualizer_;
    std::unique_ptr<ArchitectureVisualizer> architectureVisualizer_;
    std::unique_ptr<ReportGenerator> reportGenerator_;
    
    // Event handling
    std::vector<DebugEvent> eventQueue_;
    std::map<DebugEventType, std::vector<std::function<void(const DebugEvent&)>>> eventHandlers_;
    mutable std::mutex eventMutex_;
    
    // Threading
    std::atomic<bool> running_;
    std::thread eventProcessingThread_;
    std::thread monitoringThread_;
    std::thread profilingThread_;
    
    // Sessions
    std::map<std::string, std::chrono::system_clock::time_point> activeSessions_;
    
    // Integration
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<AdvancedLogger> advancedLogger_;
    std::shared_ptr<Metrics> metrics_;
    
    // Internal methods
    void initializeDebuggers();
    void initializeProfiling();
    void initializeMonitoring();
    void initializeOptimization();
    void initializeVisualization();
    void startEventProcessingThread();
    void startMonitoringThread();
    void startProfilingThread();
    void eventProcessingWorker();
    void monitoringWorker();
    void profilingWorker();
    void updateStatistics();
    void handleEvent(const DebugEvent& event);
    
    // Singleton instance
    static std::unique_ptr<DebugManager> instance_;
    static std::mutex instanceMutex_;
};

} // namespace nlm
