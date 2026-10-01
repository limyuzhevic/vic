#pragma once

// Plugin Visualization Interface for NLM (Neural Learning Machine)
// Defines the interface for visualization plugins

#include "PluginCore.hpp"
#include "../visualization/VisualizationCore.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// Plugin Visualization Categories
enum class VisualizationPluginCategory {
    Neural,          // Neural network visualization
    Temporal,       // Time series visualization
    Spatial,        // Spatial/3D visualization
    Statistical,    // Statistical data visualization
    Interactive,    // Interactive visualization
    Export,         // Export visualization
    Debug,          // Debug visualization
    Scientific,     // Scientific visualization
    Educational,    // Educational visualization
    RealTime       // Real-time visualization
};

// Plugin Visualization Types
enum class VisualizationType {
    Graph,          // Graph/network visualization
    Timeline,       // Timeline visualization
    Heatmap,        // Heatmap visualization
    Scatter,        // Scatter plot visualization
    BarChart,       // Bar chart visualization
    LineChart,      // Line chart visualization
    PieChart,       // Pie chart visualization
    Surface,        // Surface 3D visualization
    Volume,         // Volume visualization
    Custom         // Custom visualization
};

// Plugin Visualization Data
struct VisualizationData {
    std::string name;
    VisualizationType type;
    std::map<std::string, std::vector<float>> data;
    std::map<std::string, std::vector<std::string>> metadata;
    std::string format;
    double timestamp;
    
    std::vector<float> getData(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return std::vector<float>();
    }
    
    std::string getMetadata(const std::string& key) const {
        auto it = metadata.find(key);
        if (it != metadata.end()) {
            return it->second.empty() ? "" : it->second[0];
        }
        return "";
    }
};

// Plugin Visualization Event Types
enum class VisualizationPluginEventType {
    VisualizationCreated,
    VisualizationUpdated,
    VisualizationDestroyed,
    DataChanged,
    StyleChanged,
    FilterApplied,
    ExportRequested,
    InteractionDetected,
    ErrorOccurred
};

// Plugin Visualization Event
struct VisualizationPluginEvent {
    VisualizationPluginEventType type;
    std::string pluginName;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> data;
    VisualizationData visualizationData;
};

// Plugin Visualization Event Callback
using VisualizationPluginEventCallback = std::function<void(const VisualizationPluginEvent&)>;

// Plugin Visualization Settings
struct VisualizationPluginSettings {
    std::string theme;
    std::map<std::string, std::string> style;
    std::map<std::string, std::string> options;
    std::vector<std::string> filters;
    std::map<std::string, float> parameters;
    bool interactive;
    bool animated;
    double updateInterval;
    
