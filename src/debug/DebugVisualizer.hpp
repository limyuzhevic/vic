#pragma once

#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include "DebuggingSystem.hpp"
#include "PerformanceProfiler.hpp"
#include "MemoryProfiler.hpp"
#include "EventLogger.hpp"

namespace nlm {

// DebugHook: Interface for debug event hooks
class DebugHook {
public:
    virtual ~DebugHook() = default;
    
    // Called when a debug event occurs
    virtual void onDebugEvent(const DebugEvent& event) = 0;
    
    // Called for performance metrics
    virtual void onPerformanceMetric(const std::string& name, double value) = 0;
    
    // Called for memory events
    virtual void onMemoryEvent(const std::string& operation, size_t size) = 0;
    
    // Called when debugging status changes
    virtual void onDebugStatusChange(DebugStatus oldStatus, DebugStatus newStatus) = 0;
    
    // Priority for hook execution (higher priority hooks execute first)
    virtual int getPriority() const { return 0; }
};

// DebugVisualizer: Interactive visualization of debug data
class DebugVisualizer {
public:
    DebugVisualizer();
    explicit DebugVisualizer(std::shared_ptr<DebuggingSystem> debuggingSystem);
    ~DebugVisualizer();
    
    // Configuration
    void setPort(uint16_t port) { port_ = port; }
    void setHost(const std::string& host) { host_ = host; }
    void setEnableWebInterface(bool enable) { enableWebInterface_ = enable; }
    void setUpdateInterval(double interval) { updateInterval_ = interval; }
    
    // Server control
    void startServer();
    void stopServer();
    bool isServerRunning() const { return serverRunning_; }
    
    // Visualization data
    void setDebuggingSystem(std::shared_ptr<DebuggingSystem> debuggingSystem) { debuggingSystem_ = debuggingSystem; }
    void setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler) { performanceProfiler_ = profiler; }
    void setMemoryProfiler(std::shared_ptr<MemoryProfiler> profiler) { memoryProfiler_ = profiler; }
    void setEventLogger(std::shared_ptr<EventLogger> logger) { eventLogger_ = logger; }
    
    // Web interface
    std::string generateDashboardHTML() const;
    std::string generateTraceView() const;
    std::string generatePerformanceView() const;
    std::string generateMemoryView() const;
    std::string generateEventView() const;
    
    // API endpoints
    std::string getAPIDebugInfo() const;
    std::string getAPIPerformanceData() const;
    std::string getAPIMemoryData() const;
    std::string getAPIEventsData() const;
    
    // Export
    bool exportVisualization(const std::string& directory) const;
    
    // State
    bool isWebInterfaceEnabled() const { return enableWebInterface_; }
    
private:
    std::shared_ptr<DebuggingSystem> debuggingSystem_;
    std::shared_ptr<PerformanceProfiler> performanceProfiler_;
    std::shared_ptr<MemoryProfiler> memoryProfiler_;
    std::shared_ptr<EventLogger> eventLogger_;
    
    uint16_t port_;
    std::string host_;
    bool enableWebInterface_;
    double updateInterval_;
    
    bool serverRunning_;
    std::thread serverThread_;
    mutable std::mutex serverMutex_;
    
    // Helper methods
    void monitoringLoop();
    std::string generateHTMLHeader() const;
    std::string generateHTMLFooter() const;
    std::string generateEventChart(const std::vector<SimulationEvent>& events) const;
    std::string generateMemoryChart(const std::vector<MemorySnapshot>& snapshots) const;
    std::string generatePerformanceChart(const std::vector<std::string>& operations) const;
    std::string escapeHTML(const std::string& text) const;
};

} // namespace nlm
