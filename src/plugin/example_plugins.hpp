#pragma once

#include "IBrainPlugin.hpp"

namespace nlm {

// Memory plugins
class WorkingMemoryPlugin : public IBrainPlugin {
public:
    WorkingMemoryPlugin();
    ~WorkingMemoryPlugin() override = default;

    bool initialize(std::shared_ptr<const Config> config) override;
    void update(SimulationStep step) override;
    void shutdown() override;

    void setBrain(Brain* brain) override;
    void setPluginSystem(PluginSystem* pluginSystem) override;

    bool configure(const std::string& key, const std::any& value) override;
    std::any getConfig(const std::string& key) const override;

    PluginMetadata getMetadata() const override;
    bool supportsCategory(const std::string& category) const override;
    std::vector<std::string> getRequiredSystems() const override;
    void onEvent(const std::string& eventType, const std::any& eventData) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Episodic memory plugin
class EpisodicMemoryPlugin : public IBrainPlugin {
public:
    EpisodicMemoryPlugin();
    ~EpisodicMemoryPlugin() override = default;

    bool initialize(std::shared_ptr<const Config> config) override;
    void update(SimulationStep step) override;
    void shutdown() override;

    void setBrain(Brain* brain) override;
    void setPluginSystem(PluginSystem* pluginSystem) override;

    bool configure(const std::string& key, const std::any& value) override;
    std::any getConfig(const std::string& key) const override;

    PluginMetadata getMetadata() const override;
    bool supportsCategory(const std::string& category) const override;
    std::vector<std::string> getRequiredSystems() const override;
    void onEvent(const std::string& eventType, const std::any& eventData) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuromodulation plugin - Dopamine
class DopamineNeuromodulationPlugin : public IBrainPlugin {
public:
    DopamineNeuromodulationPlugin();
    ~DopamineNeuromodulationPlugin() override = default;

    bool initialize(std::shared_ptr<const Config> config) override;
    void update(SimulationStep step) override;
    void shutdown() override;

    void setBrain(Brain* brain) override;
    void setPluginSystem(PluginSystem* pluginSystem) override;

    bool configure(const std::string& key, const std::any& value) override;
    std::any getConfig(const std::string& key) const override;

    PluginMetadata getMetadata() const override;
    bool supportsCategory(const std::string& category) const override;
    std::vector<std::string> getRequiredSystems() const override;
    void onEvent(const std::string& eventType, const std::any& eventData) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plasticity plugin - STDP
class STDPPlasticityPlugin : public IBrainPlugin {
public:
    STDPPlasticityPlugin();
    ~STDPPlasticityPlugin() override = default;

    bool initialize(std::shared_ptr<const Config> config) override;
    void update(SimulationStep step) override;
    void shutdown() override;

    void setBrain(Brain* brain) override;
    void setPluginSystem(PluginSystem* pluginSystem) override;

    bool configure(const std::string& key, const std::any& value) override;
    std::any getConfig(const std::string& key) const override;

    PluginMetadata getMetadata() const override;
    bool supportsCategory(const std::string& category) const override;
    std::vector<std::string> getRequiredSystems() const override;
    void onEvent(const std::string& eventType, const std::any& eventData) override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm