// NLMIntegration.cpp - Implementation of Main Integration Module

#include "NLMIntegration.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include <pugixml.hpp>
#include <cmath>
#include <set>
#include <queue>
#include <regex>
#include <numeric>

namespace nlm {

// Helper functions for integration
double calculateSystemPerformanceScore(const IntegrationStatus& status) {
    if (status.componentStatus.empty()) return 0.0;
    
    double totalScore = 0.0;
    for (const auto& component : status.componentStatus) {
        totalScore += component.performanceScore;
    }
    
    return totalScore / status.componentStatus.size();
}

std::string formatStatus(const IntegrationStatus& status) {
    std::stringstream ss;
    ss << "Integration Status Report:\n";
    ss << "  Integration ID: " << status.integrationId << "\n";
    ss << "  Timestamp: " << status.timestamp << "\n";
    ss << "  Enabled: " << (status.enabled ? "Yes" : "No") << "\n";
    ss << "  Components: " << status.componentStatus.size() << "\n";
    
    ss << "  Component Status:\n";
    for (const auto& component : status.componentStatus) {
        ss << "    " << component.componentName << ": " << component.status;
        if (!component.lastError.empty()) {
            ss << " (Error: " << component.lastError << ")";
        }
        ss << "\n";
    }
    
    ss << "  Operations: " << status.totalOperations << " total, "
       << status.successfulOperations << " successful, "
       << status.failedOperations << " failed\n";
    
    if (status.totalOperations > 0) {
        ss << "  Average operation time: " << status.averageOperationTime << " seconds\n";
    }
    
    return ss.str();
}

// IntegrationEventHandler implementations
class DefaultSystemEventHandler : public IntegrationEventHandler {
private:
    std::string handlerName_;
    bool running_;
    
public:
    DefaultSystemEventHandler(const std::string& name = "System Event Handler")
        : handlerName_(name), running_(false) {}
    
    std::string getHandlerName() const override { return handlerName_; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void handleEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData) override {
        // Handle system events
        switch (event) {
            case IntegrationEvent::SYSTEM_INITIALIZED:
                // System initialization complete
                break;
            case IntegrationEvent::COMPONENT_LOADED:
                // Component loaded
                break;
            case IntegrationEvent::COMPONENT_STARTED:
                // Component started
                break;
            case IntegrationEvent::COMPONENT_STOPPED:
                // Component stopped
                break;
            case IntegrationEvent::WORKFLOW_PROGRESS:
                // Workflow progress update
                break;
            case IntegrationEvent::PERFORMANCE_ANALYSIS:
                // Performance analysis complete
                break;
            case IntegrationEvent::EXPORT_COMPLETED:
                // Export completed
                break;
            case IntegrationEvent::ERROR_OCCURRED:
                // Error occurred
                break;
            case IntegrationEvent::HEALTH_CHECK:
                // Health check
                break;
            case IntegrationEvent::STATUS_REPORT:
                // Status report
                break;
        }
    }
    
    bool shouldHandleEvent(IntegrationEvent event) const override {
        return event == IntegrationEvent::SYSTEM_INITIALIZED ||
               event == IntegrationEvent::COMPONENT_LOADED ||
               event == IntegrationEvent::COMPONENT_STARTED ||
               event == IntegrationEvent::COMPONENT_STOPPED ||
               event == IntegrationEvent::WORKFLOW_PROGRESS ||
               event == IntegrationEvent::ERROR_OCCURRED;
    }
    
    bool canHandleEvent(IntegrationEvent event) const override {
        return shouldHandleEvent(event);
    }
    
    std::string getHandlerStatus() const override {
        return running_ ? "running" : "stopped";
    }
    
    void startHandler() override {
        running_ = true;
    }
    
    void stopHandler() override {
        running_ = false;
    }
};

class DefaultComponentEventHandler : public IntegrationEventHandler {
private:
    std::string handlerName_;
    bool running_;
    
public:
    DefaultComponentEventHandler(const std::string& name = "Component Event Handler")
        : handlerName_(name), running_(false) {}
    
    std::string getHandlerName() const override { return handlerName_; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void handleEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData) override {
        // Handle component events
        if (event == IntegrationEvent::COMPONENT_LOADED) {
            // Component loaded successfully
        } else if (event == IntegrationEvent::COMPONENT_STARTED) {
            // Component started successfully
        } else if (event == IntegrationEvent::COMPONENT_STOPPED) {
            // Component stopped successfully
        }
    }
    
    bool shouldHandleEvent(IntegrationEvent event) const override {
        return event == IntegrationEvent::COMPONENT_LOADED ||
               event == IntegrationEvent::COMPONENT_STARTED ||
               event == IntegrationEvent::COMPONENT_STOPPED;
    }
    
    bool canHandleEvent(IntegrationEvent event) const override {
        return shouldHandleEvent(event);
    }
    
    std::string getHandlerStatus() const override {
        return running_ ? "running" : "stopped";
    }
    
    void startHandler() override {
        running_ = true;
    }
    
    void stopHandler() override {
        running_ = false;
    }
};

class DefaultPerformanceEventHandler : public IntegrationEventHandler {
private:
    std::string handlerName_;
    bool running_;
    
public:
    DefaultPerformanceEventHandler(const std::string& name = "Performance Event Handler")
        : handlerName_(name), running_(false) {}
    
    std::string getHandlerName() const override { return handlerName_; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void handleEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData) override {
        // Handle performance events
        if (event == IntegrationEvent::PERFORMANCE_ANALYSIS) {
            // Performance analysis complete
        }
    }
    
    bool shouldHandleEvent(IntegrationEvent event) const override {
        return event == IntegrationEvent::PERFORMANCE_ANALYSIS;
    }
    
    bool canHandleEvent(IntegrationEvent event) const override {
        return shouldHandleEvent(event);
    }
    
    std::string getHandlerStatus() const override {
        return running_ ? "running" : "stopped";
    }
    
    void startHandler() override {
        running_ = true;
    }
    
    void stopHandler() override {
        running_ = false;
    }
};

class DefaultErrorEventHandler : public IntegrationEventHandler {
private:
    std::string handlerName_;
    bool running_;
    
public:
    DefaultErrorEventHandler(const std::string& name = "Error Event Handler")
        : handlerName_(name), running_(false) {}
    
