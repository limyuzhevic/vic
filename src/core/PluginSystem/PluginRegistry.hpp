// NLM Plugin System
// Extensible plugin architecture for adding new commands, features, and experiments

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <any>
#include "src/experiments/Experiment.hpp"

namespace nlm {

// Plugin type enumeration
enum class PluginType {
    Experiment,
    Command,
    Feature,
    Module,
    System
};

// Plugin interface - base class for all plugins
class Plugin {
public:
    virtual ~Plugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginType getType() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Initialization and cleanup
    virtual bool initialize(const std::unordered_map<std::string, std::any>& config) = 0;
    virtual void cleanup() = 0;
    
    // Check if plugin is active
    virtual bool isActive() const = 0;
    virtual void setActive(bool active) = 0;
    
// Plugin-specific methods
    virtual void execute(const std::string& command, const std::vector<std::string>& args) = 0;
    virtual std::any getResult(const std::string& query) = 0;
};

// Experiment plugin interface
class ExperimentPlugin : public Plugin {
public:
    virtual ~ExperimentPlugin() = default;
    
    // Experiment-specific interface
    virtual std::shared_ptr<Experiment> createExperiment(const std::string& name) = 0;
    virtual bool validateExperimentConfig(const std::unordered_map<std::string, std::any>& config) = 0;
    virtual std::string getExperimentType() const = 0;
};

// Command plugin interface
class CommandPlugin : public Plugin {
public:
    virtual ~CommandPlugin() = default;
    
    // Command-specific interface
    virtual std::vector<std::string> getCommands() const = 0;
    virtual std::string getCommandHelp(const std::string& command) const = 0;
    virtual bool handleCommand(const std::string& command, const std::vector<std::string>& args) = 0;
};

// Feature plugin interface
class FeaturePlugin : public Plugin {
public:
    virtual ~FeaturePlugin() = default;
    
    // Feature-specific interface
    virtual bool requiresConfiguration() const = 0;
    virtual std::unordered_map<std::string, std::any> getDefaultConfig() const = 0;
    virtual void applyFeature(const std::unordered_map<std::string, std::any>& config) = 0;
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<Plugin>()>;
using ExperimentFactory = std::function<std::unique_ptr<ExperimentPlugin>()>;
using CommandFactory = std::function<std::unique_ptr<CommandPlugin>()>;
using FeatureFactory = std::function<std::unique_ptr<FeaturePlugin>()>;

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string description;
    std::string version;
    PluginType type;
    std::vector<std::string> tags;
    std::string author;
    std::string license;
    std::unordered_map<std::string, std::any> configSchema;
    
    PluginMetadata() = default;
    PluginMetadata(const std::string& name, const std::string& description,
                  const std::string& version, PluginType type)
        : name(name), description(description), version(version), type(type) {}
};

// Plugin registry - manages all available plugins
class PluginRegistry {
public:
    virtual ~PluginRegistry() = default;
    
    // Plugin registration
    virtual bool registerPlugin(const PluginMetadata& metadata, PluginFactory factory) = 0;
    virtual bool registerPlugin(const PluginMetadata& metadata, ExperimentFactory factory) = 0;
    virtual bool registerPlugin(const PluginMetadata& metadata, CommandFactory factory) = 0;
    virtual bool registerPlugin(const PluginMetadata& metadata, FeatureFactory factory) = 0;
    
    // Plugin discovery and management
    virtual std::vector<PluginMetadata> getPlugins() const = 0;
    virtual std::vector<PluginMetadata> getPlugins(PluginType type) const = 0;
    virtual std::vector<PluginMetadata> getPluginsByTag(const std::string& tag) const = 0;
    virtual PluginMetadata getPluginMetadata(const std::string& name) const = 0;
    
    // Plugin lifecycle management
    virtual bool loadPlugin(const std::string& name) = 0;
    virtual bool unloadPlugin(const std::string& name) = 0;
    virtual bool activatePlugin(const std::string& name) = 0;
    virtual bool deactivatePlugin(const std::string& name) = 0;
    virtual bool isPluginLoaded(const std::string& name) const = 0;
    virtual bool isPluginActive(const std::string& name) const = 0;
    
