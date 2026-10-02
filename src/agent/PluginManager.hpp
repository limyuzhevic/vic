// PluginManager.hpp - Plugin system for extending NLM functionality
// Allows loading and managing plugins that extend NLM capabilities

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace nlm {

class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Lifecycle management
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const { return initialized_; }
    
    // Configuration
    virtual void configure(const std::map<std::string, std::string>& config) {}
    virtual std::map<std::string, std::string> getConfig() const { return config_; }
    
    // Capabilities
    virtual std::vector<std::string> getCapabilities() const { return capabilities_; }
    virtual bool hasCapability(const std::string& capability) const;
    
    // Event handling
    virtual void onBrainInitialized(std::shared_ptr<class Brain> brain) {}
    virtual void onBrainReset() {}
    virtual void onStepCompleted(SimulationStep step, Timestamp time) {}
    virtual void onCheckpointSaved(const std::string& filepath) {}
    virtual void onCheckpointLoaded(const std::string& filepath) {}
    
protected:
    bool initialized_ = false;
    std::map<std::string, std::string> config_;
    std::vector<std::string> capabilities_;
};

// Plugin factory function type
typedef std::function<std::unique_ptr<PluginInterface>()> PluginFactory;

class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin loading and management
    bool loadPlugin(const std::string& path);
    bool loadPlugin(std::unique_ptr<PluginInterface> plugin);
    bool unloadPlugin(const std::string& name);
    bool unloadPlugin(std::unique_ptr<PluginInterface> plugin);
    
    // Plugin access
    std::unique_ptr<PluginInterface> getPlugin(const std::string& name);
    std::vector<std::string> listPlugins() const;
    
    // Plugin control
    bool enablePlugin(const std::string& name);
    bool disablePlugin(const std::string& name);
    bool isPluginEnabled(const std::string& name) const;
    
    // Event propagation
    void onBrainInitialized(std::shared_ptr<class Brain> brain);
    void onBrainReset();
    void onStepCompleted(SimulationStep step, Timestamp time);
    void onCheckpointSaved(const std::string& filepath);
    void onCheckpointLoaded(const std::string& filepath);
    
    // Configuration
    void configurePlugin(const std::string& name, const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPluginConfig(const std::string& name) const;
    
    // Plugin scanning and discovery
    std::vector<std::string> scanPluginDirectory(const std::string& directory) const;
    
    // Plugin validation
    bool validatePlugin(std::unique_ptr<PluginInterface> plugin) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl_;
    
    // Internal methods
    bool registerPlugin(std::unique_ptr<PluginInterface> plugin);
    void cleanupDisabledPlugins();
};

} // namespace nlm