    std::string getHandlerName() const override { return handlerName_; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void handleEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData) override {
        // Handle error events
        if (event == IntegrationEvent::ERROR_OCCURRED) {
            // Error occurred - log and potentially trigger recovery
        }
    }
    
    bool shouldHandleEvent(IntegrationEvent event) const override {
        return event == IntegrationEvent::ERROR_OCCURRED;
    }
    
    bool canHandleEvent(IntegrationEvent event) const override {
        return shouldHandleEvent(event);
    }
    
    std::string getHandlerStatus() const override {
        return running_ ? "running" : "stopped";
    }
    
    void startHandler() override {
        running_ = true;
    }
    
    void stopHandler() override {
        running_ = false;
    }
};

class DefaultHealthEventHandler : public IntegrationEventHandler {
private:
    std::string handlerName_;
    bool running_;
    
public:
    DefaultHealthEventHandler(const std::string& name = "Health Event Handler")
        : handlerName_(name), running_(false) {}
    
    std::string getHandlerName() const override { return handlerName_; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void handleEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData) override {
        // Handle health events
        if (event == IntegrationEvent::HEALTH_CHECK) {
            // Health check completed
        }
    }
    
    bool shouldHandleEvent(IntegrationEvent event) const override {
        return event == IntegrationEvent::HEALTH_CHECK;
    }
    
    bool canHandleEvent(IntegrationEvent event) const override {
        return shouldHandleEvent(event);
    }
    
    std::string getHandlerStatus() const override {
        return running_ ? "running" : "stopped";
    }
    
    void startHandler() override {
        running_ = true;
    }
    
    void stopHandler() override {
        running_ = false;
    }
};

// DefaultLifecycleListener implementations
class DefaultProfilerLifecycleListener : public ComponentLifecycleListener {
private:
    std::string listenerName_;
    bool running_;
    
public:
    DefaultProfilerLifecycleListener(const std::string& name = "Profiler Lifecycle Listener")
        : listenerName_(name), running_(false) {}
    
    std::string getListenerName() const override { return listenerName_; }
    std::string getVersion() const override { return "1.0.0"; }
    
    void onComponentInitialized(const std::string& componentName) override {
        if (componentName == "profiler") {
            // Profiler initialized
        }
    }
    
    void onComponentStarted(const std::string& componentName) override {
        if (componentName == "profiler") {
            // Profiler started
        }
    }
    
    void onComponentStopped(const std::string& componentName) override {
        if (componentName == "profiler") {
            // Profiler stopped
        }
    }
    
    void onComponentError(const std::string& componentName, const std::string& error) override {
        if (componentName == "profiler") {
            // Profiler error
        }
    }
    
    std::string getListenerStatus() const override {
        return running_ ? "running" : "stopped";
    }
    
    void startListener() override {
        running_ = true;
    }
    
    void stopListener() override {
        running_ = false;
    }
};

// DefaultEventHandlers implementation
std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createSystemEventHandler() {
    return std::make_shared<DefaultSystemEventHandler>();
}

std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createComponentEventHandler() {
    return std::make_shared<DefaultComponentEventHandler>();
}

std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createPerformanceEventHandler() {
    return std::make_shared<DefaultPerformanceEventHandler>();
}

std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createErrorEventHandler() {
    return std::make_shared<DefaultErrorEventHandler>();
}

std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createHealthEventHandler() {
    return std::make_shared<DefaultHealthEventHandler>();
}

std::vector<std::string> DefaultEventHandlers::getAvailableEventHandlers() {
    return {"system", "component", "performance", "error", "health"};
}

// DefaultLifecycleListeners implementation
std::shared_ptr<ComponentLifecycleListener> DefaultLifecycleListeners::createProfilerLifecycleListener() {
    return std::make_shared<DefaultProfilerLifecycleListener>();
}

std::shared_ptr<ComponentLifecycleListener> DefaultLifecycleListeners::createMonitorLifecycleListener() {
    return std::make_shared<DefaultProfilerLifecycleListener>();
}

std::shared_ptr<ComponentLifecycleListener> DefaultLifecycleListeners::createCollectorLifecycleListener() {
    return std::make_shared<DefaultProfilerLifecycleListener>();
}

std::shared_ptr<ComponentLifecycleListener> DefaultLifecycleListeners::createAnalyzerLifecycleListener() {
    return std::make_shared<DefaultProfilerLifecycleListener>();
}

std::shared_ptr<ComponentLifecycleListener> DefaultLifecycleListeners::createExportLifecycleListener() {
    return std::make_shared<DefaultProfilerLifecycleListener>();
}

std::shared_ptr<ComponentLifecycleListener> DefaultLifecycleListeners::createAdvancedProfilerLifecycleListener() {
    return std::make_shared<DefaultProfilerLifecycleListener>();
}

std::vector<std::string> DefaultLifecycleListeners::getAvailableLifecycleListeners() {
    return {"profiler", "monitor", "collector", "analyzer", "export", "advanced_profiler"};
}

// NLMIntegrationManager implementation
NLMIntegrationManager::NLMIntegrationManager(const std::string& name) : managerName_(name), running_(false) {
    currentWorkflow_ = IntegrationWorkflow::INITIALIZATION;
    status_.integrationId = "integration_" + std::to_string(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    status_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

NLMIntegrationManager::~NLMIntegrationManager() {
    stopIntegrationWorkflow();
}

void NLMIntegrationManager::initializeComponents() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Initialize all components
    initializeProfiler();
    initializeMonitor();
    initializeDataCollector();
    initializeAnalyzer();
    initializeExportIntegration();
    initializeAdvancedProfiler();
    
    // Update status
    currentWorkflow_ = IntegrationWorkflow::COMPONENT_CONFIGURATION;
}

void NLMIntegrationManager::startComponents() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Start all components
    startProfiler();
    startMonitor();
    startDataCollector();
    startAnalyzer();
    startExportIntegration();
    startAdvancedProfiler();
    
    // Update status
    currentWorkflow_ = IntegrationWorkflow::SYSTEM_STARTUP;
}

void NLMIntegrationManager::stopComponents() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Stop all components
    stopProfiler();
    stopMonitor();
    stopDataCollector();
    stopAnalyzer();
    stopExportIntegration();
    stopAdvancedProfiler();
    
