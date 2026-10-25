// NLM Advanced Features - Enhanced capabilities for power users

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <chrono>
#include <atomic>
#include "brain/Brain.hpp"

using namespace nlm;

// Advanced Feature Framework - Core structures

struct AdvancedFeature {
    std::string name;
    std::string description;
    bool enabled;
    double performanceImpact;
    std::vector<std::string> dependencies;
    std::function<void(std::shared_ptr<Brain>, const AdvancedFeatureConfig&)> execute;
    std::function<bool(std::shared_ptr<Brain>, const AdvancedFeatureConfig&)> validate;
};

struct AdvancedFeatureConfig {
    std::string name;
    bool enabled;
    double parameters[10];  // Generic parameter storage
    std::map<std::string, std::string> stringParams;
    int intParams[5];
    double doubleParams[5];
    bool boolParams[5];
};

// Feature categories for organization
enum class FeatureCategory {
    PERFORMANCE,
    ACCURACY,
    EXPLORATION,
    COGNITION,
    VISUALIZATION,
    EXPORT,
    ANALYSIS,
    CUSTOM
};

// Feature event system for advanced automation
struct FeatureEvent {
    enum class EventType {
        FeatureEnabled,
        FeatureDisabled,
        FeatureExecuted,
        FeatureError,
        FeatureOptimized,
        FeaturePerformanceChanged,
        FeatureDependencyChanged
    };

    EventType type;
    std::string featureName;
    std::string message;
    std::map<std::string, std::string> metadata;
    std::chrono::system_clock::time_point timestamp;

    FeatureEvent(EventType t, const std::string& name, const std::string& msg)
        : type(t), featureName(name), message(msg),
          timestamp(std::chrono::system_clock::now()) {}
};

class FeatureEventListener {
public:
    virtual ~FeatureEventListener() = default;
    virtual void onFeatureEvent(const FeatureEvent& event) = 0;
    virtual bool shouldHandleEvent(const FeatureEvent& event) const = 0;
};

// Main advanced feature manager
class AdvancedFeatureManager {
public:
    AdvancedFeatureManager(std::shared_ptr<Brain> brain);
    ~AdvancedFeatureManager();

    // Feature lifecycle management
    bool addFeature(const AdvancedFeature& feature);
    bool removeFeature(const std::string& name);
    bool enableFeature(const std::string& name);
    bool disableFeature(const std::string& name);
    bool toggleFeature(const std::string& name);
    bool isFeatureEnabled(const std::string& name) const;

    // Feature execution
    bool executeAllFeatures(const AdvancedFeatureConfig& config);
    bool executeFeature(const std::string& name, const AdvancedFeatureConfig& config);
    void executeAllEnabledFeatures(const AdvancedFeatureConfig& config);

    // Feature discovery and information
    std::vector<std::string> getEnabledFeatureNames() const;
    std::vector<std::string> getAllFeatureNames() const;
    AdvancedFeature getFeature(const std::string& name) const;
    std::vector<AdvancedFeature> getFeaturesByCategory(FeatureCategory category) const;

    // Performance monitoring and optimization
    PerformanceMetrics getPerformanceMetrics() const;
    void updatePerformanceMetrics();
    double calculateFeaturePerformanceImpact(const std::string& name) const;

    // Configuration management
    void setGlobalConfig(const AdvancedFeatureConfig& config);
    AdvancedFeatureConfig getGlobalConfig() const;
    void setFeatureConfig(const std::string& name, const AdvancedFeatureConfig& config);
    AdvancedFeatureConfig getFeatureConfig(const std::string& name) const;

    // Export/import capabilities
    bool exportFeatures(const std::string& filepath) const;
    bool importFeatures(const std::string& filepath);

    // Analysis and reporting
    FeatureAnalysis analyzeFeatureUsage(const std::string& name) const;
    FeatureUsageReport generateUsageReport() const;
    std::vector<std::string> getOptimizationSuggestions() const;

    // Advanced API interface (delegation)
    class AdvancedAPI* getAdvancedAPI() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void initializeBuiltInFeatures();
    void buildFeatureDependencies();
    void optimizeFeatureOrder();
    void validateFeatureDependencies();
    void setupEventHandling();
};

// Feature analysis and reporting structures
struct FeatureAnalysis {
    std::string featureName;
    double usageFrequency;
    double performanceScore;
    double resourceUsage;
    double efficiencyScore;
    std::vector<std::string> dependencyIssues;
    std::vector<std::string> performanceIssues;
    std::chrono::system_clock::time_point lastUsed;
    std::chrono::system_clock::time_point lastOptimized;
};

