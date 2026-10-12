#pragma once

#include "PluginAPI.hpp"
#include <memory>
#include <string>

namespace nlm {

class Plugin {
public:
    // Plugin lifecycle
    virtual ~Plugin() = default;
    
    // Get plugin metadata
    virtual const PluginMetadata& getMetadata() const = 0;
    
    // Plugin initialization
    virtual bool initialize(const std::shared_ptr<class PluginManager>& manager, 
                          const std::shared_ptr<class PluginConfig>& config) {
        manager_ = manager;
        config_ = config;
        return onInitialize();
    }
    
    // Plugin cleanup
    virtual void cleanup() {
        onCleanup();
        manager_.reset();
        config_.reset();
    }
    
    // Enable/disable plugin
    virtual void enable() {
        if (getStatus() != PluginStatus::Loaded) {
            throw PluginException("Cannot enable plugin that is not loaded");
        }
        onEnable();
        status_ = PluginStatus::Enabled;
    }
    
    virtual void disable() {
        if (getStatus() != PluginStatus::Enabled) {
            throw PluginException("Cannot disable plugin that is not enabled");
        }
        onDisable();
        status_ = PluginStatus::Disabled;
    }
    
    // Get current status
    PluginStatus getStatus() const { return status_; }
    
    // Get unique identifier for this plugin instance
    std::string getInstanceId() const { return instanceId_; }
    
    // Plugin-specific initialization
    virtual bool onInitialize() { return true; }
    virtual void onEnable() {}
    virtual void onDisable() {}
    virtual void onCleanup() {}
    
    // Plugin-specific functionality
    virtual void update(float dt) {}
    virtual void render() {}
    virtual void onMessage(const std::string& message, const std::string& data) {}
    
    // Plugin discovery interface
    virtual std::vector<std::string> getExtensionPoints() const { return {}; }
    virtual std::vector<std::string> getProvidedServices() const { return {}; }
    
    // Plugin configuration interface
    virtual std::shared_ptr<class PluginConfig> getConfig() const { return config_; }
    virtual void setConfig(const std::shared_ptr<class PluginConfig>& config) {
        config_ = config;
    }
    
    // Plugin manager access
    std::shared_ptr<class PluginManager> getManager() const { return manager_; }
    
protected:
    PluginStatus status_ = PluginStatus::Inactive;
    std::shared_ptr<class PluginManager> manager_;
    std::shared_ptr<class PluginConfig> config_;
    std::string instanceId_;
    
    Plugin() {
        static std::atomic<uint64_t> idCounter{0};
        instanceId_ = "plugin_" + std::to_string(idCounter++);
    }
};

} // namespace nlm