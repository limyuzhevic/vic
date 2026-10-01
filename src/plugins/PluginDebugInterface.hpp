#pragma once

// Plugin Debug Interface for NLM (Neural Learning Machine)
// Defines the interface for debug plugins

#include "PluginCore.hpp"
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <chrono>

namespace nlm {

// Plugin Debug Categories
enum class DebugPluginCategory {
    Neural,           // Neural network debugging
    Memory,          // Memory system debugging
    Plasticity,      // Plasticity debugging
    Neuromodulation, // Neuromodulation debugging
    Cognitive,       // Cognitive function debugging
    Sensory,         // Sensory system debugging
    Motor,           // Motor system debugging
    Development,     // Development debugging
    Performance,     // Performance debugging
    Integration,     // Integration debugging
    State,           // State debugging
    Error,           // Error debugging
    Log,             // Logging debugging
    Monitoring,      // Monitoring debugging
    Validation,      // Validation debugging
    Optimization     // Optimization debugging
};

// Plugin Debug Levels
enum class DebugLevel {
    Off,         // No debugging
    Error,       // Only error messages
    Warning,     // Error and warning messages
    Info,        // Error, warning, and info messages
    Debug,       // Error, warning, info, and debug messages
    Trace,       // Error, warning, info, debug, and trace messages
    Verbose,     // All messages including verbose information
    Detailed    // Maximum detail debugging
};

// Plugin Debug Event Types
enum class DebugPluginEventType {
    ErrorOccurred,
    WarningIssued,
    InfoMessage,
    DebugMessage,
    TraceMessage,
    PerformanceMetric,
    StateChanged,
    MemoryUsage,
    CPUUsage,
    NetworkUsage,
    PluginLoaded,
    PluginUnloaded,
    PluginStarted,
    PluginStopped,
    PluginUpdated,
    ValidationFailed,
    OptimizationCompleted,
    SystemReset,
    ConfigurationChanged,
    IntegrationUpdated
};

// Plugin Debug Event
struct DebugPluginEvent {
    DebugPluginEventType type;
    std::string pluginName;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::map<std::string, std::string> data;
    DebugLevel severity;
    bool systemEvent;
};

// Plugin Debug Event Callback
using DebugPluginEventCallback = std::function<void(const DebugPluginEvent&)>;

// Plugin Debug Filter
struct DebugPluginFilter {
    std::string name;
    std::vector<std::string> pluginNames;
    std::vector<DebugPluginEventType> eventTypes;
    DebugLevel minLevel;
    std::map<std::string, std::string> conditions;
    bool enabled;
    
    bool matches(const DebugPluginEvent& event) const {
        // Check if plugin name matches
        if (!pluginNames.empty()) {
            if (std::find(pluginNames.begin(), pluginNames.end(), event.pluginName) == pluginNames.end()) {
                return false;
            }
        }
        
        // Check if event type matches
        if (!eventTypes.empty()) {
            if (std::find(eventTypes.begin(), eventTypes.end(), event.type) == eventTypes.end()) {
                return false;
            }
        }
        
        // Check if severity matches
        if (event.severity < minLevel) {
            return false;
        }
        
        // TODO: Check additional conditions
        
        return true;
    }
};

// Plugin Debug Output
struct DebugOutput {
    std::string pluginName;
    DebugLevel level;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> stackTrace;
    
    std::string toString() const {
        std::stringstream ss;
        ss << "[" << std::chrono::system_clock::to_time_t(timestamp) << "] "
           << "[" << getDebugLevelString(level) << "] "
           << "[" << pluginName << "] "
           << message;
        
        if (!metadata.empty()) {
            ss << " ";
            bool first = true;
            for (const auto& pair : metadata) {
                if (!first) ss << ", ";
                ss << pair.first << "=" << pair.second;
                first = false;
            }
        }
        
        if (!stackTrace.empty()) {
            ss << "\nStack Trace:";
            for (const std::string& trace : stackTrace) {
                ss << "\n  " << trace;
            }
        }
        
        return ss.str();
    }
};

// Plugin Debug Manager
class PluginDebugManager {
public:
    virtual ~PluginDebugManager() = default;
    
