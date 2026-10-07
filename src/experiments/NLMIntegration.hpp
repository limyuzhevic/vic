// NLMIntegration.hpp - Main integration module for NLM performance profiling
// Integrates all profiling components into the NLM architecture

#pragma once

/**
 * NLMIntegration - Main integration module for NLM performance profiling
 * 
 * This module serves as the main integration point for all performance profiling
 * components in the NLM project. It provides:
 * 
 * 1. Core Integration
 *    - Integration with existing NLM modules
 *    - Configuration management
 *    - Initialization and shutdown
 *    - Performance profiling lifecycle management
 * 
 * 2. Component Integration
 *    - Integration with PerformanceProfiler
 *    - Integration with PerformanceMonitor
 *    - Integration with ProfilingDataCollector
 *    - Integration with AnalysisReporter
 *    - Integration with ExportIntegration
 *    - Integration with AdvancedProfiler
 * 
 * 3. Configuration Management
 *    - Global configuration management
 *    - Component-specific configurations
 *    - Dynamic configuration updates
 *    - Configuration validation
 * 
 * 4. Workflow Management
 *    - Profiling workflow orchestration
 *    - Data flow management
 *    - Error handling and recovery
 *    - Resource management
 * 
 * 5. Reporting and Monitoring
 *    - System status monitoring
 *    - Performance reporting
 *    - Integration status reporting
 *    - Health checks
 *    
 * Features:
 * - Modular integration architecture
 * - Non-intrusive operation
 * - Comprehensive configuration management
 * - Error handling and recovery
 * - Performance optimization
 * - Scalability
 * - Real-time monitoring capabilities
 */

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <future>
#include <map>
#include <variant>
#include <optional>
#include <numeric>
#include <cmath>
#include <type_traits>
#include <set>
#include <queue>
#include <list>
#include <regex>
#include <numeric>
#include <filesystem>

namespace nlm {

/**
 * Integration configuration
 */
struct IntegrationConfig {
    std::string name;
    bool enabled = true;
    std::string description;
    
    // Component configurations
    struct ComponentConfig {
        std::string componentName;
        bool enabled = true;
        std::unordered_map<std::string, std::string> settings;
        std::vector<std::string> dependencies;
    };
    
    std::vector<ComponentConfig> componentConfigs;
    
    // Performance profiling configuration
    struct ProfilingConfig {
        bool enablePerformanceProfiler = true;
        bool enablePerformanceMonitor = true;
        bool enableProfilingDataCollector = true;
        bool enableAnalysisReporter = true;
        bool enableExportIntegration = true;
        bool enableAdvancedProfiler = true;
        
        // Configuration options for each component
        std::string profilerConfigFile;
        std::string monitorConfigFile;
        std::string collectorConfigFile;
        std::string reporterConfigFile;
        std::string exportConfigFile;
        std::string advancedProfilerConfigFile;
        
        ProfilingConfig() = default;
    } profilingConfig;
    
    // Integration configuration
    struct IntegrationConfig {
        std::string debugSystemPath;
        std::string visualizationSystemPath;
        std::string monitoringSystemPath;
        std::string loggingSystemPath;
        std::string databaseConnectionString;
        std::string apiEndpoint;
        
        bool enableDebugSystemIntegration = true;
        bool enableVisualizationIntegration = true;
        bool enableMonitoringIntegration = true;
        bool enableLoggingIntegration = true;
        bool enableDatabaseIntegration = true;
        bool enableAPIIntegration = true;
        
        IntegrationConfig() = default;
    } integrationConfig;
    
    // Global configuration
    struct GlobalConfig {
        std::string logLevel = "info";
        std::chrono::seconds healthCheckInterval = std::chrono::seconds(60);
        std::chrono::seconds statusReportInterval = std::chrono::seconds(300);
        bool enableHealthChecks = true;
        bool enableStatusReporting = true;
        size_t maxConcurrentOperations = 10;
        
