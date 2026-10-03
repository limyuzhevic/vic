#pragma once

#include "IBrainPlugin.hpp"

namespace nlm {

// Base implementation for simple plugins
class SimpleBrainPlugin : public IBrainPlugin {
protected:
    std::shared_ptr<const Config> config;
    Brain* brain = nullptr;
    PluginSystem* pluginSystem = nullptr;
    bool initialized = false;

public:
    SimpleBrainPlugin() = default;
    virtual ~SimpleBrainPlugin() = default;

    bool initialize(std::shared_ptr<const Config> config) override {
        this->config = config;
        this->initialized = true;
        return onInitialize(config);
    }

    void update(SimulationStep step) override {
        if (initialized) {
            onUpdate(step);
        }
    }

    void shutdown() override {
        if (initialized) {
            onShutdown();
            initialized = false;
        }
    }

    void setBrain(Brain* brain) override {
        this->brain = brain;
        onBrainSet(brain);
    }

    void setPluginSystem(PluginSystem* pluginSystem) override {
        this->pluginSystem = pluginSystem;
        onPluginSystemSet(pluginSystem);
    }

    bool configure(const std::string& key, const std::any& value) override {
        return onConfigure(key, value);
    }

    std::any getConfig(const std::string& key) const override {
        return onGetConfig(key);
    }

    bool supportsCategory(const std::string& category) const override {
        return onSupportsCategory(category);
    }

    void onEvent(const std::string& eventType, const std::any& eventData) override {
        onEvent(eventType, eventData);
    }

    // Override these methods in derived classes
    virtual bool onInitialize(std::shared_ptr<const Config> config) { return true; }
    virtual void onUpdate(SimulationStep step) {}
    virtual void onShutdown() {}
    virtual void onBrainSet(Brain* brain) {}
    virtual void onPluginSystemSet(PluginSystem* pluginSystem) {}
    virtual bool onConfigure(const std::string& key, const std::any& value) { return false; }
    virtual std::any onGetConfig(const std::string& key) const { return std::any(); }
    virtual bool onSupportsCategory(const std::string& category) const { return false; }
    virtual void onEvent(const std::string& eventType, const std::any& eventData) {}
};

} // namespace nlm