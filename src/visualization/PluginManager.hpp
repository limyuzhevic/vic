// Plugin manager for NLM visualization and debugging plugins
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

namespace nlm {

struct PluginInfo {
    std::string pluginId;
    std::string pluginName;
    std::string pluginVersion;
    std::string pluginDescription;
    std::string author;
    std::string license;
    std::string website;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> metadata;
    std::chrono::steady_clock::time_point loadedTime;
    bool isEnabled;
    bool isCorePlugin;
    std::vector<std::string> providedServices;
    std::vector<std::string> supportedFormats;
};

struct PluginContext {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<VisualizationEngine> visualizationEngine;
    std::shared_ptr<ActivityAnalyzer> activityAnalyzer;
    std::shared_ptr<BrainStateDebugger> brainStateDebugger;
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    std::shared_ptr<MemoryAnalyzer> memoryAnalyzer;
    std::shared_ptr<DevelopmentInterface> developmentInterface;
    std::shared_ptr<ExportManager> exportManager;
    
    std::function<void(const std::string&)> logMessage;
    std::function<void(const std::string&)> errorMessage;
    std::function<void(double)> progressUpdate;
    std::function<void(const std::string&, const std::string&)> notifyService;
    std::function<void(const std::string&, const std::string&, const std::string&)> notifyEvent;
    std::function<void(const ExportOptions&)> requestExport;
};

struct Plugin {
    virtual ~Plugin() {}
    virtual std::string getPluginId() const = 0;
    virtual std::string getPluginName() const = 0;
    virtual std::string getPluginVersion() const = 0;
    virtual bool initialize(const PluginContext& context) = 0;
    virtual void shutdown() = 0;
    virtual void update(const VisualizationData& data) {}
    virtual bool handleEvent(const std::string& eventType, const std::map<std::string, std::string>& parameters) { return false; }
    virtual std::string getService(const std::string& serviceName) const { return ""; }
    virtual bool provideService(const std::string& serviceName, const std::string& serviceData) { return false; }
    virtual std::vector<std::string> getSupportedFormats() const { return {}; }
    virtual std::vector<std::string> getProvidedServices() const { return {}; }
};

struct VisualizationPlugin : public Plugin {
    virtual void visualize(const VisualizationData& data, const ExportOptions& options) = 0;
    virtual void setConfiguration(const std::map<std::string, std::string>& config) {}
    virtual std::map<std::string, std::string> getConfiguration() const { return {}; }
};

struct AnalysisPlugin : public Plugin {
    virtual std::vector<std::string> analyze(const VisualizationData& data) = 0;
    virtual std::map<std::string, double> computeMetrics(const VisualizationData& data) = 0;
    virtual std::string getAnalysisResult(const std::string& analysisType) const { return ""; }
};

struct DebugPlugin : public Plugin {
    virtual std::string debug(const std::string& target) = 0;
    virtual std::vector<std::string> getDebugInfo(const std::string& target) const { return {}; }
    virtual bool fixIssue(const std::string& issue) { return false; }
};

struct ExportPlugin : public Plugin {
    virtual bool exportData(const ExportOptions& options, const std::vector<float>& data) = 0;
    virtual std::string getExportFormat() const { return "unknown"; }
    virtual bool supportsFormat(const std::string& format) const { return false; }
};

class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    bool initialize(std::shared_ptr<Brain> brain);
    void shutdown();
    
    void update(const VisualizationData& data);
    