    // Update status
    currentWorkflow_ = IntegrationWorkflow::SHUTDOWN;
}

void NLMIntegrationManager::shutdownComponents() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Shutdown all components
    stopComponents();
    
    // Update status
    currentWorkflow_ = IntegrationWorkflow::SHUTDOWN;
}

void NLMIntegrationManager::initializeProfiler() {
    // Initialize NLPerformanceProfiler
    profiler_ = std::make_shared<NLPerformanceProfiler>("nlm_profiler", ProfileConfig());
    createComponentStatus("profiler");
}

void NLMIntegrationManager::initializeMonitor() {
    // Initialize PerformanceMonitor
    monitor_ = std::make_shared<PerformanceMonitor>("performance_monitor", PerformanceMonitorConfig());
    createComponentStatus("monitor");
}

void NLMIntegrationManager::initializeDataCollector() {
    // Initialize ProfilingDataCollector
    dataCollector_ = std::make_shared<ProfilingDataCollector>("data_collector", DataCollectionConfig());
    createComponentStatus("data_collector");
}

void NLMIntegrationManager::initializeAnalyzer() {
    // Initialize NLPerformanceAnalyzer
    analyzer_ = std::make_shared<NLPerformanceAnalyzer>("performance_analyzer");
    createComponentStatus("analyzer");
}

void NLMIntegrationManager::initializeExportIntegration() {
    // Initialize ExportIntegrationManager
    exportIntegration_ = std::make_shared<ExportIntegrationManager>("export_integration");
    createComponentStatus("export_integration");
}

void NLMIntegrationManager::initializeAdvancedProfiler() {
    // Initialize AdvancedProfilerManager
    advancedProfiler_ = std::make_shared<AdvancedProfilerManager>("advanced_profiler");
    createComponentStatus("advanced_profiler");
}

void NLMIntegrationManager::startProfiler() {
    if (profiler_) {
        profiler_->start();
        updateComponentStatus("profiler", "running");
    }
}

void NLMIntegrationManager::startMonitor() {
    if (monitor_) {
        monitor_->start();
        updateComponentStatus("monitor", "running");
    }
}

void NLMIntegrationManager::startDataCollector() {
    if (dataCollector_) {
        dataCollector_->start();
        updateComponentStatus("data_collector", "running");
    }
}

void NLMIntegrationManager::startAnalyzer() {
    if (analyzer_) {
        analyzer_->start();
        updateComponentStatus("analyzer", "running");
    }
}

void NLMIntegrationManager::startExportIntegration() {
    if (exportIntegration_) {
        exportIntegration_->start();
        updateComponentStatus("export_integration", "running");
    }
}

void NLMIntegrationManager::startAdvancedProfiler() {
    if (advancedProfiler_) {
        advancedProfiler_->start();
        updateComponentStatus("advanced_profiler", "running");
    }
}

void NLMIntegrationManager::stopProfiler() {
    if (profiler_ && profiler_->isRunning()) {
        profiler_->stop();
        updateComponentStatus("profiler", "stopped");
    }
}

void NLMIntegrationManager::stopMonitor() {
    if (monitor_ && monitor_->isRunning()) {
        monitor_->stop();
        updateComponentStatus("monitor", "stopped");
    }
}

void NLMIntegrationManager::stopDataCollector() {
    if (dataCollector_ && dataCollector_->isRunning()) {
        dataCollector_->stop();
        updateComponentStatus("data_collector", "stopped");
    }
}

void NLMIntegrationManager::stopAnalyzer() {
    if (analyzer_ && analyzer_->isRunning()) {
        analyzer_->stop();
        updateComponentStatus("analyzer", "stopped");
    }
}

void NLMIntegrationManager::stopExportIntegration() {
    if (exportIntegration_ && exportIntegration_->isRunning()) {
        exportIntegration_->stop();
        updateComponentStatus("export_integration", "stopped");
    }
}

void NLMIntegrationManager::stopAdvancedProfiler() {
    if (advancedProfiler_ && advancedProfiler_->isRunning()) {
        advancedProfiler_->stop();
        updateComponentStatus("advanced_profiler", "stopped");
    }
}

void NLMIntegrationManager::startIntegrationWorkflow() {
    if (running_.load()) return;
    
    running_.store(true);
    currentWorkflow_ = IntegrationWorkflow::RUNNING;
    
    // Start the integration workflow
    executeWorkflowStep(currentWorkflow_);
}

void NLMIntegrationManager::stopIntegrationWorkflow() {
    if (!running_.load()) return;
    
    running_.store(false);
    currentWorkflow_ = IntegrationWorkflow::SHUTDOWN;
    
    // Stop the integration workflow
    stopComponents();
}

void NLMIntegrationManager::pauseIntegrationWorkflow() {
    if (!running_.load()) return;
    
    currentWorkflow_ = IntegrationWorkflow::RUNNING; // Simplified pause
}

void NLMIntegrationManager::resumeIntegrationWorkflow() {
    if (!running_.load()) return;
    
    currentWorkflow_ = IntegrationWorkflow::RUNNING; // Simplified resume
}

bool NLMIntegrationManager::isWorkflowRunning(IntegrationWorkflow workflow) const {
    return currentWorkflow_ == workflow;
}

IntegrationStatus NLMIntegrationManager::getStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Update integration metrics
    updateIntegrationMetrics();
    
    // Calculate system performance score
    calculateSystemPerformanceScore();
    
    return status_;
}

std::string NLMIntegrationManager::getManagerStatus() const {
    std::stringstream ss;
    ss << "NLM Integration Manager Status:\n";
    ss << "  Manager: " << managerName_ << "\n";
    ss << "  Running: " << (running_.load() ? "Yes" : "No") << "\n";
    ss << "  Current Workflow: " << static_cast<int>(currentWorkflow_) << "\n";
    ss << "  Components: " << status_.componentStatus.size() << "\n";
    ss << "  Total Operations: " << operationCount_.load() << "\n";
    ss << "  Successful Operations: " << status_.successfulOperations << "\n";
    ss << "  Failed Operations: " << status_.failedOperations << "\n";
    
    if (operationCount_.load() > 0) {
        ss << "  Average Operation Time: " << totalOperationTime_.load() / operationCount_.load() << " seconds\n";
    }
    
    ss << "  System Performance Score: " << calculateSystemPerformanceScore(status_) << "\n";
    
    return ss.str();
}

