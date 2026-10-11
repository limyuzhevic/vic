#pragma once

#include "Brain.hpp"
#include "PluginManager.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

class PluginIntegratedBrain : public Brain {
public:
    PluginIntegratedBrain(std::shared_ptr<Config> config);
    ~PluginIntegratedBrain() override;
    
    // Plugin management
    void loadPlugins(const std::vector<std::string>& pluginPaths);
    void unloadPlugins();
    void initializePlugins();
    void shutdownPlugins();
    
    // Plugin execution
    void updatePlugins(double deltaTime) override;
    void processPluginMessages();
    void coordinatePluginActions();
    
    // Plugin access
    std::shared_ptr<NLMPlugin> getPlugin(const std::string& name) const;
    std::vector<std::shared_ptr<NLMPlugin>> getPluginsByType(PluginType type) const;
    
    // Plugin configuration
    void configurePlugin(const std::string& name, const PluginConfig& config);
    PluginConfig getPluginConfig(const std::string& name) const;
    
    // Plugin status
    PluginStatus getPluginStatus(const std::string& name) const;
    std::vector<PluginInfo> listPlugins() const;
    
    // Override Brain methods
    bool initialize() override;
    void step(SimulationStep currentStep) override;
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