    // Plugin execution
    virtual bool executePluginCommand(const std::string& pluginName, const std::string& command,
                                    const std::vector<std::string>& args) = 0;
    virtual std::any getPluginResult(const std::string& pluginName, const std::string& query) = 0;
    
    // Configuration management
    virtual bool configurePlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config) = 0;
    virtual std::unordered_map<std::string, std::any> getPluginConfig(const std::string& name) const = 0;
    
    // Plugin validation
    virtual bool validatePluginConfig(const std::string& name, const std::unordered_map<std::string, std::any>& config) const = 0;
    virtual std::vector<std::string> getPluginErrors(const std::string& name) const = 0;
};

// Default PluginRegistry implementation
class DefaultPluginRegistry : public PluginRegistry {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    DefaultPluginRegistry();
    ~DefaultPluginRegistry() override;
    
    // Plugin registration
    bool registerPlugin(const PluginMetadata& metadata, PluginFactory factory) override;
    bool registerPlugin(const PluginMetadata& metadata, ExperimentFactory factory) override;
    bool registerPlugin(const PluginMetadata& metadata, CommandFactory factory) override;
    bool registerPlugin(const PluginMetadata& metadata, FeatureFactory factory) override;
    
    // Plugin discovery and management
    std::vector<PluginMetadata> getPlugins() const override;
    std::vector<PluginMetadata> getPlugins(PluginType type) const override;
    std::vector<PluginMetadata> getPluginsByTag(const std::string& tag) const override;
    PluginMetadata getPluginMetadata(const std::string& name) const override;
    
    // Plugin lifecycle management
    bool loadPlugin(const std::string& name) override;
    bool unloadPlugin(const std::string& name) override;
    bool activatePlugin(const std::string& name) override;
    bool deactivatePlugin(const std::string& name) override;
    bool isPluginLoaded(const std::string& name) const override;
    bool isPluginActive(const std::string& name) const override;
    
    // Plugin execution
    bool executePluginCommand(const std::string& pluginName, const std::string& command,
                            const std::vector<std::string>& args) override;
    std::any getPluginResult(const std::string& pluginName, const std::string& query) override;
    
    // Configuration management
    bool configurePlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config) override;
    std::unordered_map<std::string, std::any> getPluginConfig(const std::string& name) const override;
    
    // Plugin validation
    bool validatePluginConfig(const std::string& name, const std::unordered_map<std::string, std::any>& config) const override;
    std::vector<std::string> getPluginErrors(const std::string& name) const override;
};

// Plugin manager - provides a unified interface for plugin operations
class PluginManager {
private:
    std::shared_ptr<PluginRegistry> registry;
    
public:
    explicit PluginManager(std::shared_ptr<PluginRegistry> registry);
    ~PluginManager() = default;
    
    // Plugin discovery
    std::vector<PluginMetadata> listPlugins() const;
    std::vector<PluginMetadata> listPlugins(PluginType type) const;
    PluginMetadata getPluginInfo(const std::string& name) const;
    
    // Plugin lifecycle
    bool loadPlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config = {});
    bool unloadPlugin(const std::string& name);
    bool activatePlugin(const std::string& name);
    bool deactivatePlugin(const std::string& name);
    
    // Plugin execution
    bool runCommand(const std::string& pluginName, const std::string& command,
                   const std::vector<std::string>& args = {});
    std::any queryPlugin(const std::string& pluginName, const std::string& query);
    
    // Configuration management
    bool configurePlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config);
    std::unordered_map<std::string, std::any> getPluginConfig(const std::string& name) const;
    
    // Plugin validation
    bool validatePlugin(const std::string& name, const std::unordered_map<std::string, std::any>& config) const;
    
    // Plugin statistics
    size_t getLoadedPluginCount() const;
    size_t getActivePluginCount() const;
    double getTotalPluginMemoryUsage() const;
    
    // Utility methods
    bool exportPluginList(const std::string& filename) const;
    bool importPluginList(const std::string& filename);
    bool backupPlugins(const std::string& directory) const;
    bool restorePlugins(const std::string& directory);
};}
