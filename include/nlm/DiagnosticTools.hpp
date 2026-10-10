#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <vector>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <set>
#include "NLMExceptions.hpp"
#include "DebugInfrastructure.hpp"

namespace nlm {

// System information structure
struct SystemInfo {
    std::string osName;
    std::string cpuArchitecture;
    int cpuCoreCount;
    double cpuUsage;           // percentage (0-100)
    size_t totalMemory;        // bytes
    size_t availableMemory;    // bytes
    double memoryUsage;        // percentage (0-100)
    std::string uptime;        // string representation
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> additionalInfo;
};

// Component state information
struct ComponentInfo {
    std::string name;
    std::string version;
    std::string status;        // "running", "stopped", "error", "unknown"
    std::chrono::system_clock::time_point lastUpdate;
    std::map<std::string, std::string> properties;
    std::vector<std::string> dependencies;
    bool isHealthy;
    std::string healthMessage;
};

// Configuration diagnostics result
struct ConfigurationInfo {
    std::string source;        // file path, environment variable, etc.
    std::map<std::string, std::string> values;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::chrono::system_clock::time_point timestamp;
};

// Dependency diagnostic result
struct DependencyInfo {
    std::string name;
    std::string version;
    std::string path;
    bool found;                // whether the dependency exists
    bool isLoaded;             // whether the dependency is loaded
    std::vector<std::string> errors;
    std::chrono::system_clock::time_point discoveryTime;
};

// Error diagnostic result
struct ErrorInfo {
    std::string id;
    std::string type;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> context;
    bool isResolved;
    std::string resolutionMethod;
};

// Performance diagnostic result
struct PerformanceInfo {
    std::string operationName;
    double duration;           // in seconds
    size_t memoryUsed;         // bytes
    double cpuUsage;           // percentage (0-100)
    std::string threadId;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> metrics;
    std::string status;        // "normal", "slow", "critical"
};

// SystemDiagnoser - System-wide diagnostics
class SystemDiagnoser {
public:
    SystemDiagnoser();
    ~SystemDiagnoser();
    
    // Get system information
    SystemInfo getSystemInfo() const;
    
    // Check system health
    bool checkSystemHealth() const;
    std::vector<std::string> getSystemHealthIssues() const;
    
    // Check resource usage
    double getCPUUsage() const;
    double getMemoryUsage() const;
    
    // Get system statistics
    std::map<std::string, std::string> getSystemStats() const;
    
    // Run comprehensive system diagnostics
    bool runDiagnostics();
    std::vector<std::string> getDiagnosticResults() const;
    
    // Enable/disable diagnostics logging
    void enableLogging(bool enabled = true);
    bool isLoggingEnabled() const;
    
    // Get global system diagnoser instance
    static SystemDiagnoser& getGlobal();
    
    // Set global system diagnoser
    static void setGlobal(std::shared_ptr<SystemDiagnoser> diagnoser);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ComponentDiagnoser - Component-specific diagnostics
class ComponentDiagnoser {
public:
    ComponentDiagnoser();
    ~ComponentDiagnoser();
    
    // Register component for diagnostics
    void registerComponent(const std::string& name, const std::string& version,
                           const std::vector<std::string>& dependencies);
    
    // Unregister component
    void unregisterComponent(const std::string& name);
    
    // Get component info
    std::optional<ComponentInfo> getComponentInfo(const std::string& name) const;
    std::map<std::string, ComponentInfo> getAllComponentInfo() const;
    
    // Check component health
    bool checkComponentHealth(const std::string& name) const;
    std::vector<std::string> getComponentHealthIssues(const std::string& name) const;
    
    // Run component diagnostics
    bool runComponentDiagnostics(const std::string& name);
    std::vector<std::string> getComponentDiagnosticResults(const std::string& name) const;
    
    // Update component state
    void updateComponentState(const std::string& name, const std::string& status,
                             const std::map<std::string, std::string>& properties);
    
    // Get global component diagnoser instance
    static ComponentDiagnoser& getGlobal();
    
    // Set global component diagnoser
    static void setGlobal(std::shared_ptr<ComponentDiagnoser> diagnoser);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ConfigurationDiagnoser - Configuration validation and diagnostics
class ConfigurationDiagnoser {
public:
    ConfigurationDiagnoser();
    ~ConfigurationDiagnoser();
    
    // Add configuration source
    void addConfigurationSource(const std::string& source,
                               const std::map<std::string, std::string>& config);
    
    // Remove configuration source
    void removeConfigurationSource(const std::string& source);
    
    // Get configuration info
    std::optional<ConfigurationInfo> getConfigurationInfo(const std::string& source) const;
    std::map<std::string, ConfigurationInfo> getAllConfigurationInfo() const;
    