std::string NLMIntegrationManager::getComponentStatus(const std::string& componentName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& component : status_.componentStatus) {
        if (component.componentName == componentName) {
            std::stringstream ss;
            ss << component.componentName << ": " << component.status;
            if (!component.lastError.empty()) {
                ss << " (Error: " << component.lastError << ")";
            }
            return ss.str();
        }
    }
    
    return "Component not found: " + componentName;
}

std::vector<std::string> NLMIntegrationManager::getComponentNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> componentNames;
    for (const auto& component : status_.componentStatus) {
        componentNames.push_back(component.componentName);
    }
    
    return componentNames;
}

void NLMIntegrationManager::loadConfigurationFromFile(const std::string& configFile) {
    try {
        YAML::Node config = YAML::LoadFile(configFile);
        
        // Parse configuration
        if (config["name"]) {
            config_.name = config["name"].as<std::string>();
        }
        
        if (config["enabled"]) {
            config_.enabled = config["enabled"].as<bool>();
        }
        
        if (config["description"]) {
            config_.description = config["description"].as<std::string>();
        }
        
        // Parse component configurations
        if (config["componentConfigs"]) {
            config_.componentConfigs.clear();
            YAML::Node components = config["componentConfigs"];
            for (const auto& component : components) {
                IntegrationConfig::ComponentConfig compConfig;
                compConfig.componentName = component["componentName"].as<std::string>();
                compConfig.enabled = component["enabled"].as<bool>();
                
                if (component["settings"]) {
                    YAML::Node settings = component["settings"];
                    for (const auto& setting : settings) {
                        compConfig.settings[setting.first.as<std::string>()] = setting.second.as<std::string>();
                    }
                }
                
                config_.componentConfigs.push_back(compConfig);
            }
        }
        
        logWorkflowProgress("Configuration loaded from " + configFile);
    } catch (const std::exception& e) {
        addError("Failed to load configuration from " + configFile + ": " + std::string(e.what()));
    }
}

void NLMIntegrationManager::saveConfigurationToFile(const std::string& configFile) const {
    try {
        YAML::Emitter out;
        
        out << YAML::BeginMap;
        out << YAML::Key << "name" << YAML::Value << config_.name;
        out << YAML::Key << "enabled" << YAML::Value << config_.enabled;
        out << YAML::Key << "description" << YAML::Value << config_.description;
        
        out << YAML::Key << "componentConfigs" << YAML::Value;
        out << YAML::BeginSeq;
        for (const auto& component : config_.componentConfigs) {
            out << YAML::BeginMap;
            out << YAML::Key << "componentName" << YAML::Value << component.componentName;
            out << YAML::Key << "enabled" << YAML::Value << component.enabled;
            
            if (!component.settings.empty()) {
                out << YAML::Key << "settings" << YAML::Value;
                out << YAML::BeginMap;
                for (const auto& setting : component.settings) {
                    out << YAML::Key << setting.first << YAML::Value << setting.second;
                }
                out << YAML::EndMap;
            }
            
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
        
        out << YAML::EndMap;
        
        std::ofstream file(configFile);
        file << out.c_str();
        file.close();
        
        logWorkflowProgress("Configuration saved to " + configFile);
    } catch (const std::exception& e) {
        addError("Failed to save configuration to " + configFile + ": " + std::string(e.what()));
    }
}

bool NLMIntegrationManager::validateConfiguration() const {
    // Validate configuration
    if (config_.name.empty()) {
        addError("Configuration name cannot be empty");
        return false;
    }
    
    if (config_.componentConfigs.empty()) {
        addError("No component configurations found");
        return false;
    }
    
    // Validate each component configuration
    for (const auto& component : config_.componentConfigs) {
        if (component.componentName.empty()) {
            addError("Component name cannot be empty");
            return false;
        }
    }
    
    return true;
}

void NLMIntegrationManager::updateComponentConfig(const std::string& componentName,
                                                   const std::unordered_map<std::string, std::string>& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& component : config_.componentConfigs) {
        if (component.componentName == componentName) {
            component.settings = settings;
            logWorkflowProgress("Updated configuration for component: " + componentName);
            return;
        }
    }
    
    addError("Component not found: " + componentName);
}

std::unordered_map<std::string, std::string> NLMIntegrationManager::getComponentConfig(const std::string& componentName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& component : config_.componentConfigs) {
        if (component.componentName == componentName) {
            return component.settings;
        }
    }
    
    return std::unordered_map<std::string, std::string>();
}

void NLMIntegrationManager::integrateWithProfiler(NLPerformanceProfiler& profiler) {
    // Integration with NLPerformanceProfiler
    if (profiler_) {
        profiler_->integrateWithProfiler(profiler);
    }
}

void NLMIntegrationManager::integrateWithMonitor(PerformanceMonitor& monitor) {
    // Integration with PerformanceMonitor
    if (monitor_) {
        monitor_->integrateWithProfiler(*profiler_);
    }
}

void NLMIntegrationManager::integrateWithDataCollector(ProfilingDataCollector& collector) {
    // Integration with ProfilingDataCollector
    if (dataCollector_) {
        dataCollector_->integrateWithProfiler(*profiler_);
        dataCollector_->integrateWithMonitor(*monitor_);
    }
}

void NLMIntegrationManager::integrateWithAnalyzer(NLPerformanceAnalyzer& analyzer) {
    // Integration with NLPerformanceAnalyzer
    if (analyzer_) {
        // Analyzer integration logic
    }
}

void NLMIntegrationManager::integrateWithExportIntegration(ExportIntegrationManager& exportManager) {
    // Integration with ExportIntegrationManager
    if (exportIntegration_) {
        // Export integration logic
    }
}