struct FeatureUsageReport {
    size_t totalFeatures;
    size_t enabledFeatures;
    size_t disabledFeatures;
    double averagePerformance;
    double totalResourceUsage;
    std::map<std::string, FeatureAnalysis> featureAnalysis;
    std::vector<std::string> recommendations;
    std::chrono::system_clock::time_point reportTime;
};

// Feature event dispatcher
class FeatureEventDispatcher {
public:
    static FeatureEventDispatcher& getInstance();
    void addFeatureListener(std::shared_ptr<FeatureEventListener> listener);
    void removeFeatureListener(std::shared_ptr<FeatureEventListener> listener);
    void dispatch(const FeatureEvent& event);
    void setFeatureEventCallback(std::function<void(const FeatureEvent&)> callback);

private:
    FeatureEventDispatcher() = default;
    std::vector<std::shared_ptr<FeatureEventListener>> listeners;
    std::function<void(const FeatureEvent&)> eventCallback;
    std::mutex mutex;
};

// Performance metrics aggregation
struct PerformanceMetrics {
    double cpuUsage;
    double memoryUsage;
    double simulationSpeed;
    double realTimePerformance;
    std::chrono::system_clock::time_point lastPerformanceUpdate;
    std::vector<double> performanceHistory;
    std::map<std::string, double> featurePerformance;
    std::map<std::string, double> systemResourceUsage;
};

// Utility functions for advanced features
namespace FeatureUtils {
    double calculatePerformanceScore(const AdvancedFeature& feature);
    double calculateResourceEfficiency(const AdvancedFeature& feature);
    std::vector<std::string> analyzeFeatureDependencies(const AdvancedFeature& feature);
    std::vector<std::string> findCircularDependencies(const std::vector<AdvancedFeature>& features);
    std::string featureConfigToString(const AdvancedFeatureConfig& config);
    AdvancedFeatureConfig stringToFeatureConfig(const std::string& configStr);
    bool validateFeatureDependencies(const AdvancedFeature& feature, const std::vector<AdvancedFeature>& allFeatures);
    std::string generateFeatureUsageReport(const FeatureUsageReport& report);
    std::vector<std::string> generateOptimizationRecommendations(const std::vector<AdvancedFeature>& features);
}

// Advanced API interface for external control
class AdvancedAPI {
public:
    AdvancedAPI(std::shared_ptr<Brain> brain, std::shared_ptr<AdvancedFeatureManager> featureManager);
    ~AdvancedAPI();

    // Brain control methods
    void step(SimulationStep step, Timestamp time);
    void reset();
    bool save(const std::string& filepath);
    bool load(const std::string& filepath);

    // Feature control methods
    bool enableFeature(const std::string& name);
    bool disableFeature(const std::string& name);
    bool toggleFeature(const std::string& name);
    bool isFeatureEnabled(const std::string& name);

    // Parameter control methods
    void setFeatureParameter(const std::string& featureName, const std::string& paramName, double value);
    double getFeatureParameter(const std::string& featureName, const std::string& paramName) const;
    void setGlobalParameter(const std::string& paramName, double value);
    double getGlobalParameter(const std::string& paramName) const;

    // Data access methods
    std::string getBrainState() const;
    std::string getFeatureState(const std::string& name) const;
    std::string getPerformanceMetrics() const;
    std::string getIntegrationStatus() const;

    // Export/import methods
    bool exportState(const std::string& filepath) const;
    bool importState(const std::string& filepath);
    bool exportFeatures(const std::string& filepath) const;
    bool importFeatures(const std::string& filepath);

    // Scripting and automation methods
    std::string executeScript(const std::string& script);
    bool loadScript(const std::string& name, const std::string& script);
    bool removeScript(const std::string& name);

    // Analysis methods
    std::string analyzeBrainState() const;
    std::string analyzeFeaturePerformance(const std::string& name) const;
    std::string generateOptimizationReport() const;

    // Query methods
    std::vector<std::string> listFeatures() const;
    std::vector<std::string> listScripts() const;
    std::map<std::string, std::string> getFeatureInfo(const std::string& name) const;
    std::map<std::string, std::string> getSystemInfo() const;

    // Event methods
    void addEventListener(std::shared_ptr<FeatureEventListener> listener);
    void removeEventListener(std::shared_ptr<FeatureEventListener> listener);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced feature builder for custom features
class AdvancedFeatureBuilder {
public:
    AdvancedFeatureBuilder();
    ~AdvancedFeatureBuilder();

