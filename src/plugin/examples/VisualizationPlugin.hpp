#pragma once

#include "VisualizationPlugin.hpp"
#include <string>

namespace nlm {

class ExampleVisualizationPlugin : public VisualizationPlugin {
public:
    ExampleVisualizationPlugin();
    ~ExampleVisualizationPlugin() override;
    
    // Plugin identification
    std::string getName() const override { return "ExampleVisualization"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { 
        return "Example visualization plugin with custom visualization"; 
    }
    std::string getAuthor() const override { return "NLM Team"; }
    PluginType getType() const override { return PluginType::VISUALIZATION; }
    
    // Plugin lifecycle
    bool initialize(const PluginConfig& config, void* manager) override;
    void update(double deltaTime) override;
    void shutdown() override;
    
    // Plugin state management
    void pause() override;
    void resume() override;
    PluginState getState() const override;
    
    // Plugin dependencies
    std::vector<std::string> getDependencies() const override;
    bool checkDependencies(const std::unordered_map<std::string, PluginInfo>& plugins) const override;
    
    // Plugin communication
    void receiveMessage(const std::string& sender, const PluginMessage& message) override;
    void sendMessage(const std::string& receiver, const PluginMessage& message) override;
    
    // Plugin configuration
    void configure(const PluginConfig& config) override;
    PluginConfig getConfig() const override;
    
    // Visualization extension
    void updateVisualization(double deltaTime) override;
    void renderVisualization(VisualizationInterface* visualizer) override;
    VisualizationMode getVisualizationMode() const override;
    
    // Visualization parameters
    std::string getOutputFormat() const override;
    double getUpdateInterval() const override;
    bool getEnable3D() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