        GlobalConfig() = default;
    } globalConfig;
    
    IntegrationConfig() = default;
};

/**
 * Integration status
 */
struct IntegrationStatus {
    std::string integrationId;
    uint64_t timestamp;
    bool enabled = false;
    
    // Component status
    struct ComponentStatus {
        std::string componentName;
        bool enabled = false;
        bool running = false;
        std::string status; // "initialized", "running", "stopped", "error"
        std::string lastError;
        uint64_t uptime = 0;
        double performanceScore = 0.0;
    };
    
    std::vector<ComponentStatus> componentStatus;
    
    // Integration metrics
    uint64_t totalOperations = 0;
    uint64_t successfulOperations = 0;
    uint64_t failedOperations = 0;
    double averageOperationTime = 0.0;
    std::unordered_map<std::string, double> componentMetrics;
    
    IntegrationStatus() : timestamp(0) {}
};

/**
 * Integration workflow
 */
enum class IntegrationWorkflow {
    INITIALIZATION = 0,
    COMPONENT_CONFIGURATION = 1,
    SYSTEM_STARTUP = 2,
    PROFILING_ACTIVATION = 3,
    MONITORING_STARTUP = 4,
    DATA_COLLECTION_START = 5,
    ANALYSIS_START = 6,
    EXPORT_START = 7,
    ADVANCED_FEATURES_START = 8,
    RUNNING = 9,
    SHUTDOWN = 10,
    ERROR_RECOVERY = 11
};

/**
 * Integration event
 */
enum class IntegrationEvent {
    SYSTEM_INITIALIZED = 0,
    COMPONENT_LOADED = 1,
    COMPONENT_STARTED = 2,
    COMPONENT_STOPPED = 3,
    WORKFLOW_PROGRESS = 4,
    PERFORMANCE_ANALYSIS = 5,
    EXPORT_COMPLETED = 6,
    ERROR_OCCURRED = 7,
    HEALTH_CHECK = 8,
    STATUS_REPORT = 9
};

/**
 * Integration event handler
 */
class IntegrationEventHandler {
public:
    virtual ~IntegrationEventHandler() = default;
    
    virtual std::string getHandlerName() const = 0;
    virtual std::string getVersion() const = 0;
    
    virtual void handleEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData) = 0;
    
    virtual bool shouldHandleEvent(IntegrationEvent event) const = 0;
    virtual bool canHandleEvent(IntegrationEvent event) const = 0;
    
    virtual std::string getHandlerStatus() const = 0;
    virtual void startHandler() = 0;
    virtual void stopHandler() = 0;
};

/**
 * Component lifecycle listener
 */
class ComponentLifecycleListener {
public:
    virtual ~ComponentLifecycleListener() = default;
    
    virtual std::string getListenerName() const = 0;
    virtual std::string getVersion() const = 0;
    
    virtual void onComponentInitialized(const std::string& componentName) = 0;
    virtual void onComponentStarted(const std::string& componentName) = 0;
    virtual void onComponentStopped(const std::string& componentName) = 0;
    virtual void onComponentError(const std::string& componentName, const std::string& error) = 0;
    
    virtual std::string getListenerStatus() const = 0;
    virtual void startListener() = 0;
    virtual void stopListener() = 0;
};

/**
 * NLM integration manager
 */
class NLMIntegrationManager {
private:
    std::string managerName_;
    bool running_;
    IntegrationWorkflow currentWorkflow_;
    
    // Configuration
    IntegrationConfig config_;
    
    // Components
    std::shared_ptr<class NLPerformanceProfiler> profiler_;
    std::shared_ptr<class PerformanceMonitor> monitor_;
    std::shared_ptr<class ProfilingDataCollector> dataCollector_;
    std::shared_ptr<class NLPerformanceAnalyzer> analyzer_;
    std::shared_ptr<class ExportIntegrationManager> exportIntegration_;
    std::shared_ptr<class AdvancedProfilerManager> advancedProfiler_;
    