    std::string getSetting(const std::string& key, const std::string& defaultValue = "") const {
        auto it = style.find(key);
        if (it != style.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    float getParameter(const std::string& key, float defaultValue = 0.0f) const {
        auto it = parameters.find(key);
        if (it != parameters.end()) {
            return it->second;
        }
        return defaultValue;
    }
};

// Plugin Visualization Interface
class VisualizationPluginInterface : public Plugin {
public:
    virtual ~VisualizationPluginInterface() = default;
    
    // Visualization category
    virtual VisualizationPluginCategory getCategory() const = 0;
    virtual VisualizationType getVisualizationType() const = 0;
    
    // Visualization data
    virtual VisualizationData getVisualizationData() const = 0;
    virtual bool setVisualizationData(const VisualizationData& data) = 0;
    virtual bool updateVisualizationData(const std::string& key, const std::vector<float>& data) = 0;
    
    // Visualization settings
    virtual VisualizationPluginSettings getSettings() const = 0;
    virtual bool setSettings(const VisualizationPluginSettings& settings) = 0;
    virtual bool updateSetting(const std::string& key, const std::string& value) = 0;
    virtual bool updateParameter(const std::string& key, float value) = 0;
    
    // Visualization control
    virtual bool createVisualization() = 0;
    virtual bool updateVisualization(double deltaTime) = 0;
    virtual bool destroyVisualization() = 0;
    virtual bool refreshVisualization() = 0;
    
    // Visualization interaction
    virtual bool handleInteraction(const std::string& event, const std::map<std::string, std::string>& data) = 0;
    virtual bool setInteractionMode(const std::string& mode) = 0;
    virtual std::string getInteractionMode() const = 0;
    
    // Visualization export
    virtual bool exportVisualization(const std::string& filepath, const std::string& format = "png") = 0;
    virtual bool exportVisualizationData(const std::string& filepath, const std::string& format = "json") = 0;
    virtual std::vector<std::string> getExportFormats() const = 0;
    
    // Visualization filtering
    virtual bool addFilter(const std::string& filter) = 0;
    virtual bool removeFilter(const std::string& filter) = 0;
    virtual bool clearFilters() = 0;
    virtual std::vector<std::string> getFilters() const = 0;
    virtual bool applyFilters() = 0;
    
    // Visualization themes
    virtual bool setTheme(const std::string& theme) = 0;
    virtual std::string getTheme() const = 0;
    virtual std::vector<std::string> getAvailableThemes() const = 0;
    
    // Visualization events
    virtual void addVisualizationEventCallback(VisualizationPluginEventCallback callback) = 0;
    virtual void removeVisualizationEventCallback(VisualizationPluginEventCallback callback) = 0;
    virtual void broadcastVisualizationEvent(const VisualizationPluginEvent& event) = 0;
    
    // Visualization configuration
    virtual bool loadConfiguration(const std::string& filepath) = 0;
    virtual bool saveConfiguration(const std::string& filepath) const = 0;
    virtual bool resetConfiguration() = 0;
    
    // Visualization commands
    virtual bool executeVisualizationCommand(const std::string& command,
                                            const std::map<std::string, std::string>& args = {}) = 0;
    
    // Visualization help
    virtual std::string getVisualizationHelp() const = 0;
    virtual std::string getPluginUsage() const = 0;
    
    // Visualization statistics
    virtual std::map<std::string, float> getVisualizationStatistics() const = 0;
    virtual std::map<std::string, float> getPerformanceMetrics() const = 0;
    
    // Visualization plugin specific methods
    virtual bool initializeVisualization() = 0;
    virtual bool shutdownVisualization() = 0;
    virtual bool validateVisualizationConfiguration() = 0;
    virtual bool updateVisualizationParameters(const std::map<std::string, float>& parameters) = 0;
    virtual std::map<std::string, float> getVisualizationParameters() const = 0;
    
    // Event processing
    virtual bool onVisualizationEvent(const VisualizationPluginEvent& event) = 0;
    virtual bool onVisualizationUpdate(double deltaTime) = 0;
    virtual bool onVisualizationError(const std::string& error) = 0;
    
    // Visualization lifecycle
    virtual bool prepareVisualization() = 0;
    virtual bool finalizeVisualization() = 0;
    virtual bool cleanupVisualization() = 0;
    
    // Plugin identification
    virtual std::string getPluginName() const override = 0;
    virtual std::string getVersion() const override = 0;
    virtual std::string getDescription() const override = 0;
    virtual std::string getAuthor() const override = 0;
    
    // Plugin lifecycle
    virtual PluginLoadStatus getLoadStatus() const override = 0;
    virtual bool initialize(const std::map<std::string, PluginConfigValue>& config) override = 0;
    virtual bool update(double deltaTime) override = 0;
    virtual bool shutdown() override = 0;
    virtual bool configure(const std::map<std::string, PluginConfigValue>& config) override = 0;
    
    // Plugin capabilities
    virtual PluginCapabilities getCapabilities() const override = 0;
    virtual bool supportsFeature(const std::string& feature) const override = 0;
    
    // Plugin state management
    virtual std::map<std::string, PluginConfigValue> getState() const override = 0;
    virtual bool setState(const std::map<std::string, PluginConfigValue>& state) override = 0;
    virtual bool saveState(const std::string& filepath) const override = 0;
    virtual bool loadState(const std::string& filepath) override = 0;
    
    // Plugin information
    virtual std::vector<std::string> getDependencies() const override = 0;
    virtual PluginSecurityLevel getSecurityLevel() const override = 0;
    virtual std::vector<std::string> getTags() const override = 0;
    virtual std::string getCategory() const override = 0;
    
    // Plugin statistics
    virtual PluginStatistics getStatistics() const override = 0;
    
    // Plugin event handling
    virtual void addEventCallback(PluginEventCallback callback) override = 0;
    virtual void removeEventCallback(PluginEventCallback callback) override = 0;
    
    // Plugin system integration
    virtual bool integrateWith(std::shared_ptr<Plugin> otherPlugin) override = 0;
    virtual bool removeIntegration(std::shared_ptr<Plugin> otherPlugin) override = 0;
    
    // Plugin validation
    virtual bool validate() const override = 0;
    virtual std::vector<std::string> getValidationErrors() const override = 0;
    
    // Plugin commands
    virtual bool executeCommand(const std::string& command,
                               const std::map<std::string, std::string>& args = {}) override = 0;
    
    // Plugin help
    virtual std::string getHelp() const override = 0;
};

// Plugin Visualization Factory Interface
class VisualizationPluginFactory : public PluginFactory {
public:
    virtual ~VisualizationPluginFactory() = default;
    
    // Create visualization-specific plugin
    virtual std::unique_ptr<VisualizationPluginInterface> createVisualizationPlugin(const std::string& name) = 0;
    
    // Get plugin type
    virtual std::string getPluginType() const override = 0;
    
    // Get metadata
    virtual PluginMetadata getMetadata() const override = 0;
    
    // Check if can create
    virtual bool canCreate(const std::string& name) const override = 0;
    
    // Create regular plugin
    virtual std::unique_ptr<Plugin> createPlugin(const std::string& name) override = 0;
};

// Plugin Visualization Base Class
class VisualizationPluginBase : public VisualizationPluginInterface {
public:
    VisualizationPluginBase(const std::string& name, const std::string& version);
    virtual ~VisualizationPluginBase() = default;
    
    // Plugin identification
    virtual std::string getName() const override;
    virtual std::string getVersion() const override;
    virtual std::string getDescription() const override;
    virtual std::string getAuthor() const override;
    
    // Plugin metadata
    virtual const PluginMetadata& getMetadata() const override;
    
    // Visualization category
    virtual VisualizationPluginCategory getCategory() const override = 0;
    virtual VisualizationType getVisualizationType() const override = 0;
    
    // Visualization data (default implementations)
    virtual VisualizationData getVisualizationData() const override;
    virtual bool setVisualizationData(const VisualizationData& data) override;
    virtual bool updateVisualizationData(const std::string& key, const std::vector<float>& data) override;
    
    // Visualization settings (default implementations)
    virtual VisualizationPluginSettings getSettings() const override;
    virtual bool setSettings(const VisualizationPluginSettings& settings) override;
    virtual bool updateSetting(const std::string& key, const std::string& value) override;
    virtual bool updateParameter(const std::string& key, float value) override;
    
    // Visualization control (default implementations)
    virtual bool createVisualization() override;
    virtual bool updateVisualization(double deltaTime) override;
    virtual bool destroyVisualization() override;
    virtual bool refreshVisualization() override;
    
    // Visualization interaction (default implementations)
    virtual bool handleInteraction(const std::string& event, const std::map<std::string, std::string>& data) override;
    virtual bool setInteractionMode(const std::string& mode) override;
    virtual std::string getInteractionMode() const override;
    
    // Visualization export (default implementations)
    virtual bool exportVisualization(const std::string& filepath, const std::string& format) override;
    virtual bool exportVisualizationData(const std::string& filepath, const std::string& format) override;
    virtual std::vector<std::string> getExportFormats() const override;
    
    // Visualization filtering (default implementations)
    virtual bool addFilter(const std::string& filter) override;
    virtual bool removeFilter(const std::string& filter) override;
    virtual bool clearFilters() override;
    virtual std::vector<std::string> getFilters() const override;
    virtual bool applyFilters() override;
    
    // Visualization themes (default implementations)
    virtual bool setTheme(const std::string& theme) override;
    virtual std::string getTheme() const override;
    virtual std::vector<std::string> getAvailableThemes() const override;
    
    // Visualization lifecycle
    virtual bool prepareVisualization() override;
    virtual bool finalizeVisualization() override;
    virtual bool cleanupVisualization() override;
    
    // Visualization configuration
    virtual bool loadConfiguration(const std::string& filepath) override;
    virtual bool saveConfiguration(const std::string& filepath) const override;
    virtual bool resetConfiguration() override;
    
    // Visualization events
    virtual void addVisualizationEventCallback(VisualizationPluginEventCallback callback) override;
    virtual void removeVisualizationEventCallback(VisualizationPluginEventCallback callback) override;
    virtual void broadcastVisualizationEvent(const VisualizationPluginEvent& event) override;
    
    // Visualization plugin specific methods
    virtual bool initializeVisualization() override;
    virtual bool shutdownVisualization() override;
    virtual bool validateVisualizationConfiguration() override;
    virtual bool updateVisualizationParameters(const std::map<std::string, float>& parameters) override;
    virtual std::map<std::string, float> getVisualizationParameters() const override;
    
    // Event processing
    virtual bool onVisualizationEvent(const VisualizationPluginEvent& event) override;
    virtual bool onVisualizationUpdate(double deltaTime) override;
    virtual bool onVisualizationError(const std::string& error) override;
    
    // Plugin lifecycle callbacks
    virtual void onPluginLoadStarted(const std::string& name) override;
    virtual void onPluginLoadCompleted(const std::string& name, bool success) override;
    virtual void onPluginLoadFailed(const std::string& name, const std::string& error) override;
    
    virtual void onPluginStartRequested(const std::string& name) override;
    virtual void onPluginStartCompleted(const std::string& name, bool success) override;
    virtual void onPluginStartFailed(const std::string& name, const std::string& error) override;
    
    virtual void onPluginStopRequested(const std::string& name) override;
    virtual void onPluginStopCompleted(const std::string& name, bool success) override;
    virtual void onPluginStopFailed(const std::string& name, const std::string& error) override;
    
    virtual void onPluginUnloadRequested(const std::string& name) override;
    virtual void onPluginUnloadCompleted(const std::string& name, bool success) override;
    virtual void onPluginUnloadFailed(const std::string& name, const std::string& error) override;
    
    virtual void onPluginUpdated(const std::string& name, double deltaTime) override;
    virtual void onPluginRecovered(const std::string& name) override;
    
    // System lifecycle callbacks
    virtual void onSystemInitialized() override;
    virtual void onSystemShutdown() override;
    virtual void onSystemError(const std::string& error) override;
    virtual void onSystemWarning(const std::string& warning) override;
    
protected:
    std::string name;
    std::string version;
    PluginMetadata metadata;
    VisualizationPluginSettings settings;
    std::vector<std::string> filters;
    std::string interactionMode;
    std::map<std::string, VisualizationData> visualizationData;
    std::map<std::string, float> visualizationParameters;
    
    std::vector<PluginEventCallback> eventCallbacks;
    std::vector<VisualizationPluginEventCallback> visualizationEventCallbacks;
    
    bool visualizationInitialized;
    PluginLoadStatus loadStatus;
    
    std::map<std::string, PluginConfigValue> config;
    
    PluginStatistics statistics;
    
    std::vector<PluginLifecycleEvent> lifecycleHistory;
};

// Plugin Visualization Registry Interface
class VisualizationPluginRegistry {
public:
    virtual ~VisualizationPluginRegistry() = default;
    
    // Register visualization plugin
    virtual bool registerVisualizationPlugin(std::unique_ptr<VisualizationPluginInterface> plugin) = 0;
    
    // Unregister visualization plugin
    virtual bool unregisterVisualizationPlugin(const std::string& name) = 0;
    
    // Get visualization plugin
    virtual VisualizationPluginInterface* getVisualizationPlugin(const std::string& name) = 0;
    virtual const VisualizationPluginInterface* getVisualizationPlugin(const std::string& name) const = 0;
    
    // Get plugins by category
    virtual std::vector<VisualizationPluginInterface*> getVisualizationPluginsByCategory(VisualizationPluginCategory category) = 0;
    virtual std::vector<VisualizationPluginInterface*> getVisualizationPluginsByType(VisualizationType type) = 0;
    
    // Get all plugins
    virtual std::vector<VisualizationPluginInterface*> getAllVisualizationPlugins() = 0;
    
    // Check if plugin is registered
    virtual bool isVisualizationPluginRegistered(const std::string& name) const = 0;
    
    // Plugin information
    virtual VisualizationPluginCategory getPluginCategory(const std::string& name) const = 0;
    virtual VisualizationType getPluginVisualizationType(const std::string& name) const = 0;
    
    // Plugin validation
    virtual bool validateVisualizationPlugin(const VisualizationPluginInterface& plugin) const = 0;
    virtual std::vector<std::string> getVisualizationPluginValidationErrors(const std::string& name) const = 0;
    
    // Plugin integration
    virtual bool integrateVisualizationPlugins() = 0;
    virtual bool removeVisualizationPluginIntegration(const std::string& name1, const std::string& name2) = 0;
    virtual std::vector<std::pair<std::string, std::string>> getVisualizationPluginIntegrations() const = 0;
    
    // Plugin statistics
    virtual std::map<std::string, float> getPluginStatistics(const std::string& name) const = 0;
    virtual std::map<std::string, float> getPluginPerformance(const std::string& name) const = 0;
    
    // Plugin commands
    virtual bool executeVisualizationPluginCommand(const std::string& name, 
                                                  const std::string& command,
                                                  const std::map<std::string, std::string>& args = {}) = 0;
};

} // namespace nlm