    // Debug event handling
    virtual void logMessage(const std::string& pluginName, DebugLevel level, const std::string& message,
                           const std::map<std::string, std::string>& metadata = {}) = 0;
    virtual void logError(const std::string& pluginName, const std::string& message,
                         const std::vector<std::string>& stackTrace = {}) = 0;
    virtual void logWarning(const std::string& pluginName, const std::string& message) = 0;
    virtual void logInfo(const std::string& pluginName, const std::string& message) = 0;
    virtual void logDebug(const std::string& pluginName, const std::string& message) = 0;
    virtual void logTrace(const std::string& pluginName, const std::string& message) = 0;
    
    // Debug event filtering
    virtual void addFilter(const DebugPluginFilter& filter) = 0;
    virtual void removeFilter(const std::string& name) = 0;
    virtual DebugPluginFilter getFilter(const std::string& name) const = 0;
    virtual std::vector<DebugPluginFilter> getFilters() const = 0;
    virtual bool updateFilters() = 0;
    
    // Debug output management
    virtual void addOutput(const DebugOutput& output) = 0;
    virtual std::vector<DebugOutput> getOutputs() const = 0;
    virtual std::vector<DebugOutput> getOutputs(const std::string& pluginName) const = 0;
    virtual std::vector<DebugOutput> getOutputs(DebugLevel minLevel) const = 0;
    virtual void clearOutputs() = 0;
    virtual void clearOutputs(const std::string& pluginName) = 0;
    
    // Debug state management
    virtual bool saveDebugState(const std::string& filepath) const = 0;
    virtual bool loadDebugState(const std::string& filepath) = 0;
    virtual bool clearDebugState() = 0;
    
    // Debug configuration
    virtual bool setDebugLevel(const std::string& pluginName, DebugLevel level) = 0;
    virtual DebugLevel getDebugLevel(const std::string& pluginName) const = 0;
    virtual bool setGlobalDebugLevel(DebugLevel level) = 0;
    virtual DebugLevel getGlobalDebugLevel() const = 0;
    virtual std::map<std::string, DebugLevel> getPluginDebugLevels() const = 0;
    
    // Debug performance
    virtual double getDebugPerformance(const std::string& pluginName) const = 0;
    virtual double getSystemDebugPerformance() const = 0;
    virtual std::map<std::string, double> getPluginDebugPerformance() const = 0;
    
    // Debug commands
    virtual bool executeDebugCommand(const std::string& command,
                                   const std::map<std::string, std::string>& args = {}) = 0;
    
    // Debug information
    virtual std::string getDebugStatus() const = 0;
    virtual std::string getDebugInfo() const = 0;
    virtual std::vector<std::string> getDebugMessages() const = 0;
    virtual std::vector<std::string> getDebugWarnings() const = 0;
    virtual std::vector<std::string> getDebugErrors() const = 0;
    
    // Debug about information
    virtual std::string getAbout() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getBuildInfo() const = 0;
    
    // Debug event handling
    virtual void onDebugEvent(const DebugPluginEvent& event) = 0;
    virtual void onDebugOutput(const DebugOutput& output) = 0;
    virtual void onDebugLevelChanged(const std::string& pluginName, DebugLevel level) = 0;
    virtual void onDebugFilterAdded(const DebugPluginFilter& filter) = 0;
    virtual void onDebugFilterRemoved(const std::string& name) = 0;
    
    // System event handling
    virtual void onSystemInitialized() = 0;
    virtual void onSystemShutdown() = 0;
    virtual void onSystemError(const std::string& error) = 0;
    virtual void onSystemWarning(const std::string& warning) = 0;
};

// Plugin Debug Base Class
class DebugPluginBase : public PluginDebugManager {
public:
    DebugPluginBase(const std::string& name, const std::string& version);
    virtual ~DebugPluginBase() = default;
    
