#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <chrono>
#include <atomic>
#include "../core/Config/Config.hpp"

namespace nlm {

// Debug mode and configuration
enum class DebugMode {
    None,
    Release,
    Debug,
    Profile,
    Trace
};

// Alert severity levels
enum class AlertSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Breakpoint types
enum class BreakpointType {
    Instruction,
    DataAccess,
    MemoryAllocation,
    Exception,
    Custom
};

// Performance metrics
struct PerformanceMetrics {
    std::chrono::steady_clock::time_point timestamp;
    double cpuUsage;
    double memoryUsage;
    uint64_t allocatedMemory;
    uint64_t deallocatedMemory;
    size_t activeThreads;
    double simulationTime;
    size_t totalSpikeCount;
    float averageFiringRate;
    std::map<std::string, double> customMetrics;
};

// Alert information
struct Alert {
    std::chrono::steady_clock::time_point timestamp;
    std::string id;
    AlertSeverity severity;
    std::string message;
    std::string source;
    std::map<std::string, std::string> metadata;
    bool isActive;
};

// Breakpoint information
struct Breakpoint {
    std::string id;
    BreakpointType type;
    std::string location; // file:line or other identifier
    bool isEnabled;
    bool hitCount;
    int hitThreshold;
    std::chrono::steady_clock::time_point lastHitTime;
    std::function<bool()> condition;
};

// Debug session information
struct DebugSession {
    std::string sessionId;
    DebugMode mode;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    bool isPaused;
    bool isRecording;
    std::vector<std::string> watchedVariables;
    std::vector<Breakpoint> breakpoints;
    std::vector<Alert> activeAlerts;
    PerformanceMetrics currentMetrics;
    int stepCount;
};

// Debug configuration
class DebugConfig {
public:
    DebugConfig();
    explicit DebugConfig(std::shared_ptr<Config> globalConfig);
    ~DebugConfig();
    
    // Disable copying, enable moving
    DebugConfig(const DebugConfig&) = delete;
    DebugConfig& operator=(const DebugConfig&) = delete;
    DebugConfig(DebugConfig&&) noexcept;
    DebugConfig& operator=(DebugConfig&&) noexcept;
    
    // Configuration loading
    bool loadFromFile(const std::string& filepath);
    bool loadFromGlobalConfig();
    
    // Global debug mode
    DebugMode getDebugMode() const;
    void setDebugMode(DebugMode mode);
    
    // Performance monitoring
    bool isPerformanceMonitoringEnabled() const;
    void setPerformanceMonitoringEnabled(bool enabled);
    
    // Memory monitoring
    bool isMemoryMonitoringEnabled() const;
    void setMemoryMonitoringEnabled(bool enabled);
    size_t getMemoryMonitoringThreshold() const;
    void setMemoryMonitoringThreshold(size_t threshold);
    
    // Alert system configuration
    bool isAlertSystemEnabled() const;
    void setAlertSystemEnabled(bool enabled);
    void setAlertThreshold(const std::string& metric, double threshold);
    double getAlertThreshold(const std::string& metric) const;
    
    // Visualization configuration
    bool isVisualizationEnabled() const;
    void setVisualizationEnabled(bool enabled);
    std::string getVisualizationOutputFormat() const;
    void setVisualizationOutputFormat(const std::string& format);
    
    // Debug session configuration
    bool isSessionRecordingEnabled() const;
    void setSessionRecordingEnabled(bool enabled);
    std::string getSessionDirectory() const;
    void setSessionDirectory(const std::string& directory);
    
    // Breakpoint configuration
    bool areBreakpointsEnabled() const;
    void setBreakpointsEnabled(bool enabled);
    
    // Neural activity monitoring
    bool isNeuralActivityMonitoringEnabled() const;
    void setNeuralActivityMonitoringEnabled(bool enabled);
    double getNeuralActivityThreshold() const;
    void setNeuralActivityThreshold(double threshold);
    
    // Profiler configuration
    bool isProfilingEnabled() const;
    void setProfilingEnabled(bool enabled);
    std::vector<std::string> getProfilerMetrics() const;
    void setProfilerMetrics(const std::vector<std::string>& metrics);
    
    // Real-time monitoring
    bool isRealTimeMonitoringEnabled() const;
    void setRealTimeMonitoringEnabled(bool enabled);
    int getRealTimeUpdateInterval() const;
    void setRealTimeUpdateInterval(int interval);
    
    // Event-driven monitoring
    bool isEventDrivenMonitoringEnabled() const;
    void setEventDrivenMonitoringEnabled(bool enabled);
    
    // Debug mode distinctions
    bool isContextAwareDebuggingEnabled() const;
    void setContextAwareDebuggingEnabled(bool enabled);
    
    // Error handling configuration
    bool isErrorRecoveryEnabled() const;
    void setErrorRecoveryEnabled(bool enabled);
    int getMaxErrorRecoveryAttempts() const;
    void setMaxErrorRecoveryAttempts(int attempts);
    
    // Performance profiling configuration
    bool isSimulationPerformanceProfilingEnabled() const;
    void setSimulationPerformanceProfilingEnabled(bool enabled);
    bool isMemoryProfilingEnabled() const;
    void setMemoryProfilingEnabled(bool enabled);
    bool isNeuralEventTimingAnalysisEnabled() const;
    void setNeuralEventTimingAnalysisEnabled(bool enabled);
    bool isBottleneckDetectionEnabled() const;
    void setBottleneckDetectionEnabled(bool enabled);
    
    // Debug integration
    bool isLoggingIntegrationEnabled() const;
    void setLoggingIntegrationEnabled(bool enabled);
    bool isIntegrationWithExistingSystemsEnabled() const;
    void setIntegrationWithExistingSystemsEnabled(bool enabled);
    
    // Debug session management
    bool isDebugSessionManagementEnabled() const;
    void setDebugSessionManagementEnabled(bool enabled);
    std::string getSessionId() const;
    void setSessionId(const std::string& sessionId);
    
    // Debug configuration summary
    std::string summary() const;
    
    // Get global config
    std::shared_ptr<Config> getGlobalConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