    AdvancedFeatureBuilder& name(const std::string& name);
    AdvancedFeatureBuilder& description(const std::string& description);
    AdvancedFeatureBuilder& category(FeatureCategory category);
    AdvancedFeatureBuilder& enabled(bool enabled);
    AdvancedFeatureBuilder& performanceImpact(double impact);
    AdvancedFeatureBuilder& addDependency(const std::string& dependency);
    AdvancedFeatureBuilder& addParameter(const std::string& name, double value);
    AdvancedFeatureBuilder& addStringParameter(const std::string& name, const std::string& value);
    AdvancedFeatureBuilder& addIntParameter(const std::string& name, int value);
    AdvancedFeatureBuilder& addBoolParameter(const std::string& name, bool value);
    AdvancedFeatureBuilder& setExecuteFunction(std::function<void(std::shared_ptr<Brain>, const AdvancedFeatureConfig&)> func);
    AdvancedFeatureBuilder& setValidateFunction(std::function<bool(std::shared_ptr<Brain>, const AdvancedFeatureConfig&)> func);
    AdvancedFeature build();

    // Predefined feature builders
    static AdvancedFeatureBuilder createMemoryOptimizationFeature();
    static AdvancedFeatureBuilder createPredictionEnhancementFeature();
    static AdvancedFeatureBuilder createNeuromodulationOptimizationFeature();
    static AdvancedFeatureBuilder createCognitiveEnhancementFeature();
    static AdvancedFeatureBuilder createVisualizationFeature();
    static AdvancedFeatureBuilder createExportFeature();
    static AdvancedFeatureBuilder createPerformanceMonitoringFeature();
    static AdvancedFeatureBuilder createScriptingFeature();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Feature statistics and analytics
struct FeatureStatistics {
    std::chrono::system_clock::time_point firstUsed;
    std::chrono::system_clock::time_point lastUsed;
    size_t totalExecutions;
    double averageExecutionTime;
    double totalPerformanceImpact;
    std::vector<double> executionHistory;
    std::vector<double> performanceHistory;
    std::map<std::string, size_t> dependencyUsageCount;
};

struct SystemResourceUsage {
    double cpuUsage;
    double memoryUsage;
    double networkUsage;
    double diskUsage;
    std::chrono::system_clock::time_point lastUpdate;
    std::map<std::string, double> featureResourceUsage;
};

struct ScalabilityMetrics {
    double throughput;
    double latency;
    double efficiency;
    size_t concurrentOperations;
    std::vector<double> scalabilityHistory;
    std::chrono::system_clock::time_point lastScalabilityUpdate;
};

// Event-driven feature monitoring
class FeatureMonitor {
public:
    FeatureMonitor(std::shared_ptr<AdvancedFeatureManager> featureManager);
    ~FeatureMonitor();

    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;

    void addHealthCheck(const std::string& featureName, std::function<bool()> healthCheck);
    void removeHealthCheck(const std::string& featureName);
    bool runHealthChecks();

    std::vector<std::string> getUnhealthyFeatures() const;
    std::map<std::string, std::string> getFeatureHealthStatus() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration management for advanced features
class FeatureConfigurationManager {
public:
    FeatureConfigurationManager(std::shared_ptr<AdvancedFeatureManager> featureManager);
    ~FeatureConfigurationManager();

    bool loadConfiguration(const std::string& filepath);
    bool saveConfiguration(const std::string& filepath) const;
    bool importConfiguration(const std::string& configData);
    std::string exportConfiguration() const;

    void setGlobalSetting(const std::string& key, const std::string& value);
    std::string getGlobalSetting(const std::string& key) const;
    void setFeatureSetting(const std::string& featureName, const std::string& key, const std::string& value);
    std::string getFeatureSetting(const std::string& featureName, const std::string& key) const;

    std::map<std::string, std::string> getAllGlobalSettings() const;
    std::map<std::string, std::map<std::string, std::string>> getAllFeatureSettings() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced feature logging and debugging
class FeatureLogger {
public:
    FeatureLogger(std::shared_ptr<AdvancedFeatureManager> featureManager);
    ~FeatureLogger();

    void enableLogging(bool enable);
    bool isLoggingEnabled() const;

    void logFeatureExecution(const std::string& featureName, const std::string& message, bool success);
    void logFeatureError(const std::string& featureName, const std::string& error);
    void logPerformanceMetrics(const std::string& featureName, double performance);

    std::string getFeatureLog(const std::string& featureName, size_t limit = 100) const;
    std::string getSystemLog(size_t limit = 1000) const;

    void exportLogs(const std::string& filepath) const;
    void clearLogs();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main advanced features header with all declarations
// Implementation details are in AdvancedFeatures.cpp