    // State
    IntegrationStatus status_;
    std::atomic<uint64_t> operationCount_{0};
    std::atomic<double> totalOperationTime_{0.0};
    
    mutable std::mutex mutex_;
    
public:
    NLMIntegrationManager(const std::string& name = "NLM Integration Manager");
    ~NLMIntegrationManager();
    
    // Configuration management
    void setConfig(const IntegrationConfig& config) { config_ = config; }
    const IntegrationConfig& getConfig() const { return config_; }
    
    // Component management
    void initializeComponents();
    void startComponents();
    void stopComponents();
    void shutdownComponents();
    
    // Individual component management
    void initializeProfiler();
    void initializeMonitor();
    void initializeDataCollector();
    void initializeAnalyzer();
    void initializeExportIntegration();
    void initializeAdvancedProfiler();
    
    void startProfiler();
    void startMonitor();
    void startDataCollector();
    void startAnalyzer();
    void startExportIntegration();
    void startAdvancedProfiler();
    
    void stopProfiler();
    void stopMonitor();
    void stopDataCollector();
    void stopAnalyzer();
    void stopExportIntegration();
    void stopAdvancedProfiler();
    
    // Workflow management
    void startIntegrationWorkflow();
    void stopIntegrationWorkflow();
    void pauseIntegrationWorkflow();
    void resumeIntegrationWorkflow();
    
    bool isRunning() const { return running_.load(); }
    bool isWorkflowRunning(IntegrationWorkflow workflow) const;
    
    // Status and monitoring
    IntegrationStatus getStatus() const;
    std::string getManagerStatus() const;
    
    // Component status
    std::string getComponentStatus(const std::string& componentName) const;
    std::vector<std::string> getComponentNames() const;
    
    // Configuration management
    void loadConfigurationFromFile(const std::string& configFile);
    void saveConfigurationToFile(const std::string& configFile) const;
    bool validateConfiguration() const;
    
    // Dynamic configuration
    void updateComponentConfig(const std::string& componentName, const std::unordered_map<std::string, std::string>& settings);
    std::unordered_map<std::string, std::string> getComponentConfig(const std::string& componentName) const;
    
    // Integration points
    void integrateWithProfiler(class NLPerformanceProfiler& profiler);
    void integrateWithMonitor(class PerformanceMonitor& monitor);
    void integrateWithDataCollector(class ProfilingDataCollector& collector);
    void integrateWithAnalyzer(class NLPerformanceAnalyzer& analyzer);
    void integrateWithExportIntegration(class ExportIntegrationManager& exportManager);
    void integrateWithAdvancedProfiler(class AdvancedProfilerManager& advancedProfiler);
    
    // Event handling
    void addEventHandler(std::shared_ptr<IntegrationEventHandler> handler);
    void removeEventHandler(const std::string& handlerName);
    void addLifecycleListener(std::shared_ptr<ComponentLifecycleListener> listener);
    void removeLifecycleListener(const std::string& listenerName);
    
    // Event management
    void emitEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData = {});
    void broadcastEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData = {});
    
    // Performance analysis
    std::string performSystemAnalysis() const;
    std::string performHealthCheck() const;
    std::string performPerformanceOptimization() const;
    
    // Reporting
    std::string generateIntegrationReport() const;
    std::string generateStatusReport() const;
    std::string generatePerformanceReport() const;
    
    // Export
    std::string exportIntegrationStatus(const std::string& format = "json") const;
    bool saveIntegrationReport(const std::string& outputPath, const std::string& format = "json") const;
    
    // Error handling
    std::vector<std::string> getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }
    void addError(const std::string& error) { errors_.push_back(error); }
    
    // Health checks
    bool performHealthCheck(const std::string& componentName) const;
    bool performPerformanceHealthCheck() const;
    bool performDataIntegrityCheck() const;
    
    // Resource management
    size_t getMaxConcurrentOperations() const;
    void setMaxConcurrentOperations(size_t maxOperations);
    
    // Configuration validation
    bool validateComponentConfiguration(const std::string& componentName) const;
    bool validateSystemConfiguration() const;
    