    // Plugin lifecycle
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginId);
    bool enablePlugin(const std::string& pluginId);
    bool disablePlugin(const std::string& pluginId);
    bool isPluginLoaded(const std::string& pluginId) const;
    bool isPluginEnabled(const std::string& pluginId) const;
    
    // Plugin information
    PluginInfo getPluginInfo(const std::string& pluginId) const;
    std::vector<PluginInfo> getLoadedPlugins() const;
    std::vector<PluginInfo> getEnabledPlugins() const;
    std::vector<std::string> getPluginIds() const;
    std::vector<std::string> getEnabledPluginIds() const;
    
    // Service management
    std::string getService(const std::string& serviceName) const;
    bool provideService(const std::string& serviceName, const std::string& serviceData);
    std::vector<std::string> getAvailableServices() const;
    
    // Format support
    bool supportsFormat(const std::string& format) const;
    std::vector<std::string> getSupportedFormats() const;
    
    // Plugin management
    void registerPlugin(const std::shared_ptr<Plugin>& plugin);
    void unregisterPlugin(const std::string& pluginId);
    std::shared_ptr<Plugin> getPlugin(const std::string& pluginId) const;
    
    // Configuration
    void setPluginDirectory(const std::string& directory);
    std::string getPluginDirectory() const;
    
    // Plugin discovery
    std::vector<PluginInfo> discoverPlugins() const;
    bool validatePlugin(const std::string& pluginPath) const;
    
    // Export management
    ExportReport exportUsingPlugin(const std::string& pluginId, const ExportOptions& options) const;
    bool canExportFormat(const std::string& pluginId, const std::string& format) const;
    
    // Event handling
    void broadcastEvent(const std::string& eventType, const std::map<std::string, std::string>& parameters);
    void registerEventHandler(const std::string& eventType, std::function<bool(const std::map<std::string, std::string>&)> handler);
    
    // Plugin-specific operations
    std::vector<std::string> getPluginCommands(const std::string& pluginId) const;
    std::string executePluginCommand(const std::string& pluginId, const std::string& command, const std::map<std::string, std::string>& parameters);
    
    // Visualization plugin operations
    void addVisualizationPlugin(const std::shared_ptr<VisualizationPlugin>& plugin);
    std::vector<std::shared_ptr<VisualizationPlugin>> getVisualizationPlugins() const;
    void executeVisualizationPlugins(const VisualizationData& data, const ExportOptions& options) const;
    
    // Analysis plugin operations
    void addAnalysisPlugin(const std::shared_ptr<AnalysisPlugin>& plugin);
    std::vector<std::shared_ptr<AnalysisPlugin>> getAnalysisPlugins() const;
    std::vector<std::string> executeAnalysisPlugins(const VisualizationData& data) const;
    std::map<std::string, double> computeMetricsUsingPlugins(const VisualizationData& data) const;
    
    // Debug plugin operations
    void addDebugPlugin(const std::shared_ptr<DebugPlugin>& plugin);
    std::vector<std::shared_ptr<DebugPlugin>> getDebugPlugins() const;
    std::string executeDebugPlugins(const std::string& target) const;
    std::vector<std::string> getDebugInfoUsingPlugins(const std::string& target) const;
    
    // Export plugin operations
    void addExportPlugin(const std::shared_ptr<ExportPlugin>& plugin);
    std::vector<std::shared_ptr<ExportPlugin>> getExportPlugins() const;
    ExportReport executeExportPlugins(const ExportOptions& options, const std::vector<float>& data) const;
    
    // Plugin state
    bool isPluginRunning(const std::string& pluginId) const;
    std::chrono::steady_clock::time_point getPluginLoadTime(const std::string& pluginId) const;
    double getPluginUptime(const std::string& pluginId) const;
    
    // Statistics
    size_t getTotalPlugins() const;
    size_t getLoadedPluginsCount() const;
    size_t getEnabledPluginsCount() const;
    size_t getRunningPluginsCount() const;
    double getAveragePluginPerformance() const;
    
    // Error handling
    std::vector<std::string> getPluginErrors() const;
    void clearPluginErrors();
    
    // Configuration
    void setAutoLoad(bool autoLoad);
    void setPluginSearchPath(const std::string& path);
    bool getAutoLoad() const;
    std::string getPluginSearchPath() const;
    
    // Export management
    void setExportPluginPriority(const std::string& pluginId, int priority);
    int getExportPluginPriority(const std::string& pluginId) const;
    std::vector<std::string> getExportPluginOrder() const;
    
    // Plugin lifecycle callbacks
    void registerPluginLoadCallback(std::function<void(const std::string&)> callback);
    void registerPluginUnloadCallback(std::function<void(const std::string&)> callback);
    void registerPluginErrorCallback(std::function<void(const std::string&, const std::string&)> callback);
    void registerPluginStatusChangeCallback(std::function<void(const std::string&, bool)> callback);
    
    // Plugin information queries
    std::string getPluginName(const std::string& pluginId) const;
    std::string getPluginVersion(const std::string& pluginId) const;
    std::string getPluginDescription(const std::string& pluginId) const;
    std::vector<std::string> getPluginDependencies(const std::string& pluginId) const;
    std::map<std::string, std::string> getPluginMetadata(const std::string& pluginId) const;