    // Validate configuration
    bool validateConfiguration(const std::string& source) const;
    std::vector<std::string> getConfigurationErrors(const std::string& source) const;
    std::vector<std::string> getConfigurationWarnings(const std::string& source) const;
    
    // Merge configurations
    void mergeConfigurations(const std::map<std::string, std::string>& config);
    
    // Get global configuration diagnoser instance
    static ConfigurationDiagnoser& getGlobal();
    
    // Set global configuration diagnoser
    static void setGlobal(std::shared_ptr<ConfigurationDiagnoser> diagnoser);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// DependencyDiagnoser - Dependency analysis and diagnostics
class DependencyDiagnoser {
public:
    DependencyDiagnoser();
    ~DependencyDiagnoser();
    
    // Add dependency to scan
    void addDependency(const std::string& name, const std::string& version = "",
                      const std::string& path = "");
    
    // Remove dependency
    void removeDependency(const std::string& name);
    
    // Get dependency info
    std::optional<DependencyInfo> getDependencyInfo(const std::string& name) const;
    std::map<std::string, DependencyInfo> getAllDependencyInfo() const;
    
    // Check dependency health
    bool checkDependencyHealth(const std::string& name) const;
    std::vector<std::string> getDependencyHealthIssues(const std::string& name) const;
    
    // Run dependency diagnostics
    bool runDependencyDiagnostics(const std::string& name);
    std::vector<std::string> getDependencyDiagnosticResults(const std::string& name) const;
    
    // Scan for all dependencies
    void scanDependencies();
    std::vector<std::string> getScanResults() const;
    
    // Get global dependency diagnoser instance
    static DependencyDiagnoser& getGlobal();
    
    // Set global dependency diagnoser
    static void setGlobal(std::shared_ptr<DependencyDiagnoser> diagnoser);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// ErrorDiagnoser - Error analysis and pattern detection
class ErrorDiagnoser {
public:
    ErrorDiagnoser();
    ~ErrorDiagnoser();
    
    // Log error for analysis
    void logError(const std::string& type, const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "",
                 const std::map<std::string, std::string>& context = {});
    
    // Get error info
    std::optional<ErrorInfo> getErrorInfo(const std::string& id) const;
    std::vector<ErrorInfo> getAllErrors() const;
    std::vector<ErrorInfo> getErrorsByType(const std::string& type) const;
    
    // Check for error patterns
    bool detectErrorPattern(const std::string& pattern) const;
    std::vector<std::string> getDetectedPatterns() const;
    
    // Analyze error trends
    std::map<std::string, int> getErrorCountByType(size_t timeWindowSeconds = 3600) const;
    std::map<std::string, double> getErrorRateByType(size_t timeWindowSeconds = 3600) const;
    
    // Resolve error
    bool resolveError(const std::string& id, const std::string& method);
    
    // Clear error log
    void clearErrors();
    void clearErrorsByType(const std::string& type);
    
    // Get global error diagnoser instance
    static ErrorDiagnoser& getGlobal();
    
    // Set global error diagnoser
    static void setGlobal(std::shared_ptr<ErrorDiagnoser> diagnoser);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PerformanceDiagnoser - Performance analysis and bottleneck detection
class PerformanceDiagnoser {
public:
    PerformanceDiagnoser();
    ~PerformanceDiagnoser();
    
    // Log performance metric
    void logPerformance(const PerformanceInfo& perfInfo);
    
    // Get performance info
    std::optional<PerformanceInfo> getPerformanceInfo(const std::string& operationName) const;
    std::vector<PerformanceInfo> getAllPerformanceInfo() const;
    std::vector<PerformanceInfo> getRecentPerformanceInfo(size_t maxEntries = 100) const;
    
    // Analyze performance trends
    std::map<std::string, double> getAveragePerformanceByOperation(size_t timeWindowSeconds = 3600) const;
    std::map<std::string, double> getPerformanceTrend(const std::string& operationName, size_t dataPoints = 10) const;
    
    // Detect performance issues
    bool detectPerformanceBottleneck(const std::string& operationName,
                                     double threshold = 1.0) const;
    std::vector<std::string> getPerformanceIssues() const;
    
    // Set performance thresholds
    void setPerformanceThreshold(const std::string& operationName, double threshold);
    double getPerformanceThreshold(const std::string& operationName) const;
    
    // Clear performance data
    void clearPerformanceData();
    
    // Get global performance diagnoser instance
    static PerformanceDiagnoser& getGlobal();
    
    // Set global performance diagnoser
    static void setGlobal(std::shared_ptr<PerformanceDiagnoser> diagnoser);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
