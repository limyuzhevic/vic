#pragma once

#include "IPlugin.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../memory/Memory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <memory>
#include <map>
#include <vector>
#include <mutex>

namespace nlm {
namespace plugins {

// Plugin integration context for brain systems
struct PluginIntegrationContext {
    Brain* brain;
    STDP* stdp;
    Hebbian* hebbian;
    StructuralPlasticity* structuralPlasticity;
    Dopamine* dopamine;
    Curiosity* curiosity;
    Novelty* novelty;
    PredictionError* predictionError;
    DevelopmentSystem* developmentSystem;
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    NeuralAssociativeMemory* associativeMemory;
    PredictionSystem* predictionSystem;
    NeuralPlanner* planner;
    ConceptFormation* conceptFormation;
    AttentionalSelection* attention;
    RandomGenerator* randomGenerator;
    SimulationClock* clock;
    Config* config;
    Logger* logger;
    
    PluginIntegrationContext()
        : brain(nullptr), stdp(nullptr), hebbian(nullptr), structuralPlasticity(nullptr),
          dopamine(nullptr), curiosity(nullptr), novelty(nullptr), predictionError(nullptr),
          developmentSystem(nullptr), workingMemory(nullptr), episodicMemory(nullptr),
          associativeMemory(nullptr), predictionSystem(nullptr), planner(nullptr),
          conceptFormation(nullptr), attention(nullptr), randomGenerator(nullptr),
          clock(nullptr), config(nullptr), logger(nullptr) {}
};

// Plugin manager for brain integration
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin lifecycle management
    bool initialize();
    void shutdown();
    void update(TimestepDuration dt);
    void cleanup();
    
    // Plugin loading and management
    bool loadPlugin(const std::string& pluginName, const std::map<std::string, std::string>& config = {});
    bool unloadPlugin(const std::string& pluginName);
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    
    // Plugin access
    IPlugin* getPlugin(const std::string& name);
    const IPlugin* getPlugin(const std::string& name) const;
    
    // Plugin integration with brain systems
    bool integrateWithBrain(Brain* brain);
    bool deintegrateFromBrain();
    bool isIntegrated() const;
    
    // System-specific plugin access
    IPlasticityPlugin* getPlasticityPlugin(const std::string& name = "");
    INeuromodulationPlugin* getNeuromodulationPlugin(const std::string& name = "");
    IMemoryPlugin* getMemoryPlugin(const std::string& name = "");
    IDevelopmentPlugin* getDevelopmentPlugin(const std::string& name = "");
    
    // Integration points
    bool integratePlasticity(PluginIntegrationContext& context);
    bool integrateNeuromodulation(PluginIntegrationContext& context);
    bool integrateMemory(PluginIntegrationContext& context);
    bool integrateDevelopment(PluginIntegrationContext& context);
    bool integratePrediction(PluginIntegrationContext& context);
    bool integrateCognition(PluginIntegrationContext& context);
    
    // Performance monitoring
    struct PluginPerformanceStats {
        std::string pluginName;
        double totalUpdateTime;
        double averageUpdateTime;
        uint64_t updateCount;
        double cpuUsage;
        size_t memoryUsage;
        uint64_t errorCount;
        double uptime;
    };
    
    std::vector<PluginPerformanceStats> getPluginPerformanceStats() const;
    double getPluginCpuUsage(const std::string& pluginName) const;
    size_t getPluginMemoryUsage(const std::string& pluginName) const;
    
    // Error handling
    std::vector<PluginError> getPluginErrors(const std::string& pluginName) const;
    bool hasPluginErrors(const std::string& pluginName) const;
    void clearPluginErrors(const std::string& pluginName);
    
    // Configuration management
    bool configurePlugin(const std::string& pluginName, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& pluginName) const;
    std::map<std::string, std::map<std::string, std::string>> getAllPluginConfigs() const;
    
    // Event system integration
    void subscribeToPluginEvents(const std::string& pluginName, 
                                const PluginEventSystem::EventCallback& callback);
    void unsubscribeFromPluginEvents(const std::string& pluginName,
                                    const PluginEventSystem::EventCallback& callback);
    
    // Plugin dependency management
    bool checkPluginDependencies(const std::string& pluginName) const;
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;
    bool resolvePluginDependencies(const std::string& pluginName);
    
    // Plugin validation
    bool validatePlugin(const std::string& pluginName) const;
    std::vector<std::string> getPluginValidationErrors(const std::string& pluginName) const;
    
    // Plugin statistics
    size_t getPluginCount() const;
    size_t getLoadedPluginCount() const;
    size_t getEnabledPluginCount() const;
    size_t getIntegratedPluginCount() const;
    
    // Plugin lifecycle management
    bool pausePlugin(const std::string& pluginName);
    bool resumePlugin(const std::string& pluginName);
    bool restartPlugin(const std::string& pluginName);
    