private:
    std::shared_ptr<Brain> brain_;
    
    // Plugin management
    std::string pluginDirectory_;
    bool autoLoad_;
    std::string pluginSearchPath_;
    
    std::map<std::string, std::shared_ptr<Plugin>> plugins_;
    std::map<std::string, PluginInfo> pluginInfos_;
    std::map<std::string, bool> pluginEnabled_;
    
    // Visualization plugins
    std::vector<std::shared_ptr<VisualizationPlugin>> visualizationPlugins_;
    
    // Analysis plugins
    std::vector<std::shared_ptr<AnalysisPlugin>> analysisPlugins_;
    
    // Debug plugins
    std::vector<std::shared_ptr<DebugPlugin>> debugPlugins_;
    
    // Export plugins
    std::vector<std::shared_ptr<ExportPlugin>> exportPlugins_;
    
    // Event handling
    std::map<std::string, std::vector<std::function<bool(const std::map<std::string, std::string>&)>>> eventHandlers_;
    
    // Statistics
    size_t totalPlugins_;
    size_t loadedPluginsCount_;
    size_t enabledPluginsCount_;
    size_t runningPluginsCount_;
    
    // Callbacks
    std::vector<std::function<void(const std::string&)>> pluginLoadCallbacks_;
    std::vector<std::function<void(const std::string&)>> pluginUnloadCallbacks_;
    std::vector<std::function<void(const std::string&, const std::string&)>> pluginErrorCallbacks_;
    std::vector<std::function<void(const std::string&, bool)> > pluginStatusChangeCallbacks_;
    
    // Plugin loading
    bool loadPluginFromFile(const std::string& pluginPath);
    bool loadPluginFromMemory(const std::string& pluginData);
    std::shared_ptr<Plugin> createPluginInstance(const PluginInfo& info);
    
    // Plugin validation
    bool validatePluginCompatibility(const PluginInfo& info) const;
    bool checkDependencies(const PluginInfo& info) const;
    
    // Plugin lifecycle
    void initializePlugin(const std::string& pluginId);
    void shutdownPlugin(const std::string& pluginId);
    void enablePluginInternal(const std::string& pluginId);
    void disablePluginInternal(const std::string& pluginId);
    
    // Plugin discovery
    void scanPluginDirectory() const;
    bool isPluginFile(const std::string& filename) const;
    
    // Statistics
    void updateStatistics();
    void resetStatistics();
    
    // Error handling
    void logPluginError(const std::string& pluginId, const std::string& error);
    
    // Export plugin management
    void updateExportPluginOrder();
    bool comparePluginPriorities(const std::string& pluginId1, const std::string& pluginId2) const;
    
    // Service management
    void updatePluginServices() const;
    
    // Callback helpers
    void invokePluginLoadCallback(const std::string& pluginId) const;
    void invokePluginUnloadCallback(const std::string& pluginId) const;
    void invokePluginErrorCallback(const std::string& pluginId, const std::string& error) const;
    void invokePluginStatusChangeCallback(const std::string& pluginId, bool enabled) const;
    
    mutable std::mutex mutex_;
};