void NLMIntegrationManager::integrateWithAdvancedProfiler(AdvancedProfilerManager& advancedProfiler) {
    // Integration with AdvancedProfilerManager
    if (advancedProfiler_) {
        advancedProfiler_->integrateWithProfiler(*profiler_);
        advancedProfiler_->integrateWithMonitor(*monitor_);
        advancedProfiler_->integrateWithDataCollector(*dataCollector_);
    }
}

void NLMIntegrationManager::addEventHandler(std::shared_ptr<IntegrationEventHandler> handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    eventHandlers_.push_back(handler);
}

void NLMIntegrationManager::removeEventHandler(const std::string& handlerName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::remove_if(eventHandlers_.begin(), eventHandlers_.end(),
                            [&handlerName](const std::shared_ptr<IntegrationEventHandler>& handler) {
                                return handler->getHandlerName() == handlerName;
                            });
    
    eventHandlers_.erase(it, eventHandlers_.end());
}

void NLMIntegrationManager::addLifecycleListener(std::shared_ptr<ComponentLifecycleListener> listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    lifecycleListeners_.push_back(listener);
}

void NLMIntegrationManager::removeLifecycleListener(const std::string& listenerName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::remove_if(lifecycleListeners_.begin(), lifecycleListeners_.end(),
                            [&listenerName](const std::shared_ptr<ComponentLifecycleListener>& listener) {
                                return listener->getListenerName() == listenerName;
                            });
    
    lifecycleListeners_.erase(it, lifecycleListeners_.end());
}

void NLMIntegrationManager::emitEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::unordered_map<std::string, std::string> fullEventData = eventData;
    fullEventData["event"] = std::to_string(static_cast<int>(event));
    fullEventData["timestamp"] = std::to_string(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    
    // Notify event handlers
    for (const auto& handler : eventHandlers_) {
        if (handler->canHandleEvent(event)) {
            try {
                handler->handleEvent(event, fullEventData);
            } catch (const std::exception& e) {
                addError("Error handling event " + std::to_string(static_cast<int>(event)) +
                       " in handler " + handler->getHandlerName() + ": " + std::string(e.what()));
            }
        }
    }
    
    // Notify lifecycle listeners
    for (const auto& listener : lifecycleListeners_) {
        switch (event) {
            case IntegrationEvent::COMPONENT_LOADED:
                // Find the component name from eventData
                auto it = eventData.find("componentName");
                if (it != eventData.end()) {
                    listener->onComponentInitialized(it->second);
                }
                break;
            case IntegrationEvent::COMPONENT_STARTED:
                it = eventData.find("componentName");
                if (it != eventData.end()) {
                    listener->onComponentStarted(it->second);
                }
                break;
            case IntegrationEvent::COMPONENT_STOPPED:
                it = eventData.find("componentName");
                if (it != eventData.end()) {
                    listener->onComponentStopped(it->second);
                }
                break;
            case IntegrationEvent::ERROR_OCCURRED:
                it = eventData.find("componentName");
                std::string componentName = it != eventData.end() ? it->second : "unknown";
                it = eventData.find("errorMessage");
                std::string errorMessage = it != eventData.end() ? it->second : "unknown error";
                listener->onComponentError(componentName, errorMessage);
                break;
        }
    }
}

void NLMIntegrationManager::broadcastEvent(IntegrationEvent event, const std::unordered_map<std::string, std::string>& eventData) {
    // Same as emit but with additional logging
    emitEvent(event, eventData);
    logWorkflowProgress("Event emitted: " + std::to_string(static_cast<int>(event)));
}

std::string NLMIntegrationManager::performSystemAnalysis() const {
    std::stringstream ss;
    ss << "System Analysis:\n";
    
    if (profilers_.empty()) {
        ss << "  No profilers configured\n";
        return ss.str();
    }
    
    // Analyze profiler performance
    double totalScore = 0.0;
    size_t healthyProfilers = 0;
    
    for (const auto& profiler : profilers_) {
        if (profiler->isRunning()) {
            double score = profiler->getCurrentPerformanceScore();
            totalScore += score;
            if (score > 0.7) {
                healthyProfilers++;
            }
        }
    }
    
    double avgScore = totalScore / profilers_.size();
    
    ss << "  Average performance score: " << avgScore << "\n";
    ss << "  Healthy profilers: " << healthyProfilers << " / " << profilers_.size() << "\n";
    
    if (avgScore > 0.8) {
        ss << "  Performance: EXCELLENT\n";
    } else if (avgScore > 0.6) {
        ss << "  Performance: GOOD\n";
    } else if (avgScore > 0.4) {
        ss << "  Performance: ACCEPTABLE\n";
    } else {
        ss << "  Performance: NEEDS IMPROVEMENT\n";
    }
    
    return ss.str();
}

std::string NLMIntegrationManager::performHealthCheck() const {
    std::stringstream ss;
    ss << "Health Check:\n";
    
    if (profilers_.empty()) {
        ss << "  No profilers configured\n";
        return ss.str();
    }
    
    bool allHealthy = true;
    
    for (const auto& profiler : profilers_) {
        if (!profiler->isHealthy()) {
            allHealthy = false;
            ss << "  UNHEALTHY: " << profiler->getProfilerName() << "\n";
        } else {
            ss << "  HEALTHY: " << profiler->getProfilerName() << "\n";
        }
    }
    
    if (allHealthy) {
        ss << "  System health: GOOD\n";
    } else {
        ss << "  System health: ISSUES DETECTED\n";
    }
    
    return ss.str();
}

std::string NLMIntegrationManager::performPerformanceOptimization() const {
    std::stringstream ss;
    ss << "Performance Optimization:\n";
    
    if (profilers_.empty()) {
        ss << "  No profilers configured\n";
        return ss.str();
    }
    
    // Generate optimization recommendations
    std::vector<std::string> recommendations;
    
    for (const auto& profiler : profilers_) {
        if (profiler->isRunning()) {
            // Get optimization recommendations for this profiler
            auto recs = profiler->getOptimizationRecommendations();
            recommendations.insert(recommendations.end(), recs.begin(), recs.end());
        }
    }
    
    if (recommendations.empty()) {
        ss << "  No optimization recommendations available\n";
    } else {
        ss << "  Optimization recommendations:\n";
        for (const auto& rec : recommendations) {
            ss << "    - " << rec << "\n";
        }
    }
    
    return ss.str();
}