    // Plugin backup and recovery
    bool backupPluginState(const std::string& pluginName);
    bool restorePluginState(const std::string& pluginName);
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Global configuration
    void setGlobalConfig(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getGlobalConfig() const;
    
    // Performance tuning
    void setPluginUpdateInterval(const std::string& pluginName, TimestepDuration interval);
    TimestepDuration getPluginUpdateInterval(const std::string& pluginName) const;
    void setPluginPriority(const std::string& pluginName, int priority);
    int getPluginPriority(const std::string& pluginName) const;
    
    // Monitoring and diagnostics
    std::map<std::string, std::string> getPluginStatus(const std::string& pluginName) const;
    std::map<std::string, std::map<std::string, std::string>> getAllPluginStatuses() const;
    bool performHealthCheck();
    std::vector<std::string> getHealthCheckResults() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin configuration management
class PluginConfiguration {
public:
    PluginConfiguration();
    explicit PluginConfiguration(const std::string& configFile);
    ~PluginConfiguration();
    
    // Configuration loading
    bool loadFromFile(const std::string& filename);
    bool loadFromString(const std::string& configString);
    bool saveToFile(const std::string& filename) const;
    
    // Plugin configuration
    bool configurePlugin(const std::string& pluginName, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& pluginName) const;
    std::map<std::string, std::map<std::string, std::string>> getAllConfigs() const;
    
    // Configuration templates
    bool setPluginConfigTemplate(const std::string& pluginName, const PluginConfigGroup& templateConfig);
    PluginConfigGroup getPluginConfigTemplate(const std::string& pluginName) const;
    
    // Configuration validation
    bool validateAllConfigs() const;
    std::map<std::string, std::vector<std::string>> getValidationErrors() const;
    
    // Configuration export/import
    std::string exportConfig() const;
    bool importConfig(const std::string& configString);
    
    // Configuration monitoring
    using ConfigChangeCallback = std::function<void(const std::string& pluginName, const std::map<std::string, std::string>& oldConfig, const std::map<std::string, std::string>& newConfig)>;
    void subscribeToConfigChanges(const ConfigChangeCallback& callback);
    void unsubscribeFromConfigChanges(const ConfigChangeCallback& callback);
    
    // Configuration history
    struct ConfigChangeHistory {
        std::string pluginName;
        std::map<std::string, std::string> oldConfig;
        std::map<std::string, std::string> newConfig;
        double timestamp;
    };
    std::vector<ConfigChangeHistory> getConfigHistory(const std::string& pluginName = "") const;
    void clearConfigHistory(const std::string& pluginName = "");
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin performance monitor
class PluginPerformanceMonitor {
public:
    PluginPerformanceMonitor();
    ~PluginPerformanceMonitor();
    
    // Performance monitoring
    void recordPluginUpdate(const std::string& pluginName, double updateTime, double cpuUsage, size_t memoryUsage);
    void recordPluginError(const std::string& pluginName, const PluginError& error);
    
    // Performance statistics
    struct PerformanceMetrics {
        std::string pluginName;
        double totalUpdateTime;
        double averageUpdateTime;
        double standardDeviation;
        uint64_t updateCount;
        double cpuUsage;
        double cpuUsageMax;
        size_t memoryUsage;
        size_t memoryUsageMax;
        uint64_t errorCount;
        double errorRate;
        double uptime;
        double lastUpdateTime;
        
        PerformanceMetrics(const std::string& name) : pluginName(name), totalUpdateTime(0.0), 
            averageUpdateTime(0.0), standardDeviation(0.0), updateCount(0), cpuUsage(0.0),
            cpuUsageMax(0.0), memoryUsage(0), memoryUsageMax(0), errorCount(0), errorRate(0.0),
            uptime(0.0), lastUpdateTime(0.0) {}
    };
    
    std::vector<PerformanceMetrics> getPerformanceMetrics(const std::string& pluginName = "") const;
    PerformanceMetrics getAggregatedMetrics() const;
    
    // Performance analysis
    std::vector<std::string> getPerformanceIssues() const;
    std::vector<std::string> getBottleneckPlugins() const;
    double getPluginSlowdownRatio(const std::string& pluginName) const;
    
    // Performance alerts
    using PerformanceAlertCallback = std::function<void(const std::string& pluginName, const std::string& issue)>;
    void subscribeToPerformanceAlerts(const PerformanceAlertCallback& callback);
    void unsubscribeFromPerformanceAlerts(const PerformanceAlertCallback& callback);
    void checkForPerformanceIssues();
    
    // Performance optimization
    void optimizePluginPerformance(const std::string& pluginName);
    void rebalancePluginLoad();
    
    // Performance report generation
    std::string generatePerformanceReport() const;
    std::string generateDetailedPerformanceReport(const std::string& pluginName) const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Configuration
    void setPerformanceSamplingInterval(double interval);
    void setAlertThreshold(double cpuThreshold, double memoryThreshold, double updateTimeThreshold);
    void setMaxPerformanceHistorySize(size_t maxSize);
    
    // Cleanup
    void cleanupOldMetrics();
    void resetMetrics(const std::string& pluginName = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace plugins
} // namespace nlm
