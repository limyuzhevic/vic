#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <unordered_set>
#include "../brain/Brain.hpp"
#include "../visualization/VisualizationCore.hpp"
#include "../debugging/DebugConsole.hpp"
#include "../analysis/DataAnalyzer.hpp"
#include "../profiling/PerformanceProfiler.hpp"

namespace nlm {

// Forward declarations
class VisualizationPlugin;

// Plugin interface base class
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Plugin name/identifier
    virtual std::string getName() const = 0;
    
    // Plugin version
    virtual std::string getVersion() const = 0;
    
    // Plugin description
    virtual std::string getDescription() const = 0;
    
    // Plugin author
    virtual std::string getAuthor() const = 0;
    
    // Initialize plugin
    virtual bool initialize() = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Update plugin state
    virtual void update(double deltaTime) = 0;
    
    // Get plugin capabilities
    virtual std::vector<std::string> getCapabilities() const = 0;
    
    // Check if plugin is active
    virtual bool isActive() const = 0;
    
    // Enable/disable plugin
    virtual void setActive(bool active) = 0;
};

// Visualization plugin interface
class VisualizationPlugin : public Plugin {
public:
    virtual ~VisualizationPlugin() = default;
    
    // Get visualization type
    virtual VisualizationType getVisualizationType() const = 0;
    
    // Get visualization data
    virtual std::shared_ptr<VisualizationData> getData() const = 0;
    
    // Set brain reference
    virtual void setBrain(std::shared_ptr<Brain> brain) = 0;
    
    // Get brain reference
    virtual std::shared_ptr<Brain> getBrain() const = 0;
    
    // Update visualization
    virtual void update(const Brain& brain, double deltaTime) = 0;
    
    // Render visualization
    virtual void render() = 0;
    
    // Get visualization settings
    virtual std::map<std::string, std::string> getSettings() const = 0;
    
    // Set visualization settings
    virtual void setSettings(const std::map<std::string, std::string>& settings) = 0;
};

// Data processing plugin interface
class DataProcessingPlugin : public Plugin {
public:
    virtual ~DataProcessingPlugin() = default;
    
    // Process brain data
    virtual std::vector<double> processData(const Brain& brain) = 0;
    
    // Get output format
    virtual std::string getOutputFormat() const = 0;
    
    // Get processed data metadata
    virtual std::map<std::string, std::string> getMetadata() const = 0;
};

// Plugin system manager
class PluginSystem {
public:
    PluginSystem();
    ~PluginSystem();

    // Disable copying, enable moving
    PluginSystem(const PluginSystem&) = delete;
    PluginSystem& operator=(const PluginSystem&) = delete;
    PluginSystem(PluginSystem&&) noexcept;
    PluginSystem& operator=(PluginSystem&&) noexcept;

    // Initialize plugin system
    bool initialize();
    
    // Shutdown plugin system
    void shutdown();
    
    // Check if plugin system is active
    bool isActive() const;
    
    // Register a plugin
    bool registerPlugin(std::unique_ptr<Plugin> plugin);
    
    // Unregister a plugin
    bool unregisterPlugin(const std::string& name);
    
    // Get plugin by name
    Plugin* getPlugin(const std::string& name);
    
    // Get all registered plugins
    std::vector<std::string> getRegisteredPlugins() const;
    
    // Get plugin capabilities
    std::vector<std::string> getPluginCapabilities(const std::string& name) const;
    
    // Enable/disable a plugin
    void enablePlugin(const std::string& name);
    void disablePlugin(const std::string& name);
    bool isPluginEnabled(const std::string& name) const;
    
    // Update all plugins
    void updatePlugins(double deltaTime);
    
    // Get system status
    std::string getSystemStatus() const;
    
    // Save plugin configuration
    bool savePluginConfig(const std::string& filename) const;
    
    // Load plugin configuration
    bool loadPluginConfig(const std::string& filename);
    
    // Get plugin configuration
    std::map<std::string, std::string> getPluginConfig(const std::string& name) const;
    
    // Set plugin configuration
    void setPluginConfig(const std::string& name, const std::map<std::string, std::string>& config);
    
    // Validate plugin
    bool validatePlugin(const Plugin& plugin) const;
    
    // Get loaded plugins info
    struct PluginInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::vector<std::string> capabilities;
        bool enabled;
        bool active;
        
        PluginInfo() : enabled(true), active(false) {}
    };
    
    std::vector<PluginInfo> getPluginsInfo() const;
    
    // Get system resources
    struct SystemResources {
        size_t totalMemory;
        size_t availableMemory;
        double cpuUsage;
        size_t activePluginCount;
        size_t totalPluginCount;
        
        SystemResources() : totalMemory(0), availableMemory(0), cpuUsage(0.0),
                           activePluginCount(0), totalPluginCount(0) {}
    };
    
    SystemResources getSystemResources() const;
    
    // Search for plugins by capability
    std::vector<std::string> findPluginsByCapability(const std::string& capability) const;
    
    // Create plugin instance
    virtual std::unique_ptr<Plugin> createPlugin(const std::string& pluginType, const std::string& name);
    
    // Get plugin factory
    std::function<std::unique_ptr<Plugin>(const std::string&)> getPluginFactory(const std::string& pluginType);
    
    // Register plugin factory
    void registerPluginFactory(const std::string& pluginType, 
                               std::function<std::unique_ptr<Plugin>(const std::string&)> factory);
    
    // Get default plugin configuration
    static std::map<std::string, std::string> getDefaultConfig();
    
    // Validate plugin configuration
    static bool validateConfig(const std::map<std::string, std::string>& config);
    
    // Get plugin system version
    static std::string getVersion();
    
    // Get plugin system build information
    static std::string getBuildInfo();
    
    // Get plugin system license information
    static std::string getLicenseInfo();
    
    // Get plugin system copyright information
    static std::string getCopyrightInfo();
    
    // Get plugin system help
    static std::string getHelp();
    
    // Get plugin system about information
    static std::string getAbout();
    
    // Check for system errors
    bool hasSystemErrors() const;
    
    // Get system error messages
    std::vector<std::string> getSystemErrorMessages() const;
    
    // Clear system error messages
    void clearSystemErrorMessages();
    
    // Get system warning messages
    std::vector<std::string> getSystemWarningMessages() const;
    
    // Clear system warning messages
    void clearSystemWarningMessages();
    
    // Get system information messages
    std::vector<std::string> getSystemInfoMessages() const;
    
    // Clear system information messages
    void clearSystemInfoMessages();
    
    // Log system message
    void logSystemMessage(const std::string& message, const std::string& level = "info");
    
    // Set log level
    void setLogLevel(const std::string& level);
    
    // Get log level
    std::string getLogLevel() const;
    
    // Export plugin system data
    bool exportPluginSystem(const std::string& filename, const std::string& format = "json");
    
    // Import plugin system data
    bool importPluginSystem(const std::string& filename, const std::string& format = "json");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm