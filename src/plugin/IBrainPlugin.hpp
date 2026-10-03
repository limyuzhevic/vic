#pragma once

#include <string>
#include <memory>
#include <any>
#include "../core/Types/Types.hpp"

namespace nlm {

// Plugin metadata for discovery and registration
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string category; // memory, prediction, cognition, neuromodulation, development, plasticity
    bool isCore = false;   // Whether this is a built-in plugin
};

// Base interface for all brain plugins
class IBrainPlugin {
public:
    virtual ~IBrainPlugin() = default;

    // Plugin lifecycle management
    virtual bool initialize(std::shared_ptr<const class Config> config) = 0;
    virtual void update(SimulationStep step) = 0;
    virtual void shutdown() = 0;

    // Integration hooks with brain systems
    virtual void setBrain(class Brain* brain) = 0;
    virtual void setPluginSystem(class PluginSystem* pluginSystem) = 0;

    // Configuration interface
    virtual bool configure(const std::string& key, const std::any& value) = 0;
    virtual std::any getConfig(const std::string& key) const = 0;

    // Plugin metadata
    virtual PluginMetadata getMetadata() const = 0;

    // Plugin capabilities
    virtual bool supportsCategory(const std::string& category) const = 0;
    virtual std::vector<std::string> getRequiredSystems() const = 0;

    // Event system integration
    virtual void onEvent(const std::string& eventType, const std::any& eventData) = 0;
};

} // namespace nlm