std::string NLMIntegrationManager::generateIntegrationReport() const {
    std::stringstream ss;
    ss << "NLM Integration Report\n";
    ss << "=======================\n\n";
    
    ss << formatStatusReport() << "\n";
    ss << generateStatusReport() << "\n";
    ss << generatePerformanceReport() << "\n";
    
    return ss.str();
}

std::string NLMIntegrationManager::generateStatusReport() const {
    return formatStatusReport();
}

std::string NLMIntegrationManager::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "Performance Report:\n";
    ss << "=================\n\n";
    
    if (profilers_.empty()) {
        ss << "No profilers configured\n";
        return ss.str();
    }
    
    // Generate performance metrics for each profiler
    for (const auto& profiler : profilers_) {
        if (profiler->isRunning()) {
            ss << "Profiler: " << profiler->getProfilerName() << "\n";
            ss << "  Performance Score: " << profiler->getCurrentPerformanceScore() << "\n";
            ss << "  Uptime: " << profiler->getUptime() << " seconds\n";
            ss << "  Memory Usage: " << profiler->getCurrentMemoryUsage() << " bytes\n";
            ss << "  Active Events: " << profiler->getActiveEventCount() << "\n";
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::string NLMIntegrationManager::exportIntegrationStatus(const std::string& format) const {
    if (format == "json") {
        return exportToJSON();
    } else if (format == "csv") {
        return exportToCSV();
    } else if (format == "yaml") {
        return exportToYAML();
    } else if (format == "xml") {
        return exportToXML();
    } else {
        return exportToJSON(); // Default
    }
}

bool NLMIntegrationManager::saveIntegrationReport(const std::string& outputPath, const std::string& format) const {
    try {
        std::string report;
        
        if (format == "json") {
            report = exportToJSON();
        } else if (format == "csv") {
            report = exportToCSV();
        } else if (format == "yaml") {
            report = exportToYAML();
        } else if (format == "xml") {
            report = exportToXML();
        } else {
            report = exportToJSON(); // Default
        }
        
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            addError("Failed to open output file: " + outputPath);
            return false;
        }
        
        file << report;
        file.close();
        
        logWorkflowProgress("Integration report saved to " + outputPath);
        return true;
    } catch (const std::exception& e) {
        addError("Failed to save integration report: " + std::string(e.what()));
        return false;
    }
}

bool NLMIntegrationManager::performHealthCheck(const std::string& componentName) const {
    // Perform health check for a specific component
    for (const auto& profiler : profilers_) {
        if (profiler->getProfilerName() == componentName) {
            return profiler->isHealthy();
        }
    }
    
    return false;
}

bool NLMIntegrationManager::performPerformanceHealthCheck() const {
    // Perform overall performance health check
    if (profilers_.empty()) return true;
    
    double totalScore = 0.0;
    for (const auto& profiler : profilers_) {
        if (profiler->isRunning()) {
            totalScore += profiler->getCurrentPerformanceScore();
        }
    }
    
    double avgScore = totalScore / profilers_.size();
    return avgScore > 0.5; // Consider healthy if average score > 0.5
}

bool NLMIntegrationManager::performDataIntegrityCheck() const {
    // Perform data integrity check
    if (profilers_.empty()) return true;
    
    for (const auto& profiler : profilers_) {
        if (!profiler->isDataIntegrityGood()) {
            return false;
        }
    }
    
    return true;
}

size_t NLMIntegrationManager::getMaxConcurrentOperations() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_.globalConfig.maxConcurrentOperations;
}

void NLMIntegrationManager::setMaxConcurrentOperations(size_t maxOperations) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_.globalConfig.maxConcurrentOperations = maxOperations;
}

bool NLMIntegrationManager::validateComponentConfiguration(const std::string& componentName) const {
    // Validate component configuration
    for (const auto& component : config_.componentConfigs) {
        if (component.componentName == componentName) {
            return true; // Simplified validation
        }
    }
    return false;
}

bool NLMIntegrationManager::validateSystemConfiguration() const {
    // Validate system configuration
    return validateConfiguration();
}

void NLMIntegrationManager::executeWorkflowStep(IntegrationWorkflow workflow) {
    std::string stepDescription = getWorkflowStepDescription(workflow);
    onWorkflowProgress(workflow, stepDescription);
    
    logWorkflowProgress("Executing workflow step: " + stepDescription);
    
    // Execute the workflow step
    switch (workflow) {
        case IntegrationWorkflow::INITIALIZATION:
            // Initialize the system
            initializeComponents();
            break;
        case IntegrationWorkflow::COMPONENT_CONFIGURATION:
            // Configure components
            break;
        case IntegrationWorkflow::SYSTEM_STARTUP:
            // Start the system
            startComponents();
            break;
        case IntegrationWorkflow::PROFILING_ACTIVATION:
            // Activate profiling
            break;
        case IntegrationWorkflow::MONITORING_STARTUP:
            // Start monitoring
            break;
        case IntegrationWorkflow::DATA_COLLECTION_START:
            // Start data collection
            break;
        case IntegrationWorkflow::ANALYSIS_START:
            // Start analysis
            break;
        case IntegrationWorkflow::EXPORT_START:
            // Start export
            break;
        case IntegrationWorkflow::ADVANCED_FEATURES_START:
            // Start advanced features
            break;
        case IntegrationWorkflow::RUNNING:
            // System is running
            break;
        case IntegrationWorkflow::SHUTDOWN:
            // Shutdown the system
            stopComponents();
            break;
        case IntegrationWorkflow::ERROR_RECOVERY:
            // Error recovery
            break;
    }
    
    emitEvent(IntegrationEvent::WORKFLOW_PROGRESS, {
        {"workflow", std::to_string(static_cast<int>(workflow))},
        {"description", stepDescription}
    });
}