private:
    std::vector<std::string> errors_;
    std::vector<std::shared_ptr<IntegrationEventHandler>> eventHandlers_;
    std::vector<std::shared_ptr<ComponentLifecycleListener>> lifecycleListeners_;
    
    void executeWorkflowStep(IntegrationWorkflow workflow);
    void onWorkflowProgress(IntegrationWorkflow workflow, const std::string& stepDescription);
    void onComponentError(const std::string& componentName, const std::string& error);
    void onHealthCheckComplete(const std::string& componentName, bool healthy);
    
    std::string getWorkflowStepDescription(IntegrationWorkflow workflow) const;
    std::chrono::seconds getWorkflowStepDuration(IntegrationWorkflow workflow) const;
    
    void initializeComponent(const std::string& componentName);
    void startComponent(const std::string& componentName);
    void stopComponent(const std::string& componentName);
    
    void updateComponentStatus(const std::string& componentName, const std::string& status);
    void updateComponentMetrics(const std::string& componentName, double metricValue);
    
    void setupComponentDependencies(const std::string& componentName);
    bool checkComponentDependencies(const std::string& componentName) const;
    
    void logWorkflowProgress(const std::string& message);
    void logComponentStatus(const std::string& componentName, const std::string& status);
    
    void createComponentStatus(const std::string& componentName);
    void updateComponentStatusInternal(const std::string& componentName, bool enabled, bool running, const std::string& status);
    
    void calculateSystemPerformanceScore();
    void updateIntegrationMetrics();
    
    std::string formatStatusReport() const;
    std::string formatPerformanceReport() const;
    
    bool isComponentHealthy(const std::string& componentName) const;
    double calculateComponentPerformanceScore(const std::string& componentName) const;
};

/**
 * Default event handlers
 */
class DefaultEventHandlers {
public:
    static std::shared_ptr<IntegrationEventHandler> createSystemEventHandler();
    static std::shared_ptr<IntegrationEventHandler> createComponentEventHandler();
    static std::shared_ptr<IntegrationEventHandler> createPerformanceEventHandler();
    static std::shared_ptr<IntegrationEventHandler> createErrorEventHandler();
    static std::shared_ptr<IntegrationEventHandler> createHealthEventHandler();
    
    static std::vector<std::string> getAvailableEventHandlers();
};

/**
 * Default lifecycle listeners
 */
class DefaultLifecycleListeners {
public:
    static std::shared_ptr<ComponentLifecycleListener> createProfilerLifecycleListener();
    static std::shared_ptr<ComponentLifecycleListener> createMonitorLifecycleListener();
    static std::shared_ptr<ComponentLifecycleListener> createCollectorLifecycleListener();
    static std::shared_ptr<ComponentLifecycleListener> createAnalyzerLifecycleListener();
    static std::shared_ptr<ComponentLifecycleListener> createExportLifecycleListener();
    static std::shared_ptr<ComponentLifecycleListener> createAdvancedProfilerLifecycleListener();
    
    static std::vector<std::string> getAvailableLifecycleListeners();
};

/**
 * Integration factory
 */
class NLMIntegrationFactory {
public:
    static std::shared_ptr<NLMIntegrationManager> createManager(const std::string& name = "NLM Integration Manager");
    static std::shared_ptr<NLMIntegrationManager> createManager(const IntegrationConfig& config);
    
    static std::vector<std::string> getAvailableManagers();
    static std::vector<std::string> getAvailableEventHandlers();
    static std::vector<std::string> getAvailableLifecycleListeners();
};

} // namespace nlm