    // Plugin identification
    virtual std::string getName() const override;
    virtual std::string getVersion() const override;
    virtual std::string getDescription() const override;
    virtual std::string getAuthor() const override;
    
    // Plugin metadata
    virtual const PluginMetadata& getMetadata() const override;
    
    // Debug category
    virtual DebugPluginCategory getCategory() const override = 0;
    
    // Debug level management
    virtual bool setDebugLevel(DebugLevel level) = 0;
    virtual DebugLevel getDebugLevel() const = 0;
    
    // Debug output (default implementations)
    virtual void logMessage(const std::string& pluginName, DebugLevel level, const std::string& message,
                           const std::map<std::string, std::string>& metadata = {}) override;
    virtual void logError(const std::string& pluginName, const std::string& message,
                         const std::vector<std::string>& stackTrace = {}) override;
    virtual void logWarning(const std::string& pluginName, const std::string& message) override;
    virtual void logInfo(const std::string& pluginName, const std::string& message) override;
    virtual void logDebug(const std::string& pluginName, const std::string& message) override;
    virtual void logTrace(const std::string& pluginName, const std::string& message) override;
    
    // Debug event filtering (default implementations)
    virtual void addFilter(const DebugPluginFilter& filter) override;
    virtual void removeFilter(const std::string& name) override;
    virtual DebugPluginFilter getFilter(const std::string& name) const override;
    virtual std::vector<DebugPluginFilter> getFilters() const override;
    virtual bool updateFilters() override;
    
    // Debug output management (default implementations)
    virtual void addOutput(const DebugOutput& output) override;
    virtual std::vector<DebugOutput> getOutputs() const override;
    virtual std::vector<DebugOutput> getOutputs(const std::string& pluginName) const override;
    virtual std::vector<DebugOutput> getOutputs(DebugLevel minLevel) const override;
    virtual void clearOutputs() override;
    virtual void clearOutputs(const std::string& pluginName) override;
    
    // Debug state management (default implementations)
    virtual bool saveDebugState(const std::string& filepath) const override;
    virtual bool loadDebugState(const std::string& filepath) override;
    virtual bool clearDebugState() override;
    
    // Debug configuration (default implementations)
    virtual bool setDebugLevel(const std::string& pluginName, DebugLevel level) override;
    virtual DebugLevel getDebugLevel(const std::string& pluginName) const override;
    virtual bool setGlobalDebugLevel(DebugLevel level) override;
    virtual DebugLevel getGlobalDebugLevel() const override;
    virtual std::map<std::string, DebugLevel> getPluginDebugLevels() const override;
    
    // Debug performance (default implementations)
    virtual double getDebugPerformance(const std::string& pluginName) const override;
    virtual double getSystemDebugPerformance() const override;
    virtual std::map<std::string, double> getPluginDebugPerformance() const override;
    
    // Debug commands (default implementations)
    virtual bool executeDebugCommand(const std::string& command,
                                   const std::map<std::string, std::string>& args = {}) override;
    
    // Debug information (default implementations)
    virtual std::string getDebugStatus() const override;
    virtual std::string getDebugInfo() const override;
    virtual std::vector<std::string> getDebugMessages() const override;
    virtual std::vector<std::string> getDebugWarnings() const override;
    virtual std::vector<std::string> getDebugErrors() const override;
    
    // Debug about information (default implementations)
    virtual std::string getAbout() const override;
    virtual std::string getVersion() const override;
    virtual std::string getBuildInfo() const override;
    
    // Debug event handling (default implementations)
    virtual void onDebugEvent(const DebugPluginEvent& event) override;
    virtual void onDebugOutput(const DebugOutput& output) override;
    virtual void onDebugLevelChanged(const std::string& pluginName, DebugLevel level) override;
    virtual void onDebugFilterAdded(const DebugPluginFilter& filter) override;
    virtual void onDebugFilterRemoved(const std::string& name) override;
    
    // System event handling (default implementations)
    virtual void onSystemInitialized() override;
    virtual void onSystemShutdown() override;
    virtual void onSystemError(const std::string& error) override;
    virtual void onSystemWarning(const std::string& warning) override;
    