void NLMIntegrationManager::onWorkflowProgress(IntegrationWorkflow workflow, const std::string& stepDescription) {
    // Update status
    status_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void NLMIntegrationManager::onComponentError(const std::string& componentName, const std::string& error) {
    updateComponentStatus(componentName, "error");
    addError("Component " + componentName + " error: " + error);
}

void NLMIntegrationManager::onHealthCheckComplete(const std::string& componentName, bool healthy) {
    if (healthy) {
        logWorkflowProgress("Health check passed for component: " + componentName);
    } else {
        addError("Health check failed for component: " + componentName);
    }
}

std::string NLMIntegrationManager::getWorkflowStepDescription(IntegrationWorkflow workflow) const {
    switch (workflow) {
        case IntegrationWorkflow::INITIALIZATION:
            return "Initialize system components";
        case IntegrationWorkflow::COMPONENT_CONFIGURATION:
            return "Configure system components";
        case IntegrationWorkflow::SYSTEM_STARTUP:
            return "Start system components";
        case IntegrationWorkflow::PROFILING_ACTIVATION:
            return "Activate profiling system";
        case IntegrationWorkflow::MONITORING_STARTUP:
            return "Start monitoring system";
        case IntegrationWorkflow::DATA_COLLECTION_START:
            return "Start data collection";
        case IntegrationWorkflow::ANALYSIS_START:
            return "Start analysis system";
        case IntegrationWorkflow::EXPORT_START:
            return "Start export system";
        case IntegrationWorkflow::ADVANCED_FEATURES_START:
            return "Start advanced features";
        case IntegrationWorkflow::RUNNING:
            return "System is running";
        case IntegrationWorkflow::SHUTDOWN:
            return "Shutdown system components";
        case IntegrationWorkflow::ERROR_RECOVERY:
            return "Recover from errors";
        default:
            return "Unknown workflow step";
    }
}

std::chrono::seconds NLMIntegrationManager::getWorkflowStepDuration(IntegrationWorkflow workflow) const {
    // Return estimated duration for workflow step
    return std::chrono::seconds(30); // Default 30 seconds
}

void NLMIntegrationManager::initializeComponent(const std::string& componentName) {
    // Initialize a specific component
    updateComponentStatus(componentName, "initialized");
    emitEvent(IntegrationEvent::COMPONENT_LOADED, {{"componentName", componentName}});
}

void NLMIntegrationManager::startComponent(const std::string& componentName) {
    // Start a specific component
    updateComponentStatus(componentName, "running");
    emitEvent(IntegrationEvent::COMPONENT_STARTED, {{"componentName", componentName}});
}

void NLMIntegrationManager::stopComponent(const std::string& componentName) {
    // Stop a specific component
    updateComponentStatus(componentName, "stopped");
    emitEvent(IntegrationEvent::COMPONENT_STOPPED, {{"componentName", componentName}});
}

void NLMIntegrationManager::updateComponentStatus(const std::string& componentName, const std::string& status) {
    // Update component status
    for (auto& component : status_.componentStatus) {
        if (component.componentName == componentName) {
            component.status = status;
            component.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            break;
        }
    }
}

void NLMIntegrationManager::updateComponentMetrics(const std::string& componentName, double metricValue) {
    // Update component metrics
    for (auto& component : status_.componentStatus) {
        if (component.componentName == componentName) {
            component.performanceScore = metricValue;
            break;
        }
    }
}

void NLMIntegrationManager::setupComponentDependencies(const std::string& componentName) {
    // Setup component dependencies
    // Simplified implementation
}

bool NLMIntegrationManager::checkComponentDependencies(const std::string& componentName) const {
    // Check component dependencies
    return true; // Simplified implementation
}

void NLMIntegrationManager::logWorkflowProgress(const std::string& message) {
    // Log workflow progress
    // In a real implementation, this would log to a logging system
}

void NLMIntegrationManager::logComponentStatus(const std::string& componentName, const std::string& status) {
    // Log component status
    // In a real implementation, this would log to a logging system
}

void NLMIntegrationManager::createComponentStatus(const std::string& componentName) {
    // Create component status
    IntegrationStatus::ComponentStatus component;
    component.componentName = componentName;
    component.enabled = true;
    component.running = false;
    component.status = "initialized";
    component.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    status_.componentStatus.push_back(component);
}

void NLMIntegrationManager::updateComponentStatusInternal(const std::string& componentName, bool enabled, bool running, const std::string& status) {
    // Update component status internally
    for (auto& component : status_.componentStatus) {
        if (component.componentName == componentName) {
            component.enabled = enabled;
            component.running = running;
            component.status = status;
            break;
        }
    }
}

void NLMIntegrationManager::calculateSystemPerformanceScore() {
    // Calculate system performance score
    double totalScore = 0.0;
    for (const auto& component : status_.componentStatus) {
        totalScore += component.performanceScore;
    }
    
    if (!status_.componentStatus.empty()) {
        status_.performanceScore = totalScore / status_.componentStatus.size();
    }
}

void NLMIntegrationManager::updateIntegrationMetrics() {
    // Update integration metrics
    status_.totalOperations = operationCount_.load();
    status_.successfulOperations = operationCount_.load(); // Simplified
    status_.failedOperations = 0; // Simplified
    status_.averageOperationTime = totalOperationTime_.load() / status_.totalOperations;
}

std::string NLMIntegrationManager::formatStatusReport() const {
    return formatStatus(status_);
}

std::string NLMIntegrationManager::formatPerformanceReport() const {
    std::stringstream ss;
    ss << "Performance Metrics:\n";
    ss << "==================\n\n";
    
    ss << "Total Operations: " << status_.totalOperations << "\n";
    ss << "Successful Operations: " << status_.successfulOperations << "\n";
    ss << "Failed Operations: " << status_.failedOperations << "\n";
    
    if (status_.totalOperations > 0) {
        ss << "Average Operation Time: " << status_.averageOperationTime << " seconds\n";
    }
    
    ss << "System Performance Score: " << status_.performanceScore << "\n";
    
    return ss.str();
}

std::string NLMIntegrationManager::exportToJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"integrationId\": \"" << status_.integrationId << "\",\n";
    ss << "  \"timestamp\": " << status_.timestamp << ",\n";
    ss << "  \"enabled\": " << (status_.enabled ? "true" : "false") << ",\n";
    ss << "  \"components\": [\n";
    
    for (size_t i = 0; i < status_.componentStatus.size(); ++i) {
        const auto& component = status_.componentStatus[i];
        ss << "    {\"componentName\": \"" << component.componentName << "\", \"enabled\": " 
           << (component.enabled ? "true" : "false") << ", \"running\": " 
           << (component.running ? "true" : "false") << ", \"status\": \"" 
           << component.status << "\"";
        
        if (!component.lastError.empty()) {
            ss << ", \"lastError\": \"" << component.lastError << "\"";
        }
        
        ss << (i == status_.componentStatus.size() - 1 ? "}" : "},\n");
    }
    
    ss << "  ],\n";
    ss << "  \"totalOperations\": " << status_.totalOperations << ",\n";
    ss << "  \"successfulOperations\": " << status_.successfulOperations << ",\n";
    ss << "  \"failedOperations\": " << status_.failedOperations << ",\n";
    ss << "  \"averageOperationTime\": " << status_.averageOperationTime << ",\n";
    ss << "  \"performanceScore\": " << status_.performanceScore << "\n";
    ss << "}\n";
    
    return ss.str();
}

std::string NLMIntegrationManager::exportToCSV() const {
    std::stringstream ss;
    ss << "componentName,enabled,running,status,lastError,performanceScore\n";
    
    for (const auto& component : status_.componentStatus) {
        ss << component.componentName << ","
           << (component.enabled ? "true" : "false") << ","
           << (component.running ? "true" : "false") << ","
           << component.status << ","
           << (component.lastError.empty() ? "" : component.lastError) << ","
           << component.performanceScore << "\n";
    }
    
    return ss.str();
}

std::string NLMIntegrationManager::exportToYAML() const {
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "integrationId" << YAML::Value << status_.integrationId;
    out << YAML::Key << "timestamp" << YAML::Value << status_.timestamp;
    out << YAML::Key << "enabled" << YAML::Value << status_.enabled;
    
    out << YAML::Key << "components" << YAML::Value;
    out << YAML::BeginSeq;
    for (const auto& component : status_.componentStatus) {
        out << YAML::BeginMap;
        out << YAML::Key << "componentName" << YAML::Value << component.componentName;
        out << YAML::Key << "enabled" << YAML::Value << component.enabled;
        out << YAML::Key << "running" << YAML::Value << component.running;
        out << YAML::Key << "status" << YAML::Value << component.status;
        
        if (!component.lastError.empty()) {
            out << YAML::Key << "lastError" << YAML::Value << component.lastError;
        }
        
        out << YAML::Key << "performanceScore" << YAML::Value << component.performanceScore;
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    
    out << YAML::Key << "totalOperations" << YAML::Value << status_.totalOperations;
    out << YAML::Key << "successfulOperations" << YAML::Value << status_.successfulOperations;
    out << YAML::Key << "failedOperations" << YAML::Value << status_.failedOperations;
    out << YAML::Key << "averageOperationTime" << YAML::Value << status_.averageOperationTime;
    out << YAML::Key << "performanceScore" << YAML::Value << status_.performanceScore;
    
    out << YAML::EndMap;
    
    return out.c_str();
}

std::string NLMIntegrationManager::exportToXML() const {
    pugi::xml_document doc;
    auto root = doc.append_child("IntegrationStatus");
    
    root.append_attribute("integrationId") = status_.integrationId.c_str();
    root.append_attribute("timestamp") = static_cast<unsigned long long>(status_.timestamp);
    root.append_attribute("enabled") = status_.enabled;
    
    auto componentsElem = root.append_child("Components");
    for (const auto& component : status_.componentStatus) {
        auto componentElem = componentsElem.append_child("Component");
        componentElem.append_attribute("name") = component.componentName.c_str();
        componentElem.append_attribute("enabled") = component.enabled;
        componentElem.append_attribute("running") = component.running;
        componentElem.append_attribute("status") = component.status.c_str();
        componentElem.append_attribute("performanceScore") = component.performanceScore;
        
        if (!component.lastError.empty()) {
            componentElem.append_child("Error").append_child_node_value(component.lastError.c_str());
        }
    }
    
    auto metricsElem = root.append_child("Metrics");
    metricsElem.append_child("totalOperations").append_child_node_value(std::to_string(status_.totalOperations).c_str());
    metricsElem.append_child("successfulOperations").append_child_node_value(std::to_string(status_.successfulOperations).c_str());
    metricsElem.append_child("failedOperations").append_child_node_value(std::to_string(status_.failedOperations).c_str());
    metricsElem.append_child("averageOperationTime").append_child_node_value(std::to_string(status_.averageOperationTime).c_str());
    metricsElem.append_child("performanceScore").append_child_node_value(std::to_string(status_.performanceScore).c_str());
    
    std::stringstream ss;
    doc.save(ss);
    return ss.str();
}

// DefaultEventHandlers implementation
std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createSystemEventHandler() {
    return std::make_shared<DefaultSystemEventHandler>();
}

std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createComponentEventHandler() {
    return std::make_shared<DefaultComponentEventHandler>();
}

std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createPerformanceEventHandler() {
    return std::make_shared<DefaultPerformanceEventHandler>();
}

std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createErrorEventHandler() {
    return std::make_shared<DefaultErrorEventHandler>();
}

std::shared_ptr<IntegrationEventHandler> DefaultEventHandlers::createHealthEventHandler() {
    return std::make_shared<DefaultHealthEventHandler>();
}

std::vector<std::string> DefaultEventHandlers::getAvailableEventHandlers() {
    return {"system", "component", "performance", "error", "health"};
}

// NLMIntegrationFactory implementation
std::shared_ptr<NLMIntegrationManager> NLMIntegrationFactory::createManager(const std::string& name) {
    return std::make_shared<NLMIntegrationManager>(name);
}

std::shared_ptr<NLMIntegrationManager> NLMIntegrationFactory::createManager(const IntegrationConfig& config) {
    auto manager = std::make_shared<NLMIntegrationManager>();
    manager->setConfig(config);
    return manager;
}

std::vector<std::string> NLMIntegrationFactory::getAvailableManagers() {
    return {"nlm_integration"};
}

std::vector<std::string> NLMIntegrationFactory::getAvailableEventHandlers() {
    return DefaultEventHandlers::getAvailableEventHandlers();
}

std::vector<std::string> NLMIntegrationFactory::getAvailableLifecycleListeners() {
    return DefaultLifecycleListeners::getAvailableLifecycleListeners();
}

} // namespace nlm