    // Plugin lifecycle (default implementations)
    virtual bool prepareDebug() = 0;
    virtual bool finalizeDebug() = 0;
    virtual bool cleanupDebug() = 0;
    
    // Debug configuration
    virtual bool loadConfiguration(const std::string& filepath) = 0;
    virtual bool saveConfiguration(const std::string& filepath) const = 0;
    virtual bool resetConfiguration() = 0;
    
    // Debug validation
    virtual bool validateDebugConfiguration() = 0;
    virtual std::vector<std::string> getDebugValidationErrors() const = 0;
    
    // Debug statistics
    virtual std::map<std::string, float> getDebugStatistics() const = 0;
    virtual std::map<std::string, float> getDebugPerformanceMetrics() const = 0;
    
    // Debug plugin specific methods
    virtual bool initializeDebug() = 0;
    virtual bool shutdownDebug() = 0;
    virtual bool validateDebugSetup() = 0;
    virtual bool updateDebugParameters(const std::map<std::string, float>& parameters) = 0;
    virtual std::map<std::string, float> getDebugParameters() const = 0;
    
    // Event processing
    virtual bool onDebugEvent(const DebugPluginEvent& event) = 0;
    virtual bool onDebugUpdate(double deltaTime) = 0;
    virtual bool onDebugError(const std::string& error) = 0;
    
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
    DebugLevel globalDebugLevel;
    DebugLevel defaultDebugLevel;
    
    std::map<std::string, DebugLevel> pluginDebugLevels;
    
    std::vector<DebugPluginFilter> filters;
    std::vector<DebugOutput> outputs;
    
    bool debugInitialized;
    PluginLoadStatus loadStatus;
    
    std::map<std::string, PluginConfigValue> config;
    
    std::map<std::string, double> debugPerformance;
    
    PluginStatistics statistics;
    
    std::vector<PluginLifecycleEvent> lifecycleHistory;
};

// Plugin Debug Registry Interface
class DebugPluginRegistry {
public:
    virtual ~DebugPluginRegistry() = default;
    
    // Register debug plugin
    virtual bool registerDebugPlugin(std::unique_ptr<DebugPluginBase> plugin) = 0;
    
    // Unregister debug plugin
    virtual bool unregisterDebugPlugin(const std::string& name) = 0;
    
    // Get debug plugin
    virtual DebugPluginBase* getDebugPlugin(const std::string& name) = 0;
    virtual const DebugPluginBase* getDebugPlugin(const std::string& name) const = 0;
    
    // Get plugins by category
    virtual std::vector<DebugPluginBase*> getDebugPluginsByCategory(DebugPluginCategory category) = 0;
    
    // Get all plugins
    virtual std::vector<DebugPluginBase*> getAllDebugPlugins() = 0;
    
    // Check if plugin is registered
    virtual bool isDebugPluginRegistered(const std::string& name) const = 0;
    
    // Plugin information
    virtual DebugPluginCategory getPluginCategory(const std::string& name) const = 0;
    
    // Plugin validation
    virtual bool validateDebugPlugin(const DebugPluginBase& plugin) const = 0;
    virtual std::vector<std::string> getDebugPluginValidationErrors(const std::string& name) const = 0;
    
    // Plugin integration
    virtual bool integrateDebugPlugins() = 0;
    virtual bool removeDebugPluginIntegration(const std::string& name1, const std::string& name2) = 0;
    virtual std::vector<std::pair<std::string, std::string>> getDebugPluginIntegrations() const = 0;
    
    // Plugin statistics
    virtual std::map<std::string, float> getPluginStatistics(const std::string& name) const = 0;
    virtual std::map<std::string, float> getPluginPerformance(const std::string& name) const = 0;
    
    // Plugin commands
    virtual bool executeDebugPluginCommand(const std::string& name, 
                                          const std::string& command,
                                          const std::map<std::string, std::string>& args = {}) = 0;
};

} // namespace nlm